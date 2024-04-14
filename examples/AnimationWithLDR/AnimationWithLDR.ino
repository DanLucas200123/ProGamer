/*
In the original library this does something even on 1.9, but not with the new library.
Weird.
*/

// Load in the Gamer library
#include <ProGamer.h>

// Make an instance of the library
ProGamer gamer;

int currentFrame;

// Animation code from our Animation generator app! (R.I.P.)
// Replace this with yours!
#define NUMFRAMES 6
byte frames[NUMFRAMES][8] = {
		{B00000000,
		B00000000,
		B00000000,
		B00011000,
		B00011000,
		B00000000,
		B00000000,
		B00000000},

		{B00000000,
		B00000000,
		B00100100,
		B00011000,
		B00011000,
		B00100100,
		B00000000,
		B00000000},

		{B00000000,
		B01000010,
		B00100100,
		B00011000,
		B00011000,
		B00100100,
		B01000010,
		B00000000},

		{B00000000,
		B01011010,
		B00100100,
		B01011010,
		B01011010,
		B00100100,
		B01011010,
		B00000000},

		{B00011000,
		B01011010,
		B00100100,
		B11011011,
		B11011011,
		B00100100,
		B01011010,
		B00011000},

		{B10011001,
		B01011010,
		B00100100,
		B11011011,
		B11011011,
		B00100100,
		B01011010,
		B10011001}};


void setup() {
  // Set up Gamer
  gamer.begin();
  gamer.setFramelength(500);
}

void loop() {
  // Convert LDR value to a frame.
  currentFrame = map(gamer.ldrValue(), 0, 1023, 0, NUMFRAMES);
  
  // Print current frame!
  gamer.printImage(frames[currentFrame]);
  gamer.update();
}
