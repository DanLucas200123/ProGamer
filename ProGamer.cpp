#include "ProGamer.h"

#define INPUT_MASK(_X_) (1 << (_X_))

#define TOP_DIGIT(_X_) (_X_ / (pow(10, floor(log10(_X_)))))
#define TOP_2_DIGITS(_X_) (_X_ / (pow(10, floor(log10(_X_ / 10)))))
#define REMOVE_FIRST_DIGIT(_X_) (_X_ % (int)(pow(10, floor(log10(_X_))) + 0.1))

bool toggleVal = false;
int split = 0;
bool ir = false;
bool irTog = false;
bool toneIsPlaying = false;
bool playTog = false;
bool toneStopped = false;
int splitMod = 10;

ProGamer *thisGamer = NULL;

// Interrupt service routine.
ISR(TIMER2_COMPB_vect)
{
  if(ir == 1) {
    thisGamer->isrRoutine();
  }

}

// Interrupt service routine for simultaneous IR & buzzer.
ISR(TIMER2_COMPA_vect)
{
  if(ir == 0) {
    if(toneIsPlaying) {
      if (toggleVal) {
        PORTD |= _BV(PORTD2);
        toggleVal = 0;
        if(split % splitMod == 0 ){
          thisGamer->isrRoutine();
        }
        //split++;
      }
      else {
        PORTD &= ~_BV(PORTD2);
        toggleVal = 1;
        if(split % splitMod == 0 ) {
          thisGamer->isrRoutine();
        }
        //split++;
      }
      //split++;
    }
    else {
      PORTD &= ~_BV(PORTD2);
      if(split % splitMod == 0 ) {
        thisGamer->isrRoutine();
      }
    }
    split++;
  }
}

ProGamer::ProGamer() {}

void ProGamer::playTone(int note)
{
  TIMSK2 &= (1<<OCIE2A);

  if(playTog == false){
    toneStopped = false;
    irTog = true;
    ir = 0;
    noInterrupts();

    TCCR2A = ~(_BV(COM2B1)) | ~(_BV(WGM21)) | ~(_BV(WGM20));
    TCCR2B = ~(_BV(WGM22)) | ~(_BV(CS22));
    TCCR2B = (TCCR2B & 0b0000000) | 0;
    TIMSK2 = ~(_BV(OCIE2B));

    OCR2A = 0;
    TIMSK2 = 0;
    OCR2B = 0;
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2  = 0;
    TCCR2A |= (1 << WGM21);
    OCR2A = 180;
    TCCR2B |= (1 << CS21);
    TIMSK2 |= (1 << OCIE2A);
    playTog = true;
  }
  toneIsPlaying = true;
  OCR2A = note;
  interrupts();
}

void ProGamer::stopTone()
{
  if(toneStopped == false){
    TIMSK2 &= (1<<OCIE1A);
    toneStopped = true;
    toneIsPlaying = false;
    playTog = false;
    OCR2A = 180;
    digitalWrite(2,LOW);
    split = 0;
    toggleVal = 0;
    irTog = false;
    irBegin();
  }

  OCR1A = 14;
  toneIsPlaying = false;
}

/**
  Stops the 38KHz wave for the IR transmitter.
 */
void ProGamer::irEnd()
{
  irTog = false;
}

/**
  Creates a 38KHz wave for the IR transmitter.
 */
void ProGamer::irBegin()
{
  TIMSK2 &= ~(1<<OCIE2A);
  TIMSK2 &= (1<<OCIE1B);

  if(irTog == false) {

    irTog = true;

    noInterrupts();
    TCCR2A |=  ~(_BV(WGM21));
    TCCR2B |= ~(_BV(CS21));
    TIMSK2 |=  ~(_BV(OCIE2A));
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2  = 0;
    TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(WGM22) | _BV(CS22);
    OCR2A = 51;
    OCR2B = 26;
    TCCR2B = (TCCR2B & 0b00111000) | 0x2;
    TIMSK2 = _BV(OCIE2B);
    interrupts();

    ir = 1;
  }
}

void ProGamer::begin()
{
  ::thisGamer = this;
	
  _refreshRate = DEFAULT_REFRESH_RATE;
  ldrThreshold = 300;

  // Setup outputs
  pinMode(3, OUTPUT);
  for(int i=6; i<=10; i++) pinMode(i, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(5,INPUT);

  // Setup inputs
  DDRC = B00000;
  PORTC = B11111;

  //Analog Read 16 divisor
  ADCSRA &= ~(1 << ADPS2);

  irBegin();
}

void ProGamer::update()
{
  long processTimer = 0;
  pressedInputs = 0;
  while(processTimer < frameLength) {
    updateAudio();

    capTouchFlag = capTouch(); //This incurs a 1-frame delay - sub this from the delay at the loop end

    tick++;
    processTimer += FLASH_LOOP_TIME;
    delay(FLASH_LOOP_TIME - 1);
  }

  if(renderFunction)
    (this->*renderFunction)();
}

bool ProGamer::isPressed(uint8_t input)
{
  return pressedInputs & INPUT_MASK(input);
}

bool ProGamer::isHeld(uint8_t input)
{
  return !(currentInputState & INPUT_MASK(input));
}

void ProGamer::setFramelength(long value)
{
  frameLength = value;
}

long ProGamer::getFramelength()
{
  return frameLength;
}

bool ProGamer::isRenderingSpecial()
{
  return renderFunction; // != nullptr;
}

int ProGamer::ldrValue()
{
    return analogRead(LDR);
}

void ProGamer::setldrThreshold(uint16_t threshold)
{
  ldrThreshold = threshold;
}

void ProGamer::setRefreshRate(uint16_t refreshRate)
{
  _refreshRate = refreshRate;
}

void ProGamer::allOn()
{
  for(int j=0; j<8; j++) {
    image[j] = 0xFFFF;
  }
  renderFunction = nullptr;
}

void ProGamer::clear()
{
  for(int j=0; j<8; j++) {
    image[j] = 0;
  }
  renderFunction = nullptr;
}

void ProGamer::setPixel(int x, int y, byte colour)
{
  int shift = 14 - 2 * x;
  uint16_t mask = 0xFFFF ^ (0b11 << shift);
  image[y] &= mask;
  image[y] |= (colour << shift);

  renderFunction = nullptr;
}

byte ProGamer::getPixel(int x, int y)
{
  int shift = 14 - 2 * x;
  return (image[y] >> shift) & 0b11;
}

void ProGamer::printImage(byte *img, int x, int y)
{
  for(int j=max(y,0); j<min(y+8,8); j++) {
    for(int i=max(x,0); i<min(x+8,8); i++) {
      setPixel(i, j, img[j-y] & (1 << (7-(i-x))) ? CLR4_ONE : CLR4_ZERO);
    }
  }
  renderFunction = nullptr;
}

void ProGamer::printString(char *string)
{
  currentString = string;
  renderVar = 0;
  renderVar2 = 0;
  renderFunction = &ProGamer::renderString;
}

void ProGamer::printChar(char chr)
{
  bool end = false;
  for(int i=0; i<8; i++) {
    byte column = 0;
    if(!end) {
      int letIdx = charToLetterIndex(chr);
      column = pgm_read_byte(&allLetters[letIdx][i]);
      if(column == LETEND) {
        end = true;
        column = 0;
      }
    }
    appendColumn(column);
  }
}

void ProGamer::showScore(int n)
{
  if(n < 100) {
    if(n > 9)
      printDigit(n / 10, 0);
    printDigit(n % 10, 4);
    renderFunction = nullptr;
  }
  else {
    int top = TOP_DIGIT(n);
    printDigit(top, 4);
    renderFunction = &ProGamer::renderScore;
    renderVar = n;
    renderVar2 = -1;
  }
}

void ProGamer::appendColumn(uint16_t col)
{
  for(int j=0; j<8; j++) {
    image[j] <<= 2;
    image[j] |= (col >> (14 - 2 * j)) & 0b11;
  }
}

void ProGamer::appendColumn(byte col)
{
  for(int j=0; j<8; j++) {
    image[j] <<= 2;
    if((col >> (7 - j)) & 0b1)
      image[j] += CLR4_ONE;
  }
}

void ProGamer::setLED(bool value)
{
  digitalWrite(LED, value);
}

void ProGamer::toggleLED()
{
  digitalWrite(LED, !digitalRead(LED));
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

void ProGamer::playSFX(int trackSize, byte track[], int beatLength)
{
  currentSFX = track;
  sfxBeatLength = beatLength;
  sfxEndIdx = trackSize;

  sfxTick = -1;
}

void ProGamer::setSoundOn(bool value)
{
  soundOn = value;
  if(!value)
    stopTone();
}

bool ProGamer::isSoundOn()
{
    return soundOn;
}

bool ProGamer::colourToBinaryDigit(byte colour)
{
  if(colour == CLR4_ONE)
    return 1;
  else if(colour == CLR4_ZERO)
    return 0;
  else if(colour == CLR4_LIGHT)
    return (colourCounter >> 4) < LIGHT_TICKS_ON;
  else //(colour == CLR4_DARK)
    return (colourCounter & 0x0f) < DARK_TICKS_ON;
}

bool ProGamer::capTouch()
{
	pinMode(19, OUTPUT);

  digitalWrite(19, LOW);

  delay(1);

  // Prevent the timer IRQ from disturbing our measurement

  noInterrupts();

  // Make the pin an input with the internal pull-up on

  pinMode(19, INPUT_PULLUP);



  // Now see how long the pin to get pulled up. This manual unrolling of the loop

  // decreases the number of hardware cycles between each read of the pin,

  // thus increasing sensitivity.

  uint8_t cycles = 17;

        if (digitalRead(19)) { cycles =  0;}

  else if (digitalRead(19)) { cycles =  1;}

  else if (digitalRead(19)) { cycles =  2;}

  else if (digitalRead(19)) { cycles =  3;}

  else if (digitalRead(19)) { cycles =  4;}

  else if (digitalRead(19)) { cycles =  5;}

  else if (digitalRead(19)) { cycles =  6;}

  else if (digitalRead(19)) { cycles =  7;}

  else if (digitalRead(19)) { cycles =  8;}

  else if (digitalRead(19)) { cycles =  9;}

  else if (digitalRead(19)) { cycles = 10;}

  else if (digitalRead(19)) { cycles = 11;}

  else if (digitalRead(19)) { cycles = 12;}

  else if (digitalRead(19)) { cycles = 13;}

  else if (digitalRead(19)) { cycles = 14;}

  else if (digitalRead(19)) { cycles = 15;}

  else if (digitalRead(19)) { cycles = 16;}

  // End of timing-critical section

  interrupts();

  // Discharge the pin again by setting it low and output

  //  It's important to leave the pins low if you want to 

  //  be able to touch more than 1 sensor at a time - if

  //  the sensor is left pulled high, when you touch

  //  two sensors, your body will transfer the charge between

  //  sensors.

  digitalWrite(19, LOW);

  pinMode(19, OUTPUT);


  if(cycles > 0){
    return true;
  } else {
      return false;
  }
}

void ProGamer::updateAudio()
{
  int toneToPlay = -1;
  if(currentTrack) {
    if(trackIdx < 0) {
      trackIdx = 0;
      noteTime = currentTrack[0].duration * beatLength;
      toneToPlay = currentTrack[0].value;
    }
    else {
      noteTime -= FLASH_LOOP_TIME;
      while(noteTime < 0) {
        trackIdx++;
        if(trackIdx == trackEndIdx) {
          currentTrack = NULL;
          toneToPlay = 0;
          goto updateAudioSFX;
        }
        noteTime += currentTrack[trackIdx].duration * beatLength;
      }
      toneToPlay = currentTrack[trackIdx].value;
    }
  }
  
updateAudioSFX:
  if(currentSFX) {
    sfxTick += FLASH_LOOP_TIME;
    int sfxIdx = sfxTick / sfxBeatLength;
    if(sfxIdx >= sfxEndIdx) {
      currentSFX = NULL;
      if(toneToPlay < 0)
        toneToPlay = 0;
    }
    else {
      toneToPlay = currentSFX[sfxIdx];
    }
  }
  
  if(toneToPlay >= 0 && isSoundOn()) {
    if(toneToPlay >= 50)
      playTone(toneToPlay);
    else
      stopTone();
  }
}

void ProGamer::printDigit(int digit, int x)
{
  byte result[8];
  for(int p=0;p<8;p++) {
    result[p] = pgm_read_byte(&allNumbers[digit][p]);
  }
  printImage(result, x, 0);
}

int ProGamer::charToLetterIndex(char c) {
  if( c>='A' && c<='Z' ) return c-'A'+1;
  else if( c>='a' && c<='z' ) return c-'a'+1+26;
  else if( c>='!' && c<='/' ) return c+20;
  else if( c>=':' && c<='@' ) return c+10;
  else if( c>='0' && c<='9' ) return c+27;

  return 0;
}

void ProGamer::renderScore()
{
  renderVar2++;
  appendColumn((uint16_t)0);

  int top2Digits = TOP_2_DIGITS(renderVar);
  printDigit(top2Digits % 10, 7 - renderVar2);
  //renderMode = RM_SCORE;
  renderFunction = &ProGamer::renderScore;

  if(renderVar2 == 3) {
    renderVar2 = -1;
    renderVar = REMOVE_FIRST_DIGIT(renderVar);
    if(renderVar < 10) {
      renderFunction = nullptr;
    }
  }
}

void ProGamer::renderString()
{
  byte col = 0;
  char c = currentString[renderVar];
  if(c != '\0') {
    int letIx = charToLetterIndex(c);
    
    col = pgm_read_byte(&allLetters[letIx][renderVar2]);

    renderVar2++;
    if(pgm_read_byte(&allLetters[letIx][renderVar2]) == LETEND) {
      renderVar2 = 0;
      renderVar++;
    }
  }
  else {
    renderVar2++;
    if(renderVar2 > 7) {
      renderFunction = nullptr;
    }
  }

  appendColumn(col);
}

/**
  Runs routines within the Interrupt Service Routine.
  Display updating, button event tracking, LDR updating.
 */
void ProGamer::isrRoutine()
{
  if(ir == 1){
    pulseCount++;
    if(pulseCount >= _refreshRate) {
      updateRow();
      pulseCount = 0;
    }
    if(pulseCount == _refreshRate/2) {
      checkInputs();
    }
  }

  if (ir == 0){
    updateRow();
    checkInputs();
  }
}

/**
  Writes a byte to the TLC5916 LED driver (cathodes).
 */
void ProGamer::writeToDriver()
{
  // Output enable HIGH
  PORTB |= _BV(PORTB2);

  // Send byte to driver
  for(int x=0; x<8; x++) {
    PORTD &= ~_BV(PORTD6);

    byte pixel = getPixel(7 - x, counter);
    if(colourToBinaryDigit(pixel))
      PORTB |= _BV(PORTB0);
    else
      PORTB &= ~_BV(PORTB0);
    
    PORTD |= _BV(PORTD6);
  }

  PORTD &= ~_BV(PORTD6);
  PORTB |= _BV(PORTB1);
  PORTB &= ~_BV(PORTB1);
  PORTB &= ~_BV(PORTB2);
}

/**
  Writes a byte to the MIC5891 shift register (anodes).
  @param dataOut the byte to write to the register
 */
void ProGamer::writeToRegister(byte dataOut)
{
  for(int y=0; y<8; y++) {
    if((dataOut & (1<<y)) >> y) PORTB |= _BV(PORTB0);
    else PORTB &= ~_BV(PORTB0);
    PORTD |= _BV(PORTD7);
    PORTD &= ~_BV(PORTD7);
  }

  PORTB |= _BV(PORTB1);
  PORTB &= ~_BV(PORTB1);
}

void ProGamer::checkInputs()
{
  lastInputState = currentInputState;
  currentInputState = 0;

  for(int i=0; i<7; i++) {
    if(i != LDR) {
      byte inputBit = i == CAPTOUCH ? (!capTouchFlag << i) : (PINC & (1<<i));
      currentInputState |= inputBit;
      if(!inputBit && (lastInputState & INPUT_MASK(i))) {
        pressedInputs |= 1 << i;
      }
    }
    else {
      int ldrValue = analogRead(LDR);
      if(ldrValue - lastLDRValue >= ldrThreshold)
        pressedInputs |= 1 << LDR;
      lastLDRValue = ldrValue;
      if(ldrValue > ldrThreshold)
        currentInputState |= 1 << LDR;
    }
  }
}

void ProGamer::updateRow()
{
  if(counter==8) {
    counter = 0;
    currentRow = 0x80;
    colourCounter += 0x11; //Upper++, lower++
    if((colourCounter >> 4) == LIGHT_TICKS_OFF + LIGHT_TICKS_ON)
      colourCounter &= 0x0f;
    if((colourCounter & 0x0f) == DARK_TICKS_OFF + DARK_TICKS_ON)
      colourCounter &= 0xf0;
  }
  writeToRegister(0);
  writeToDriver();
  writeToRegister(currentRow);
  currentRow >>= 1;
  counter++;
}

/**
  All the letters in the world.
 */

const PROGMEM uint8_t ProGamer::allLetters[85][8] = {
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
};

/**
  All the numbers in the world.
  (Change from original: all bytes have been left-shifted by 4)
 */

const PROGMEM uint8_t ProGamer::allNumbers[10][8] = {
  { B00100000,B01010000,B01010000,B01010000,B01010000,B01010000,B01010000,B00100000 },
  { B00100000,B01100000,B00100000,B00100000,B00100000,B00100000,B00100000,B01110000 },
  { B00100000,B01010000,B00010000,B00100000,B00100000,B01000000,B01000000,B01110000 },
  { B01110000,B00010000,B00010000,B01100000,B00010000,B00010000,B01010000,B00100000 },
  { B01000000,B01010000,B01010000,B01110000,B00010000,B00010000,B00010000,B00010000 },
  { B01110000,B01000000,B01000000,B01100000,B00010000,B00010000,B01010000,B00100000 },
  { B00110000,B01000000,B01000000,B01100000,B01010000,B01010000,B01010000,B00100000 },
  { B01110000,B00010000,B00010000,B00100000,B00100000,B01000000,B01000000,B01000000 },
  { B00100000,B01010000,B01010000,B00100000,B01010000,B01010000,B01010000,B00100000 },
  { B00100000,B01010000,B01010000,B00110000,B00010000,B00010000,B00010000,B01100000 }
};