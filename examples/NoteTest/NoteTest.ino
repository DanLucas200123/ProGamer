#include <ProGamer.h>

ProGamer gamer;
byte note[1];
#define N ProGamer::Note::
#define TOTAL_NOTES 22
byte noteList[TOTAL_NOTES] = {
  N G1, N GSHARP1,
  N A2, N ASHARP2, N B2, N C2, N CSHARP2, N D2, N DSHARP2, N E2, N F2, N FSHARP2, N G2, N GSHARP2,
  N A3, N ASHARP3, N B3, N C3, N CSHARP3, N D3, N DSHARP3, N E3
};
int noteListIndex = 6;

void setup() {
  gamer.begin();
  gamer.setFramelength(50);
}

void loop() {
  int drawMode = 0;

  if(gamer.isPressed(RIGHT)) {
    noteListIndex++;
    drawMode = 1;
  }
  else if(gamer.isPressed(LEFT)) {
    noteListIndex--;
    drawMode = 1;
  }
  else if(gamer.isPressed(UP)) {
    note[0]++;
    drawMode = 2;
  }
  else if(gamer.isPressed(DOWN)) {
    note[0]--;
    drawMode = 2;
  }

  if(noteListIndex < 0)
    noteListIndex = TOTAL_NOTES-1;
  if(noteListIndex >= TOTAL_NOTES)
    noteListIndex = 0;
  
  if(drawMode == 1) {
    gamer.clear();
    note[0] = noteList[noteListIndex];

    char c;
    bool sharp = false;
    switch(note[0]) {
      case N ASHARP2:
        sharp = true;
      case N A2:
        c = 'a'; break;
      case N B2:
        c = 'b'; break;
      case N CSHARP2:
        sharp = true;
      case N C2:
        c = 'c'; break;
      case N DSHARP2:
        sharp = true;
      case N D2:
        c = 'd'; break;
      case N E2:
        c = 'e'; break;
      case N FSHARP2:
        sharp = true;
      case N F2:
        c = 'f'; break;
      case N GSHARP1:
        sharp = true;
      case N G1:
        c = 'g'; break;
      case N GSHARP2:
        sharp = true;
      case N G2:
        c = 'G'; break;
      case N ASHARP3:
        sharp = true;
      case N A3:
        c = 'A'; break;
      case N B3:
        c = 'B'; break;
      case N CSHARP3:
        sharp = true;
      case N C3:
        c = 'C'; break;
      case N DSHARP3:
        sharp = true;
      case N D3:
        c = 'D'; break;
      case N E3:
        c = 'E'; break;
    }

    gamer.printChar(c);
    if(sharp)
      gamer.setPixel(7, 0, ProGamer::ONE);
  }
  else if(drawMode == 2) {
    gamer.clear();
    gamer.showScore(note[0]);
  }

  gamer.playSFX(1, note, 200);

  gamer.update();
}
