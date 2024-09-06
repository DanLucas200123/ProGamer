/*
Flappy Bird example using the ProGamer library
*/

#include <ProGamer.h>

ProGamer gamer;

// General
int score;
boolean isPlaying;
int gameOverTimer = -1;

// Wall - related
int wallThickness = 2;
int gapSize = 3;
int currentWallPosition;
int gapPosition;

// Bird - related
int flyingSpeed = 150;
int birdX = 2;
int birdY;
int gravity = 1;

// Sound - related
int gameStartSongLength = 3;
byte gameStartNotes[] = {160, 140, 120};
int gameOverSongLength = 4;
byte gameOverNotes[] = {120, 140, 160, 190};
byte wallNote[] = {200};

// Splash screen image
byte splashScreen[8] = {B11101110,
                        B10001001,
                        B10001001,
                        B11101111,
                        B10001001,
                        B10001001,
                        B10001110,
                        B00000000};


void setup() {
  gamer.begin();
  randomSeed(gamer.ldrValue());
  gamer.setFramelength(flyingSpeed);
}

void loop() {
  if(isPlaying && gameOverTimer < 0) {
    gamer.clear();
    moveWall();
    drawWall();
    updateBird();
    detectCollision();
    recordScore();
    drawBird();
  }
  else if(gameOverTimer >= 0) {
    updateGameOver();
  }
  else {
    showSplashScreen();
  }

  gamer.update();
}

/* ---------------------------------------------------------------
 Updates the bird's position. If you press UP, it will move up.
 Otherwise, gravity will bring it down :)
 */
void updateBird() {
  /* 
   If the UP button or capacitive touch is pressed, move the bird up. Otherwise, 
   move it down with gravity. Remember, the Y axis' 0 is on the top
   of the screen. Therefore, when we move the bird up, we REDUCE birdY. 
   When gravity brings it down, away from the axis' origin, 
   we INCREASE birdY.
   */
  if(gamer.isPressed(UP) || gamer.isPressed(CAPTOUCH)) {
    birdY--;
  }
  else {
    birdY = birdY + gravity;
  }

  // Detect if the bird is on the floor.
  if(birdY == 7) gameOver();
}

/* ---------------------------------------------------------------
 Draws the bird. Wherever it might be!
 */
void drawBird() {
  // Make sure the bird isn't off the screen.
  birdY = constrain(birdY, 0, 7);
  // Display the bird dot.
  if(gameOverTimer < 0)
    gamer.setPixel(birdX, birdY, ProGamer::CLR4_ONE);
}

/* ---------------------------------------------------------------
 Moves the walls from right to left on the screen, with a constant
 speed. 
 */
void moveWall() {
  // If the wall is at the end of the screen, get a new wall going!
  if(currentWallPosition == 0 - wallThickness) {
    generateWall();
  }
  // Otherwise, move the wall. 
  else {
    currentWallPosition--;
  }
}

/* ---------------------------------------------------------------
 Places a new wall on the edge of the screen, ready to be moved.
 */
void generateWall() {
  // Set the wall to the right of the screen.
  currentWallPosition = 8;
  // Get a random gap in the wall.
  gapPosition = random(1, 7-gapSize);
}

/* ---------------------------------------------------------------
 Draws the walls at their current position
 */
void drawWall() {
  // Draw multiple walls, if we need to.
  for(int j=0; j<wallThickness; j++) {
    if(currentWallPosition+j >= 0 && currentWallPosition+j <= 7) {
      for(int i=0; i<8; i++) {
        // Draw the wall, but miss out the gap. 
        if(i > gapPosition + gapSize - 1 || i < gapPosition) {
          gamer.setPixel(currentWallPosition+j, i, ProGamer::CLR4_ONE);
        }
      }
    }
  }
}

/* ---------------------------------------------------------------
 Checks if the bird is on a wall. If there's a pixel on the 
 same position as the bird, it's game over!
 */
void detectCollision() {
  if(gamer.getPixel(birdX, birdY)) {
    gameOver();
  }
}

/* ---------------------------------------------------------------
 Keeps track of the score. If the bird is flying through a wall, 
 add one to the score, and make a sound!
 */
void recordScore() {
  if(birdX == currentWallPosition + wallThickness) {
    score++;
    gamer.playSFX(1, wallNote, flyingSpeed);
  }
}

/* ---------------------------------------------------------------
 Shows an image when the game isn't being played.
 */
void showSplashScreen() {
  if(gamer.isPressed(START)) {
    
    // Play a tune before the game starts.
    gamer.playSFX(3, gameStartNotes, 100);
    
    isPlaying = true;
    generateWall();
    birdY = 2;
    score = 0;

    gamer.setFramelength(flyingSpeed);
  }
  else {
    gamer.printImage(splashScreen);
  }
}

/* ---------------------------------------------------------------
 Shows Game over, followed by the score. 
 */
void gameOver() {
  
  // Play the Game Over tune.
  gamer.playSFX(4, gameOverNotes, 100);
  gameOverTimer = 0;
  
  // Display Game Over followed by the score.
  gamer.printString("Game over");
  gamer.setFramelength(50);
}

void updateGameOver() {
  if(gamer.isRenderingSpecial())
    return;
  
  if(gameOverTimer == 0) {
    gamer.printString("Score");
    gamer.setFramelength(50);
  }
  else if(gameOverTimer == 1) {
    gamer.showScore(score);
    gamer.setFramelength(100);
  }
  else if(gameOverTimer > 20) {
    isPlaying = false;
    gameOverTimer = -1;
    return;
  }

  gameOverTimer++;
}



