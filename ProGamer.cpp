#include "ProGamer.h"

#define INPUT_MASK(_X_) (1 << (7 - _X_))

bool ProGamer::isPressed(uint8_t input)
{
  return pressedInputs & INPUT_MASK(input);
}

bool ProGamer::isHeld(uint8_t input)
{
  return heldInputs & INPUT_MASK(input);
}

void ProGamer::setFramelength(int value)
{
  frameLength = value;
}

int ProGamer::getFramelength()
{
  return frameLength;
}

void ProGamer::update()
{
  int processTimer = 0;
  while(processTimer < frameLength) {
    updateDisplay();
    updateAudio();
    tick++;
    processTimer += REFRESH_TIME;
    delay(REFRESH_TIME);
  }

  if(renderMode == RM_SCORE) {
    appendColumn(Gamer::image, 0);
    //Check if right half of screen is empty
    bool empty = true;
    for(int j=8; j<16; j++) {
      if(image[j]) {
        empty = false;
        break;
      }
    }

    if(empty) {
      renderVar /= 10;
      showScore(renderVar);
    }
  }

  updateInputs();
}

void ProGamer::allOn()
{
  for(int j=0; j<16; j++) {
    image[j] = 1;
  }
  renderMode = RM_NONE;
}

void ProGamer::clear()
{
  for(int j=0; j<16; j++) {
    image[j] = 0;
  }
  renderMode = RM_NONE;
}

void ProGamer::setPixel(int x, int y, byte colour)
{
  int shift = 6 - 2 * (x % 4);
  byte mask = 0xFF ^ (0b11 << shift);
  int index = 2 * y + (int)(x / 4);
  image[index] &= mask;
  image[index] |= (colour << shift);
  renderMode = RM_NONE;
}

byte ProGamer::getPixel(int x, int y)
{
  int shift = 6 - 2 * (x % 4);
  int index = 2 * y + (int)(x / 4);
  return (image[index] >> shift) & 0b11;
}

void ProGamer::printImage(byte *img)
{
  Gamer::printImage(img);
  copyBaseDisplay();
  renderMode = RM_NONE;
}

void ProGamer::printImage(byte *img, int x, int y)
{
  Gamer::printImage(img, x, y);
  copyBaseDisplay();
  renderMode = RM_NONE;
}

void ProGamer::printString(String string)
{
  Gamer::printString(string);
  renderMode = RM_NONE;
}

void ProGamer::showScore(int n)
{
  if(n < 100) {
    Gamer::showScore(n);
    renderMode = RM_NONE;
  }
  else {
    Gamer::showScore(n / (pow(10, floor(log(n / 10)))));
    renderMode = RM_SCORE;
  }
  copyBaseDisplay();
}

void ProGamer::appendColumn(byte *screen, byte col)
{
  //Gamer::appendColumn(screen, col); //DON'T USE THIS! IT INCURS A DELAY!
  copyBaseDisplay();
}

//void ProGamer::playTrack(Note track[], int beatLength) {playTrack(track, beatLength, 0);}

void ProGamer::playTrack(int trackSize, Note track[], int beatLength, int pitchModifier)
{
  currentTrack = track;
  this->beatLength = beatLength;
  this->pitchModifier = pitchModifier;
  trackEndIdx = trackSize;

  trackIdx = -1;
}

void ProGamer::setSoundOn(bool value)
{
  soundOn = value;
  if(!value)
    Gamer::stopTone();
}

bool ProGamer::isSoundOn()
{
    return soundOn;
}

bool ProGamer::colourToBinaryDigit(byte colour)
{
  switch(colour) {
    case ZERO: return 0;
    case DARK:
      return tick % (DARK_TICKS_OFF + DARK_TICKS_ON) < DARK_TICKS_ON;
    case LIGHT:
      return tick % (LIGHT_TICKS_OFF + LIGHT_TICKS_ON) < LIGHT_TICKS_ON;
    case ONE: return 1;
  }
}

void ProGamer::updateInputs()
{
  bool capHeldLastFrame = isHeld(CAPTOUCH);

  //Directions and Start
  for(int i=UP; i<START+1; i++) {
    pressedInputs <<= 1;
    heldInputs <<= 1;
    pressedInputs |= Gamer::isPressed(i);
    heldInputs |= Gamer::isHeld(i);
  }

  //LDR (?)
  pressedInputs <<= 1;
  heldInputs <<= 1;

  //Capacitive touch
  pressedInputs <<= 1;
  heldInputs <<= 1;
  if(Gamer::capTouch()) {
    heldInputs |= 1;
    if(!capHeldLastFrame) {
      pressedInputs |= 1;
    }
  }

  pressedInputs <<= 1;
  heldInputs <<= 1;
}

void ProGamer::updateDisplay()
{
  for(int j=0; j<8; j++) {
    for(int i=0; i<8; i++) {
      Gamer::image[j] <<= 1;
      Gamer::image[j] |= colourToBinaryDigit(getPixel(i, j));
    }
  }
}

void ProGamer::updateAudio()
{
  if(!currentTrack)
    return;
  
  bool playNextTone = false;
  if(trackIdx < 0) {
    trackIdx = 0;
    noteTime = currentTrack[0].duration * beatLength;
    playNextTone = true;
  }
  else {
    noteTime -= REFRESH_TIME;
    while(noteTime < 0) {
      trackIdx++;
      if(trackIdx == trackEndIdx) {
        currentTrack = NULL;
        Gamer::stopTone();
        return;
      }
      noteTime += currentTrack[trackIdx].duration * beatLength;
      playNextTone = true;
    }
  }

  if(playNextTone && isSoundOn()) {
    int pitch = currentTrack[trackIdx].value;
    if(pitch >= 50)
      Gamer::playTone(pitch);
    else
      Gamer::stopTone();
  }
}

void ProGamer::copyBaseDisplay()
{
  for(int j=0; j<8; j++) {
    for(int i=0; i<8; i++) {
      byte pixelValue = (Gamer::image[j] >> (7 - i)) & 0b1;
      setPixel(i, j, pixelValue ? ONE : ZERO);
    }
  }
}

/**
  All the letters in the world.
 */
/*
const uint8_t ProGamer::allLetters[85][9] = {
  {B00000000,B00000000,B00000000,LETEND},   // space
  {B01111110,B10010000,B10010000,B10010000,B01111110,B00000000,LETEND}, // A
  {B11111110,B10010010,B10010010,B10010010,B01101100,B00000000,LETEND}, // B
  {B01111100,B10000010,B10000010,B10000010,B01000100,B00000000,LETEND}, // C
  {B11111110,B10000010,B10000010,B01000100,B00111000,B00000000,LETEND}, // D
  {B11111110,B10010010,B10010010,B10010010,B10000010,B00000000,LETEND}, // E
  {B11111110,B10010000,B10010000,B10010000,B10000000,B00000000,LETEND}, // F
  {B01111100,B10000010,B10001010,B10001100,B01001110,B00000000,LETEND}, // G
  {B11111110,B00010000,B00010000,B00010000,B11111110,B00000000,LETEND}, // H
  {B10000010,B11111110,B10000010,B00000000,LETEND}, // I
  {B00000100,B00000010,B10000010,B11111100,B10000000,B00000000,LETEND}, // J
  {B11111110,B00010000,B00101000,B01000100,B10000010,B00000000,LETEND}, // K
  {B11111110,B00000010,B00000010,B00000010,B00000010,B00000000,LETEND}, // L
  {B11111110,B01000000,B00100000,B01000000,B11111110,B00000000,LETEND}, // M
  {B11111110,B01100000,B00010000,B00001100,B11111110,B00000000,LETEND}, // N
  {B01111100,B10000010,B10000010,B10000010,B01111100,B00000000,LETEND}, // O
  {B11111110,B10010000,B10010000,B10010000,B01100000,B00000000,LETEND}, // P
  {B01111100,B10000010,B10000010,B10000100,B01111010,B00000000,LETEND}, // Q
  {B11111110,B10010000,B10011000,B10010100,B01100010,B00000000,LETEND}, // R
  {B01100100,B10010010,B10010010,B10010010,B01001100,B00000000,LETEND}, // S
  {B10000000,B10000000,B11111110,B10000000,B10000000,B00000000,LETEND}, // T
  {B11111100,B00000010,B00000010,B00000010,B11111100,B00000000,LETEND}, // U
  {B11100000,B00011000,B00000110,B00011000,B11100000,B00000000,LETEND}, // V
  {B11111100,B00000010,B00011100,B00000010,B11111100,B00000000,LETEND}, // W
  {B10000010,B01101100,B00010000,B01101100,B10000010,B00000000,LETEND}, // X
  {B11000000,B00100000,B00011110,B00100000,B11000000,B00000000,LETEND}, // Y
  {B10000110,B10001010,B10010010,B10100010,B11000010,B00000000,LETEND}, // Z
  {B00011100,B00100010,B00100010,B00011110,B00000000,LETEND}, // a
  {B01111110,B00010010,B00010010,B00001100,B00000000,LETEND}, // b
  {B00011100,B00100010,B00100010,B00010010,B00000000,LETEND}, // c
  {B00001100,B00010010,B00010010,B11111110,B00000000,LETEND}, // d
  {B00011100,B00101010,B00101010,B00011010,B00000000,LETEND}, // e
  {B00111110,B01001000,B01000000,B00000000,LETEND}, // f
  {B00001101,B00010101,B00011110,B00000000,LETEND}, // g
  {B01111110,B00010000,B00001110,B00000000,LETEND}, // h
  {B01011110,B00000000,LETEND}, // i
  {B00000010,B00000010,B01011100,B00000000,LETEND}, // j
  {B11111110,B00011000,B00100110,B00000000,LETEND}, // k
  {B01111100,B00000010,B00000010,B00000000,LETEND}, // l
  {B00011110,B00100000,B00011000,B00100000,B00011110,B00000000,LETEND}, // m
  {B00111110,B00010000,B00100000,B00011110,B00000000,LETEND}, // n
  {B00011100,B00100010,B00100010,B00011100,B00000000,LETEND}, // o
  {B00111111,B00100100,B00100100,B00011000,B00000000,LETEND}, // p
  {B00011000,B00100100,B00100100,B00111111,B00000000,LETEND}, // q
  {B00111110,B00010000,B00100000,B00100000,B00000000,LETEND}, // r
  {B00010010,B00101010,B00101010,B00100100,B00000000,LETEND}, // s
  {B01111100,B00100010,B00000010,B00000000,LETEND}, // t
  {B00111100,B00000010,B00111110,B00000000,LETEND}, // u
  {B00111000,B00000110,B00111000,B00000000,LETEND}, // v
  {B00111100,B00000010,B00001100,B00000010,B00111100,B00000000,LETEND}, // w
  {B00110110,B00001000,B00110110,B00000000,LETEND}, // x
  {B00111001,B00000101,B00111110,B00000000,LETEND}, // y
  {B00100110,B00101010,B00110010,B00000000,LETEND}, // z
  {B11111010,B00000000,LETEND}, // !
  {B11000000,B00000000,B11000000,B00000000,LETEND}, // "
  {B00100100,B01111110,B00100100,B01111110,B00100100,B00000000,LETEND}, // #
  {B01110100,B01010100,B11111110,B01010100,B01011100,B00000000,LETEND}, // $
  {B01100010,B01100100,B00001000,B00010000,B00100110,B01000110,B00000000,LETEND}, // %
  {B00001100,B01010010,B10100010,B01010010,B00001100,B00010010,B00000000,LETEND}, // &
  {B10000000,B01000000,B00000000,LETEND}, // '
  {B01111110,B10000001,B00000000,LETEND}, // (
  {B10000001,B01111110,B00000000,LETEND}, // )
  {B00010000,B01010100,B00111000,B01010100,B00010000,B00000000,LETEND}, // *
  {B00010000,B00010000,B01111100,B00010000,B00010000,B00000000,LETEND}, // +
  {B00000001,B00000010,B00000000,LETEND}, // ,
  {B00010000,B00010000,B00010000,B00010000,B00000000,LETEND}, // -
  {B00000010,B00000000,LETEND}, // .
  {B00000010,B00000100,B00001000,B00010000,B00100000,B01000000,B00000000,LETEND}, // /
  {B00100010,B00000000,LETEND}, // :
  {B00000001,B00100010,B00000000,LETEND}, // ;
  {B00010000,B00101000,B01000100,B10000010,B00000000,LETEND}, // <
  {B00100010,B00100010,B00100010,B00000000,LETEND}, // =
  {B10000010,B01000010,B00101000,B00010000,B00000000,LETEND}, // >
  {B01000000,B10000000,B10001101,B10010000,B01100000,B00000000,LETEND}, // ?
  {B01111100,B10000010,B10111010,B10111010,B10001010,B01111010,B00000000,LETEND}, // @
  {B01111100,B10000010,B10000010,B01111100,B00000000,LETEND}, // 0
  {B00100010,B01000010,B11111110,B00000010,B00000010,B00000000,LETEND}, // 1
  {B01000010,B10000110,B10001010,B10010010,B10100010,B01000010,B00000000,LETEND}, // 2
  {B01000100,B10000010,B10010010,B01101100,B00000000,LETEND}, // 3
  {B11110000,B00010000,B00111110,B00000000,LETEND}, // 4
  {B11110010,B10010010,B10010010,B10010010,B10001100,B00000000,LETEND}, // 5
  {B01111100,B10010010,B10010010,B01001100,B00000000,LETEND}, // 6
  {B10000000,B10001110,B10110000,B11000000,B00000000,LETEND}, // 7
  {B01101100,B10010010,B10010010,B01101100,B00000000,LETEND}, // 8
  {B01100000,B10010010,B10010010,B01111100,B00000000,LETEND}, // 9
};*/

/**
  All the numbers in the world.
 */
/*
const uint8_t ProGamer::allNumbers[10][8] = {
  { B00000010,B00000101,B00000101,B00000101,B00000101,B00000101,B00000101,B00000010 },
  { B00000010,B00000110,B00000010,B00000010,B00000010,B00000010,B00000010,B00000111 },
  { B00000010,B00000101,B00000001,B00000010,B00000010,B00000100,B00000100,B00000111 },
  { B00000111,B00000001,B00000001,B00000110,B00000001,B00000001,B00000101,B00000010 },
  { B00000100,B00000101,B00000101,B00000111,B00000001,B00000001,B00000001,B00000001 },
  { B00000111,B00000100,B00000100,B00000110,B00000001,B00000001,B00000101,B00000010 },
  { B00000011,B00000100,B00000100,B00000110,B00000101,B00000101,B00000101,B00000010 },
  { B00000111,B00000001,B00000001,B00000010,B00000010,B00000100,B00000100,B00000100 },
  { B00000010,B00000101,B00000101,B00000010,B00000101,B00000101,B00000101,B00000010 },
  { B00000010,B00000101,B00000101,B00000011,B00000001,B00000001,B00000001,B00000110 }
};*/