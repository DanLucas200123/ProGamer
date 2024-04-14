/*
A cool jumping alien. We have 3 images,
which we will display on the Gamer screen
one after another to make an animation. 
*/

// Include Gamer library
#include <ProGamer.h>

// Create a copy of the Gamer library.
ProGamer gamer;

// Here are three images we will animate.
byte alien1[] = {
  B00000000,
  B00000000,
  B01111110,
  B01011010,
  B01111110,
  B00100100,
  B00100100,
  B01100110
};

byte alien2[] = {
  B00000000,
  B01111110,
  B01011010,
  B01111110,
  B00100100,
  B01000010,
  B11000011,
  B00000000
};

byte alien3[] = {
  B01111110,
  B01011010,
  B01111110,
  B10100101,
  B11000011,
  B00000000,
  B00000000,
  B00000000
};

void setup() {
  // Start up Gamer.
  gamer.begin();
  gamer.setFramelength(500);
}

int step;
void loop() {
  // Show images, with a small delay in between.
  switch(step) {
    case 0: case 5:
      gamer.printImage(alien1);
      break;
    case 1: case 4:
      gamer.printImage(alien2);
      break;
    case 2: case 3:
      gamer.printImage(alien3);
      break;
  }

  step++;
  if(step > 5)
    step = 0;
  gamer.update();
}

