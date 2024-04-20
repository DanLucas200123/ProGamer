byte seqMax=0;
#define SEQ_CAP 30
int delayMils = 300; //larger = easier
#define DELAY_MILS_CAP 10
byte framesSimon[4][8];
byte go[8];
byte right[8];
byte wrong[8];
volatile byte sequence[SEQ_CAP];
int simonState = 0;
int seqIdx = 0;

enum SimonState {
  SS_COUNT_3 = 0,
  SS_COUNT_2 = 1,
  SS_COUNT_1 = 2,
  SS_COUNT_GO = 3,
  SS_MEM_SHOW,
  SS_MEM_HIDE,
  SS_CPY,
  SS_LOSE_START,
  SS_LOSE_SCORE,
  SS_LOSE_RESET,
  SS_WIN,
};

void resetSimon() {
  gamer.clear();
  sequence[0]=random(0,4);
  seqMax=1;
  delayMils = 300;
  simonState = SS_COUNT_3;
}

void simonLoop() {
  //I could just use one switch statement here, but the program won't work for some reason
  if(simonState < SS_LOSE_START) {
    switch(simonState) {
      case SS_COUNT_3:
        gamer.setFramelength(delayMils);
        sequence[seqMax % SEQ_CAP]=random(0,4);
      case SS_COUNT_2:
      case SS_COUNT_1:
        gamer.showScore(3-simonState);
        simonState++;
        break;
      case SS_COUNT_GO:
        gamer.printImage(go);
        seqIdx = 0;
        simonState = SS_MEM_SHOW;
        break;
      case SS_MEM_SHOW:
        gamer.printImage(framesSimon[sequence[seqIdx]]);
        simonState = SS_MEM_HIDE;
        break;
      case SS_MEM_HIDE:
        gamer.clear();
        seqIdx++;
        if(seqIdx > seqMax % SEQ_CAP) {
          simonState = SS_CPY;
          seqIdx = 0;
        }
        else {
          simonState = SS_MEM_SHOW;
        }
        break;
      case SS_CPY:
        byte key = 4;
        if(gamer.isPressed(UP)) key=0;
        if(gamer.isPressed(DOWN)) key=1;
        if(gamer.isPressed(LEFT)) key=2;
        if(gamer.isPressed(RIGHT)) key=3;

        if(key != 4) {
          gamer.printImage(framesSimon[key]);
          if(key == sequence[seqIdx]) {
            seqIdx++;
            if(seqIdx > seqMax % SEQ_CAP) {
              simonState = SS_WIN;
            }
          }
          else {
            simonState = SS_LOSE_START;
          }
        }
        break;
    }
  }
  else {
    switch(simonState) {
      case SS_LOSE_START:
        gamer.printImage(wrong);
        gamer.setFramelength(500);
        simonState = SS_LOSE_SCORE;
        break;
      case SS_LOSE_SCORE:
        gamer.showScore(seqMax-1);
        simonState = SS_LOSE_RESET;
        break;
      case SS_LOSE_RESET:
        resetSimon();
        break;
      case SS_WIN:
        seqMax++;
        gamer.printImage(right);
        gamer.setFramelength(500);
        simonState = SS_COUNT_3;
        delayMils -= delayMils / 40;
        delayMils = constrain(delayMils, DELAY_MILS_CAP, 99999);
        break;
      default:
        gamer.clear();
        simonState = SS_COUNT_3;
        break;
    }
  }

  // sequence[seqMax]=random(0,4);
  // if(seqMax>0) {
  //   for(byte p=3;p>0;p--) {
  //     showScore(0,p);
  //     delay(delayMils);
  //   }
  //   gamer.printImage(go); //move this later...
  //   delay(delayMils);
  //   for(int i=0;i<seqMax;i++) {
  //     if(gamer.isHeld(START)) return;
  //     gamer.printImage(framesSimon[sequence[i]]);
  //     delay(delayMils);
  //     gamer.clear();
  //     delay(delayMils);
  //   }
  //   gamer.clear();
  //   boolean success = true;
  //   for(byte count=0;count<seqMax;count++) {
  //     if(gamer.isHeld(START)) return;
  //     byte key = 4;
  //     while(key==4) { //wait for a keypress
  //       if(gamer.isHeld(START)) return;
  //       if(gamer.isPressed(UP)) key=0;
  //       if(gamer.isPressed(DOWN)) key=1;
  //       if(gamer.isPressed(LEFT)) key=2;
  //       if(gamer.isPressed(RIGHT)) key=3;
  //     }
  //     gamer.printImage(framesSimon[key]);
  //     //is it riiggghhhttt???
  //     if(key!=sequence[count]) {
  //       success = false; // game over...
  //       break;
  //     }
  //   }
  //   delayMils-=(delayMils/40);
  //   delay(delayMils);
  //   if(success) {
  //     seqMax++; //they got it right, MAKE IT HARDER!
  //     gamer.printImage(right);
  //   } 
  //   else {
  //     gamer.printImage(wrong);
  //     delay(500);
  //     showScore((seqMax-1)/10,(x-1)%10); //showScore wants digits, not numbers! (as in 1,5 rather than 15)
  //     delay(500);
  //     resetSimon();
  //   }
  // } else {
  //   x++; //rack up the difficulty anyway!
  // }
  // delay(500);
}

void setupSimonImages() {
  framesSimon[0][0] = B00000000; //up
  framesSimon[0][1] = B00011000;
  framesSimon[0][2] = B00111100;
  framesSimon[0][3] = B01111110;
  framesSimon[0][4] = B00011000;
  framesSimon[0][5] = B00011000;
  framesSimon[0][6] = B00011000;
  framesSimon[0][7] = B00000000;
  framesSimon[1][7] = B00000000; //down (aka up, but flipped)
  framesSimon[1][6] = B00011000;
  framesSimon[1][5] = B00111100;
  framesSimon[1][4] = B01111110;
  framesSimon[1][3] = B00011000;
  framesSimon[1][2] = B00011000;
  framesSimon[1][1] = B00011000;
  framesSimon[1][0] = B00000000;
  framesSimon[2][0] = B00000000; //left
  framesSimon[2][1] = B00010000;
  framesSimon[2][2] = B00110000;
  framesSimon[2][3] = B01111110;
  framesSimon[2][4] = B01111110;
  framesSimon[2][5] = B00110000;
  framesSimon[2][6] = B00010000;
  framesSimon[2][7] = B00000000;
  framesSimon[3][0] = B00000000; //right (left but flipped)
  framesSimon[3][1] = B00001000;
  framesSimon[3][2] = B00001100;
  framesSimon[3][3] = B01111110;
  framesSimon[3][4] = B01111110;
  framesSimon[3][5] = B00001100;
  framesSimon[3][6] = B00001000;
  framesSimon[3][7] = B00000000;
  go[0] = B00000000;
  go[1] = B01101110;
  go[2] = B10001010;
  go[3] = B10001010;
  go[4] = B10001010;
  go[5] = B10101010;
  go[6] = B01101110;
  go[7] = B00100000;
  wrong[0] = B11000011;
  wrong[1] = B01100110;
  wrong[2] = B00111100;
  wrong[3] = B00011000;
  wrong[4] = B00011000;
  wrong[5] = B00111100;
  wrong[6] = B01100110;
  wrong[7] = B11000011;
  right[0] = B00000001;
  right[1] = B00000011;
  right[2] = B00000111;
  right[3] = B00001110;
  right[4] = B11011100;
  right[5] = B11111000;
  right[6] = B01110000;
  right[7] = B00100000;
}



