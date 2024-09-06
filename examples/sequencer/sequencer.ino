#include <ProGamer.h>
#include <string.h>

ProGamer gamer;


int buttonxPos = 0;
int buttonyPos = 0;
int currNote;

long  prevDecay;
long  noteInterval = 50;

int upB;
int downB;
int leftB;
int rightB;
int startB;
int tempoB;

int noteOn;

byte counter[8];
byte noteGrid[8];
byte byCo[8] = {1, 2, 4, 8, 16, 32, 64, 128};
byte notes[8] = {ProGamer::Note::C2, ProGamer::Note::D2, ProGamer::Note::E2, ProGamer::Note::F2,
                ProGamer::Note::G2, ProGamer::Note::A3, ProGamer::Note::B3, ProGamer::Note::C3};
int tempoCounts[5] = {50, 100, 200, 300, 400};
int currTemp;
int noteStep[8];
long previousMillis = 0;
long interval = 100;

void setup() {
  gamer.begin();
  Serial.begin(9600);
  gamer.setFramelength(tempoCounts[0]);
  memset(noteStep, -1, 8 * sizeof(int));
}

void loop() {
  gamer.clear();

  if (gamer.isPressed(CAPTOUCH)) {
    currTemp++;
    if (currTemp > 4) {
      currTemp = 0;
    }
  }
  interval = tempoCounts[currTemp];

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (currNote > 6) {
      currNote = 0;
    } else {
      currNote++;
    }
  }


  if (gamer.isPressed(UP)) {buttonyPos--;}
  if (gamer.isPressed(DOWN)) {buttonyPos++;}
  if (gamer.isPressed(LEFT)) {buttonxPos--;}
  if (gamer.isPressed(RIGHT)) {buttonxPos++;}

  if (buttonyPos > 7) {
    buttonyPos = 0;
  }
  if (buttonyPos < 0) {
    buttonyPos = 7;
  }
  if (buttonxPos > 7) {
    buttonxPos = 0;
  }
  if (buttonxPos < 0) {
    buttonxPos = 7;
  }

  if (gamer.isPressed(START)) {
    if (noteOn != 1) {
      noteGrid[buttonyPos] = byCo[buttonxPos];
      noteStep[buttonyPos] = buttonxPos;
      noteOn = 1;
    } else {
      noteStep[buttonyPos] = -1;
      noteGrid[buttonyPos] = 0;
      noteOn = 0;
    }
  } else {
    startB = 0;
  }

  for (int j = 0; j < 8; j++) {
    gamer.setPixel(j, currNote, ProGamer::CLR4_DARK);
  }
  for(int i=0; i<8; i++) {
    int xPos = noteStep[i];
    if(xPos > -1)
      gamer.setPixel(xPos, i, ProGamer::CLR4_ONE);
  }
  gamer.setPixel(buttonxPos, buttonyPos, ProGamer::CLR4_LIGHT);
  //Serial.println(noteGrid[currNote]);
  if (noteGrid[currNote] == 0) {
    //gamer.stopTone();
  } else {
    gamer.playSFX(1, &notes[noteStep[currNote]], interval);
  }

  gamer.update();
}
