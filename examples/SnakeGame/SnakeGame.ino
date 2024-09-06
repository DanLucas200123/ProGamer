/*
SnakeGame example but it takes up less memory :-)
*/

#include <ProGamer.h>

ProGamer gamer;

int isPlaying;

// Tweaking variables
int snakeGrowthFactor = 1;
boolean passThroughWalls = true;
int snakeSpeed = 180;

// Changed these from ints to bytes, saving 24 bits of space per snake chunk :-D
byte snakeX[64];
byte snakeY[64];
int snakeDirection;

// Stores the snake's length. This is also the score!
int snakeLength;

byte appleX;
byte appleY;

byte appleColour = ProGamer::CLR4_LIGHT;
byte snakeColourA = ProGamer::CLR4_ONE;
byte snakeColourB = ProGamer::CLR4_DARK;
int gameOverStep = -1;

// Splash screen image
byte splashScreen[8] = {
  B00000000,
  B01111110,
  B01000000,
  B01111110,
  B00000010,
  B00000010,
  B01011110,
  B00000000
};

void setup() {
  gamer.begin();
  randomSeed(gamer.ldrValue());
  gamer.setFramelength(snakeSpeed);
}

void loop() {
  if (isPlaying) {
    gamer.clear();
    updateApple();
    updateDirection();
    moveSnake();
    detectCollision();
    drawSnake();
    drawApple();
  }
  else if(gameOverStep >= 0) {
    gameOverUpdate();
  }
  else {
    showSplashScreen();
  }

  gamer.update();
}

/* ---------------------------------------------------------------
  Displays the game's splash screen. Press start and the game will
  start.
*/
void showSplashScreen() {
  if (gamer.isPressed(START)) {
    // begin!
    snakeDirection = DOWN;
    snakeLength = 1;
    snakeX[0] = 0;
    snakeY[0] = 0;
    generateApple();
    isPlaying = true;
    gamer.setFramelength(snakeSpeed);
  }
  else {
    gamer.printImage(splashScreen);
  }
}

/* ---------------------------------------------------------------
  It's game over baby! Shows the score, and takes us back
  to the splash screen.
*/
void gameOver() {
  gameOverStep = 0;
  isPlaying = false;
}

void gameOverUpdate() {
  if(gamer.isRenderingSpecial())
    return;
  
  if(gameOverStep == 0) {
    gamer.clear();
    gamer.printString("Game over");
    gamer.setFramelength(50);
  }
  else if(gameOverStep == 1) {
    gamer.printString("Score");
  }
  else if(gameOverStep == 2) {
    gamer.showScore(snakeLength);
  }
  else if(gameOverStep == 3) {
    gamer.setFramelength(500);
  }
  else {
    gameOverStep = -1;
    return;
  }

  gameOverStep++;
}
