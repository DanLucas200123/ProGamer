/*
This example shows you how to scroll text
across the Gamer's screen. 
*/

// Include Gamer library
#include <ProGamer.h>

// Create a copy of the Gamer library
ProGamer gamer;

void setup() {
  // Start up Gamer.
  gamer.begin();
}

void loop() {
  // Print "Hello world"
  if(!gamer.isRenderingSpecial())
    gamer.printString("Hello world! :D");
  gamer.update();
}
