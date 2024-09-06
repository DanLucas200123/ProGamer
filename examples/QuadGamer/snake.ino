// Tweaking variables
int snakeGrowthFactor = 1;
boolean passThroughWalls = true;
int snakeSpeed = 180;

// Changed these from ints to bytes, saving 24 bits of space per snake chunk
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
byte snakeSplashScreen[8] = {
  B00000000,
  B01111110,
  B01000000,
  B01111110,
  B00000010,
  B00000010,
  B01011110,
  B00000000
};

void setupSnakeGame() {
  isPlaying = false;
  gamer.setFramelength(snakeSpeed);
}

void snakeLoop() {
  if (isPlaying) {
    gamer.clear();
    updateApple();
    updateDirection();
    moveSnake();
    snakeDetectCollision();
    drawSnake();
    drawApple();
  }
  else if(gameOverStep >= 0) {
    gameOverUpdate();
  }
  else {
    snakeShowSplashScreen();
  }
}

/* ---------------------------------------------------------------
  Displays the game's splash screen. Press start and the game will
  start.
*/
void snakeShowSplashScreen() {
  if (gamer.isPressed(UP)) {
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
    gamer.printImage(snakeSplashScreen);
  }
}

/* ---------------------------------------------------------------
  It's game over baby! Shows the score, and takes us back
  to the splash screen.
*/
void snakeGameOver() {
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

/*
 * These are all apple-related functions.
 */

/* ---------------------------------------------------------------
  Checks if the apple has been eaten by the snake. If it has, 
  it makes the snake longer and generates a new apple. 
*/
void updateApple() {
  if(snakeX[0] == appleX && snakeY[0] == appleY) {
    generateApple();
    snakeLength = snakeLength + snakeGrowthFactor;
  }
}

/* ---------------------------------------------------------------
  Generates a new apple, at a random position on the screen.
*/
void generateApple() {
  appleX = random(0, 7);
  appleY = random(0, 7);
}

/* ---------------------------------------------------------------
  Draws the apple on the screen. 
*/
void drawApple() {
  gamer.setPixel(appleX, appleY, appleColour);
}

/* ---------------------------------------------------------------
 Checks to see if buttons are pressed, then changes the direction
 of the snake.
 */
void updateDirection() {
  if(gamer.isPressed(UP)) {
    if(snakeDirection != DOWN) snakeDirection = UP;
  }
  else if(gamer.isPressed(DOWN)) {
    if(snakeDirection != UP) snakeDirection = DOWN;
  }
  else if(gamer.isPressed(LEFT)) {
    if(snakeDirection != RIGHT) snakeDirection = LEFT;
  }
  else if(gamer.isPressed(RIGHT)) {
    if(snakeDirection != LEFT) snakeDirection = RIGHT;
  }
}

/* ---------------------------------------------------------------
 Moves the snake's head by one position forward, depending on the snake's
 direction. 
 */
void moveSnake() {
  int newX;
  int newY;
  
  // Change the head's position, depending
  // on the direction the snake is going towards.
  if(snakeDirection == LEFT) {
    newX = snakeX[0] - 1;
    newY = snakeY[0];
  }
  else if(snakeDirection == RIGHT) {
    newX = snakeX[0] + 1;
    newY = snakeY[0];
  }
  else if(snakeDirection == UP) {
    newX = snakeX[0];
    newY = snakeY[0] - 1;
  }
  else if(snakeDirection == DOWN) {
    newX = snakeX[0];
    newY = snakeY[0] + 1;
  }
  
  moveBody();
  
  /* Store the snake's new head position. 
  If it's off the screen, put it back into 
  the screen! */
  if(passThroughWalls == true) {
    if(newX == 8) snakeX[0] = 0;
    else if(newX == -1) snakeX[0] = 7;
    else snakeX[0] = newX;
    
    if(newY == 8) snakeY[0] = 0;
    else if(newY == -1) snakeY[0] = 7;
    else snakeY[0] = newY;
  }
  else if(passThroughWalls == false) {
    if(newX == 8 || newX == -1) snakeGameOver();
    else snakeX[0] = newX;

    if(newY == 8 || newY == -1) snakeGameOver();
    else snakeY[0] = newY;
  }
}

/* ---------------------------------------------------------------
 Moves the snake's body to follow the head.
 */
void moveBody() {
  // Move the rest of the snake forward!
  for(int i=snakeLength-1; i>0; i--) {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }
}

/* ---------------------------------------------------------------
 Checks whether the snake has crashed into itself.
 */
void snakeDetectCollision() {
  for(int i=1; i<snakeLength; i++) {
    if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) snakeGameOver();
  }
}

/* ---------------------------------------------------------------
 Draws the snake on the screen.
 */
void drawSnake() {
  for(int i=0; i<snakeLength; i++) {
    gamer.setPixel(snakeX[i], snakeY[i], i % 2 ? snakeColourB : snakeColourA);
  }
}