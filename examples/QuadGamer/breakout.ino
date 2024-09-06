const int BREAKOUT_GAME_FRAMELENGTH = 200;
const int PADDLE_SIZE = 2;
 
//TODO!:
// GET IMAGES AND SCORE TO DISPLAY CORRECTLY!
// SLIGHT PHYSICS TWEAKS!
 
int currentX = 5;
int currentY = 4;
int velocity[2] = {
  -1,-1};
int blocks[8][8];
int paddleX = 2;
int counter = 0;
int origXV=-1;
int origYV=-1;

byte frames[2][8]; //declare at top of code
void setupImages() { //run this at the start
  frames[0][0] = B00000000;
  frames[0][1] = B01100110;
  frames[0][2] = B01100110;
  frames[0][3] = B00000000;
  frames[0][4] = B01000010;
  frames[0][5] = B00111100;
  frames[0][6] = B00000000;
  frames[0][7] = B00000000;
  frames[1][0] = B00000000;
  frames[1][1] = B01100110;
  frames[1][2] = B01100110;
  frames[1][3] = B00000000;
  frames[1][4] = B00000000;
  frames[1][5] = B00111100;
  frames[1][6] = B01000010;
  frames[1][7] = B00000000;
}

void startBreakout() {
  gamer.setFramelength(BREAKOUT_GAME_FRAMELENGTH);
  breakoutStartGame(true);
}
 
void breakoutStartGame(boolean resetIt) {
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
      blocks[x][y] = 1;
    }
  }
  if(!resetIt) {
    score++;
    gamer.clear();
    gamer.printImage(frames[0]);
  } 
  else
    score=0;
  currentX=5;
  currentY=5;
  velocity[0]=-1;
  velocity[1]=-1;
}
 
void breakoutLoop() {
  gamer.clear();

  movePaddle();
  drawPaddle();

  origXV = velocity[0];
  origYV = velocity[1];
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
      if(blocks[x][y] == 1) {
        gamer.setPixel(x, y, ProGamer::CLR4_ONE);
      }
    }
  }
  physics();
  blocksplosion();
  int newX = currentX + velocity[0];
  int newY = currentY + velocity[1];
  /*if(newX>-1 && newX<8) {
    if(newY>-1 && newY<8) {
    } 
    else {
      if(gamer.getPixel(newX, currentY-velocity[1]) == ProGamer::ZERO) {
        blocks[newX][currentY+velocity[1]]=0;
        velocity[1]*=-1;
      } 
      else {
        blocks[currentX+velocity[0]][currentY+velocity[1]]=0;
        velocity[1]*=-1;
        velocity[0]*=-1;
      }
    }
  } 
  else {
    if(gamer.getPixel(currentX-velocity[0], newY) == ProGamer::ZERO) {
      blocks[currentX+velocity[0]][newY]=0;
      velocity[0]*=-1;
      if(newY<0 || newY>7) {
        if(gamer.getPixel(currentX+velocity[0], currentY-velocity[1]) == ProGamer::ZERO) {
          blocks[currentX-velocity[0]][currentY-velocity[1]]=0;
          velocity[1]*=-1;
        } 
        else panic();
      }
    } 
    else {
      for(int x=-1;x<2;x++) {
        for(int y=-1;y<2;y++) {
          blocks[currentX+x][currentY+y]=0;
        }
      }
      velocity[0]*=-1;
      velocity[1]*=-1;
    }
  }*/
  currentX = currentX+velocity[0];
  currentY = currentY+velocity[1];
  gamer.setPixel(currentX, currentY, ProGamer::CLR4_ONE);

  if(currentY==7) { //if out of play, lose
    for(int b=0;b<4;b++) {
      gamer.clear();
      //delay(150);
      gamer.setPixel(currentX, currentY, ProGamer::CLR4_ONE);
      //delay(150);
    }
    if(score==0){
      gamer.clear();
      gamer.printImage(frames[1]);
    }
    else {
      gamer.showScore(score);
    }
    //delay(500);
    breakoutStartGame(true);
  }
  boolean finished = true;
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
      if(blocks[x][y]==HIGH) finished=false;
    }
  }
  if(finished) {
    breakoutStartGame(false);
  }
}
 
boolean outOfBounds(int xV, int yV) {
  if(xV > 7 || xV < 0) {
    return true;
  } 
  else if(yV > 7 || yV < 0) {
    return true;
  } 
  else {
    return false;
  }
}

void movePaddle() {
  if(gamer.isHeld(LEFT)&&paddleX>0){
    paddleX--;
  } 
  else if(gamer.isHeld(RIGHT)&&paddleX<8 - PADDLE_SIZE) {
    paddleX++;
  }
}

void drawPaddle() {
  for(int x=0; x<8; x++) {
    gamer.setPixel(x, 7, x >= paddleX && x < paddleX+PADDLE_SIZE ? ProGamer::CLR4_ONE : ProGamer::CLR4_ZERO);
  }
}
 
void physics() {
  //If collided with something, reflect off the collided thing, destroying it if it is a block
  bool recheck;
  while(true) {
    int nextX = currentX+velocity[0];
    int nextY = currentY+velocity[1];
    if(outOfBounds(nextX, nextY)) {
      if(nextX == -1 || nextX == 8)
        velocity[0] *= -1;
      else
        velocity[1] *= -1;
      continue;
    }

    if(nextY == 7) {
      // if(currentX == paddleX + 1) {
      //   if(velocity[0] == 0)
      //     velocity[0] = random(2) ? 1 : -1;
      // }
      if(/*nextX == paddleX || */currentX == paddleX) {
        velocity[0] = constrain(velocity[0]-1, -1, 1);
      }
      else if(/*nextX == paddleX + PADDLE_SIZE-1 ||*/ currentX == paddleX + PADDLE_SIZE-1) {
        velocity[0] = constrain(velocity[0]+1, -1, 1);
      }
      else if(nextX == paddleX || nextX == paddleX + PADDLE_SIZE-1) {
        velocity[0] *= -1;
      }
      else {
        break;
      }

      velocity[1] *= -1;
      continue;
    }

    if(gamer.getPixel(nextX, currentY) != ProGamer::CLR4_ZERO) {
      velocity[0] *= -1;
      breakBlock(nextX, currentY);
      continue;
    }
    if(gamer.getPixel(currentX, nextY) != ProGamer::CLR4_ZERO) {
      velocity[1] *= -1;
      breakBlock(currentX, nextY);
      continue;
    }

    if(gamer.getPixel(nextX, nextY) != ProGamer::CLR4_ZERO) {
      velocity[0] *= -1;
      velocity[1] *= -1;
      breakBlock(nextX, nextY);
      continue;
    }

    break;
  }
}

void breakBlock(int x, int y) {
  if(blocks[x][y]) {
    blocks[x][y] = 0;
    gamer.setPixel(x, y, ProGamer::CLR4_ZERO);
  }
}

void blocksplosion() {
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      if(blocks[x][y]==0) {
        if(x%2==0) {
          if(y%2==0) {
            breakBlock(x+1, y);
          } 
          else
            breakBlock(x-1, y);
        } 
        else {
          if(y%2==0) {
            breakBlock(x-1, y);
          } 
          else
            breakBlock(x+1, y);
        }
      }  
    }
  }
}