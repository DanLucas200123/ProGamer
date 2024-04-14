/*
This example shows you how to do an animation on the
Gamer's screen. Remember that if you want to use our awesome
animation generator to automagically convert your
animations into code that can be read by the Gamer,
well then it sucks to be you because it's been taken
offline, have fun hand drawing animations!
*/

// Include Gamer library
#include <ProGamer.h>

// Create a copy of the Gamer library.
ProGamer gamer;

// Animation code from our Animation generator app! (R.I.P.)
// Replace this with yours!
#define NUMFRAMES 5
byte frames[NUMFRAMES][8] = {
		{B00000000,
		B00000000,
		B00100100,
		B00000000,
		B00000000,
		B00111100,
		B00000000,
		B00000000},

		{B00000000,
		B00000000,
		B00100100,
		B00000000,
		B00011000,
		B00111100,
		B00011000,
		B00000000},

		{B00000000,
		B00000000,
		B00100100,
		B00000000,
		B00000000,
		B00111100,
		B00000000,
		B00000000},

		{B00000000,
		B00000000,
		B00100100,
		B00000000,
		B00000000,
		B00111100,
		B00011000,
		B00000000},

		{B00000000,
		B00000000,
		B00100100,
		B00000000,
		B00000000,
		B00111100,
		B00011000,
		B00011000}};

// -- Animation code ends here.

void setup() {
  // Start up Gamer.
  gamer.begin();
  gamer.setFramelength(200);
}

int i = 0;
void loop() {
  if(i < NUMFRAMES) {
    gamer.printImage(frames[i]);
    i++;
  }

  gamer.update();
}
