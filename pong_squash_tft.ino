/********
 * Arduino Pong
 * By Pete Lamonica
 *  modified by duboisvb
 *  updated by James Bruce 
 *  modified for tft, add menu and squash for single player, by Hans-Peter Dietrich
 * A simple implementation of Pong on the Arduino using a tft for output.
 * 
 * I used Arduino IDE 1.6.12 from Arduino.CC
 * 
 * Testet with 2.4" TFT 320x240 LCD Shield (8-bit parallel, ST7783) and SWTFT library 
 * from http://www.smokeandwires.co.nz on Arduino UNO. Uno modified with additional soldered A6 and A7 lines, 
 * because all other pins are connected to TFT. Should work with any other display which can work with 
 * the Adafruit GFX Master library.
 * 2 Paddel (10K) connected to 5V, GND and A6 (A7)
 * 1 Button connected to 5V and A5 (A5 pulldown with 10K to GND)
 * 1 optional Buzzer connected to GND and D11
 * 
 * 
 * In Menu select with Paddle 1 and Button. To start place paddle on "Start".
 * In Game press Button for exit
 * 
 */
 // Last modified 12. Nov. 2016 12.00h
 
#include <SWTFT.h>         // library for 2.4" TFT from www.smokkeandwires.co.nz
#include <Adafruit_GFX.h>  // Adafruit GFX Master library

#define SOUNDPORT 11
#define WHEEL_ONE_PIN A6 //analog for player 1 paddle
#define WHEEL_TWO_PIN A7 //analog for player 2 paddle
#define BUTTON A5        //for button
#define PADDLE_WIDTH 4
 
#define IN_MENU 1  //in menu state 
#define IN_GAMEA 2 //in game state - draw constants of the game box
#define IN_GAMEB 3 //in game state - draw the dynamic part of the game
 
#define LEFT_SCORE_X (319/2-40)  //x-position of score numbers
#define RIGHT_SCORE_X (319/2+40)
#define SCORE_Y 14                      //y-position of score numbers
 
#define LEFT 0
#define RIGHT 1
 
#define BLACK   0x0000
#define BLUE    0x001F
#define DARKBLU 0x000F
#define RED     0xF800
#define DARKRED 0xF000
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define TS_MINX 170
#define TS_MINY 120
#define TS_MAXX 865
#define TS_MAXY 930


SWTFT tft;


unsigned char x,y;

boolean button1Status = false;
int RIGHT_PADDLE_X = 300;  // x position of paddles
int LEFT_PADDLE_X = 20;
int PADDLE_HEIGHT = 40;     //Paddle size

int wheelOnePosition = 0;
int wheelTwoPosition = 0;
int rightPaddleY = 120;      // dynamic position of Paddles
int leftPaddleY = 120;
int rightPaddleYold = 120;   // for store ole Paddles positions
int leftPaddleYold = 120;
int ballX = 11;     // ball position
int ballY = 11;
int ballXold = 11;  // for store old ball position
int ballYold = 11;

int ballVolX = -3;         // Ball speed horizontal
int ballVolY = 2;          // Ball speed vertical
int MAX_Y_VELOCITY = 2;    //max y speed. Hard Game

int leftPlayerScore = 0;
int rightPlayerScore = 0;
 
int frame = 0;
int state = IN_MENU;
int game = 1;     // 1 for Squash, -1 for Tennis
int easy = 1;     // 1 for easy, -1 for difficult
int sound = 1;     // 1 for sound, -1 for no sound
int fast = 1;      // 1 for slow, -1 for fast
int gameover = 0; 
int PLAY_TO = 9;   // for game over counter

boolean taste1 = 0;





void setup()  {
  Serial.begin(57600);
  x=0;
  y=0;
   tft.reset();
   uint16_t identifier = tft.readID();
   Serial.print(F("LCD driver chip: "));
   Serial.println(identifier, HEX);
   Serial.end();
   
   tft.begin(identifier);
   tft.setRotation(3); // landscape
   tft.setTextSize(2);
   
  ballX = 319 / 2; 
  ballY = 239 / 2; 
}
 

void loop() {
  if (state == IN_MENU) drawMenu(); 
  if (game == -1) tennis();  
  if (game ==  1) squash();
}




 
void processInputs() {
  wheelOnePosition = analogRead(WHEEL_ONE_PIN);
  wheelTwoPosition = analogRead(WHEEL_TWO_PIN);
  button1Status = digitalRead(BUTTON); // 1 = Button pressed
  if (button1Status == 1) gameover = 1;
}


void menuPaddle(){
    rightPaddleY = ((wheelOnePosition /8) * (239-PADDLE_HEIGHT))/ 128;
    x = RIGHT_PADDLE_X;
    if (rightPaddleY < 27) rightPaddleY = 27;  // paddle should stay under "ARDUINO" 
    tft.fillRect(RIGHT_PADDLE_X,rightPaddleYold,PADDLE_WIDTH,PADDLE_HEIGHT,BLACK);
    tft.fillRect(RIGHT_PADDLE_X,rightPaddleY,PADDLE_WIDTH,PADDLE_HEIGHT,GREEN);
    rightPaddleYold = rightPaddleY;
}


void drawRightPaddle(){
    //rightPaddleY = ((wheelOnePosition /8) * (239-PADDLE_HEIGHT))/ 128; // normal Full Range
    rightPaddleY = ((wheelOnePosition /7) * (239-PADDLE_HEIGHT))/ 128;
    if (rightPaddleY > 220) rightPaddleY = 220; // Paddle should stay on board
    //Serial.println (rightPaddleY);
    x = RIGHT_PADDLE_X;
    tft.fillRect(RIGHT_PADDLE_X,rightPaddleYold,PADDLE_WIDTH,PADDLE_HEIGHT,DARKRED);
    tft.fillRect(RIGHT_PADDLE_X,rightPaddleY,PADDLE_WIDTH,PADDLE_HEIGHT,YELLOW);
    rightPaddleYold = rightPaddleY;
}


void drawLeftPaddle(){
    //leftPaddleY = ((wheelTwoPosition /8) * (239-PADDLE_HEIGHT))/ 128; // normal Full Range
    leftPaddleY = ((wheelTwoPosition /4) * (239-PADDLE_HEIGHT))/ 128; // normal Full Range
    if (leftPaddleY > 200) leftPaddleY = 200; // Paddle should stay on board
    x = LEFT_PADDLE_X;
    tft.fillRect(LEFT_PADDLE_X,leftPaddleYold,PADDLE_WIDTH,PADDLE_HEIGHT,DARKRED);
    tft.fillRect(LEFT_PADDLE_X,leftPaddleY,PADDLE_WIDTH,PADDLE_HEIGHT,CYAN);
    leftPaddleYold = leftPaddleY;
}


 
void drawTennisScreen() {
  drawRightPaddle();
  drawLeftPaddle();

  //draw score
  tft.setTextColor(WHITE, DARKRED);
  tft.setTextSize (3);
  tft.setCursor(LEFT_SCORE_X, SCORE_Y);
  tft.print(leftPlayerScore);
  tft.setCursor(RIGHT_SCORE_X, SCORE_Y);
  tft.print(rightPlayerScore);
  
  //draw ball
  tft.fillRect(ballXold, ballYold, 6, 6, DARKRED); // draw old position in background color
  tft.fillRect(ballX, ballY, 6, 6, WHITE);  // draw ball
  ballXold = ballX; ballYold = ballY;
}
 
 
 
void drawSquashScreen() {
  drawRightPaddle ();

  //draw score
  tft.setTextColor(WHITE, DARKRED);
  tft.setTextSize (3);
  tft.setCursor(120,SCORE_Y);
  tft.print(rightPlayerScore);
  
  //draw ball
  tft.fillRect(ballXold, ballYold, 7, 7, DARKRED);  //draw old position in background color
  tft.fillRect(ballX, ballY, 7, 7, WHITE); // draw ball
  ballXold = ballX; ballYold = ballY;
}
 



void playerScored(byte player) {
  if(player == LEFT) leftPlayerScore++;
  if(player == RIGHT) rightPlayerScore++;
  //check for win
  if(leftPlayerScore == PLAY_TO || rightPlayerScore == PLAY_TO) {
    gameover = 1;
  }
 ballVolX = -ballVolX;  // change ball direction
}
 
  
 
void drawMenu() {
  //tone (4,1300,100);
  RIGHT_PADDLE_X = 70;
  processInputs();
  
  tft.fillScreen(BLACK);
  tft.setTextSize(3);
  tft.setCursor(50,1);
  tft.setTextColor(WHITE, BLACK);
  tft.print("Arduino Pong");
  tft.setCursor(80,200);
  tft.setTextSize(2);
  tft.print("Start");
  tft.setTextSize(1);
  tft.setCursor(190,220);
  tft.print("2016 H.P.Dietrich");
 
  while (wheelOnePosition < 923)  {
   processInputs();
  
   PADDLE_HEIGHT = 20;
   menuPaddle();
   
   tft.setTextSize(2);
   tft.setTextColor(WHITE, BLACK);
  if (game == 1)   { tft.setCursor(80,40); tft.print("Squash");  }
  if (game == -1)  { tft.setCursor(80,40); tft.print("Tennis"); }
  if (fast == 1)   { tft.setCursor(80,80); tft.print("Slow");}
  if (fast == -1)  { tft.setCursor(80,80); tft.print("Fast");}
  if (easy == 1)   { tft.setCursor(80,120);tft.print("Easy");}
  if (easy == -1)  { tft.setCursor(80,120);tft.print("Hard");}
  if (sound == 1)  { tft.setCursor(80,160);tft.print("Sound on ");  }
  if (sound == -1) { tft.setCursor(80,160);tft.print("Sound off");  }
  
  //gettouchpoint();
  button1Status = digitalRead(BUTTON); // 0 = Button pressed
    
  
   //Set Game
   if (button1Status == 1 && wheelOnePosition >150 && wheelOnePosition < 280){
    game = -game; 
    delay(200);
  };
    
  // Set Speed
  if (button1Status == 1 && wheelOnePosition >320 && wheelOnePosition < 450){
    fast = -fast; 
    delay(200);
  };
  
  // set easy
  if (button1Status == 1 && wheelOnePosition >480 && wheelOnePosition < 630){
    easy = -easy;
    delay(200);
  };
  
  // set sound
  if (button1Status == 1 && wheelOnePosition >660 && wheelOnePosition < 800){
    sound = -sound;
    delay(200);
  };

    tft.setTextSize(2);

  if (easy == 1){
    PADDLE_HEIGHT = 40;  //Easy Game, big Paddle
    ballX = 11;
  }
  if (easy == -1){
    PADDLE_HEIGHT = 25;   //Hard Game, smaller Paddle
    ballX = 11;            //start position of ball
  }

  if (fast == 1 ) {  // normal Speed
    MAX_Y_VELOCITY = 2;   //higher max y speed.
    if (game == 1) ballVolX = 2;         // Balls speed for sqash
    else ballVolX = 3;                   // and tennis
  }

  if (fast == -1 ) {  // high Speed
    MAX_Y_VELOCITY = 4;   //higher max y speed.
    if (game == 1) ballVolX = 3;         // Balls speed
    else ballVolX = 6;
  }

  // while end
 tft.setTextColor(WHITE, DARKRED);
 gameover = 0; 
 };

  state = IN_GAMEA; 
}
 

void drawTennisBox() {
  RIGHT_PADDLE_X = 300;
  LEFT_PADDLE_X = 20;
  tft.fillScreen(DARKRED);
      
   //draw net
  for(int i=5; i<319 - 4; i+=20) {
    tft.drawLine(319/2,i,319/2,i+20,YELLOW);
  }
  // had to make box a bit smaller to fit tft 
    tft.fillRect(3, 3, 4,235,WHITE );  // left
    tft.fillRect(3, 3, 315,4,WHITE ); // top
    tft.fillRect(315, 4, 316,236,WHITE ); // right
    tft.fillRect(4, 235, 316,236,WHITE ); // bottom
  state = IN_GAMEB;
}
 
 
 
 void drawSquashBox() {
  if (easy == 1) RIGHT_PADDLE_X = 300;
  if (easy == -1) RIGHT_PADDLE_X = 210;  // smaller field
  
  tft.fillScreen(DARKRED);
      
  //draw net
  for(int i=1; i<239 - 4; i+=6) {
    tft.drawLine(RIGHT_PADDLE_X+10,i,RIGHT_PADDLE_X+10,i+3,WHITE);
  }
    // draw frame 
    tft.fillRect(3, 3, 4,235,WHITE );  // left
    tft.fillRect(3, 3, RIGHT_PADDLE_X+10,4,WHITE ); // top
    tft.fillRect(3, 236, RIGHT_PADDLE_X+10,235,WHITE ); // bottom
  state = IN_GAMEB; 
}
  
  
  

 

void squash () {
  processInputs();                        // read paddle position

  if(state == IN_GAMEA)  drawSquashBox(); // draw frame box of field
  if(state == IN_GAMEB) {                 // game
      ballX += ballVolX;                  // set ball direction
      ballY += ballVolY; 
  
 // change if hit top or bottom
      if(ballY <= 8 || ballY >= 226) {     // test if ball is on border
        ballVolY = -ballVolY;             // if border, then change direction
        if (sound == 1 )tone(SOUNDPORT, 600,30  );   
      }
      
 // test right side for wall hit     
      // first test if on paddle
      if(ballVolX > 0 &&                    // if Ball go right and
         ballX >= RIGHT_PADDLE_X - 1 &&     // if ballx is on paddle and
         ballX <= RIGHT_PADDLE_X  &&        // if ballx is on paddle and
         ballY >= rightPaddleY - 1 &&           // if ball is on paddle 
         ballY <= rightPaddleY + PADDLE_HEIGHT + 1 ) { 
           ballVolX = -ballVolX;            // change x and y direction
           if (sound == 1 )tone(SOUNDPORT, 400,30  );   
           // ball speed + 2x position of paddle
           ballVolY += 2 * ((ballY - rightPaddleY) - (PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
           if (fast == -1){         // fast
             if (ballVolY >= -1 || ballVolY <= 1) ballVolX = -4;  // higher speed
             else ballVolX = -1;                                  // normal speed
           }
           
       }
 
      //limit vertical speed
      if(ballVolY > MAX_Y_VELOCITY) ballVolY = MAX_Y_VELOCITY;
      if(ballVolY < -MAX_Y_VELOCITY) ballVolY = -MAX_Y_VELOCITY;
      if(ballVolY == 0){        // if near wall and endless loop
        if (ballY < 15) ballVolY = 1;
        if (ballY > 220) ballVolY = -1;
      }
      
      // Scoring if not on paddle
      if(ballX >= RIGHT_PADDLE_X+10) {      // ball hit net wall
        playerScored(RIGHT); 
        if (sound == 1 )tone(SOUNDPORT,300,100);  
        ballX = 11;               // place ball on left wall
        ballY = random(10,210);    // position random
        ballVolY = random (2,6); // direction random
        ballVolX = 2;            // if net then new ball from left
        delay (30);
      }
  
     // test if left wall
     if(ballX <= 9) {
       ballVolX = -ballVolX;    // Switch ball direction
       if (sound == 1 )tone(SOUNDPORT, 600,30  );   
     }

       
    drawSquashScreen();
   }
  
  if(gameover == 1) {  // only if game over
    //drawTennisScreen();
    tft.setCursor(110,110);
    tft.setTextSize(2);
    tft.print("GAME OVER");
    delay(4000);
    //reset the scores
    leftPlayerScore = 0;
    rightPlayerScore = 0;
    ballVolX = -2;         // Balls speed
    ballVolY = 2;
    state = IN_MENU;
    gameover = 0;
  }
  
}




void tennis () {
  processInputs();
  if(state == IN_GAMEA)  drawTennisBox(); 
  if(state == IN_GAMEB) {
      ballX += ballVolX;
      ballY += ballVolY; 
    
  
 // change if hit top or bottom
      if(ballY <= 8 || ballY >= 226) {     // test if ball is on border
        ballVolY = -ballVolY;             // if border, then change direction
        if (sound == 1 )tone(SOUNDPORT,1000,30);   
      }
      
  // test left side for wall hit    
        if(ballVolX < 0 &&                // if Ball go left and
          ballX <= LEFT_PADDLE_X + 7 &&  // if ballx = left paddle and
          ballX >= LEFT_PADDLE_X     &&  // if ballx = left paddle and
          ballY >= leftPaddleY &&        // if bally = paddle width
          ballY <= leftPaddleY + PADDLE_HEIGHT) 
          {
            ballVolX = -ballVolX;         // change x direction
            ballVolY += 2 * ((ballY - leftPaddleY) - (PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
            if (fast == -1){         // not easy
              if (ballVolY >= -1 || ballVolY <= 1) ballVolX = 6;  // higher speed
              else ballVolX = 3;                                  // normal speed
            }
            if (sound == 1 )tone(SOUNDPORT, 400,30  );
         }
      
 // test right side for wall hit     
      if(ballVolX > 0 &&                // if Ball go right and
         ballX >= RIGHT_PADDLE_X - 7 &&  // if ballx is on paddle and
         ballX <= RIGHT_PADDLE_X  &&     // if ballx is on paddle and
         ballY >= rightPaddleY &&       // ball is on paddle
         ballY <= rightPaddleY + PADDLE_HEIGHT) 
         {
           ballVolX = -ballVolX;            // change x direction
           ballVolY += 2 * ((ballY - rightPaddleY) - (PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
           if (fast == -1){         // fast
             if (ballVolY >= -1 || ballVolY <= 1) ballVolX = -6;  // higher speed
             else ballVolX = -3;                                  // normal speed
           }
           if (sound == 1 )tone(SOUNDPORT, 400,30  );   
        }
 
      //limit vertical speed
      if(ballVolY > MAX_Y_VELOCITY) ballVolY = MAX_Y_VELOCITY;
      if(ballVolY < -MAX_Y_VELOCITY) ballVolY = -MAX_Y_VELOCITY;
      if(ballVolY == 0){        // if near wall and endless loop
        if (ballY < 15) ballVolY = 2; // give ball other direction
        if (ballY > 220) ballVolY = -2;
      }
  
 // Scoring
    if(ballX <= 9) {
        for(int i=5; i<319 - 4; i+=20) {              // redraw box
           tft.drawLine(319/2,i,319/2,i+20,YELLOW);
        }
        tft.fillRect(3, 3, 4,235,WHITE );  // left
        tft.fillRect(3, 3, 315,4,WHITE ); // top
        tft.fillRect(315, 4, 316,236,WHITE ); // right
        tft.fillRect(4, 235, 316,236,WHITE ); // bottom
      playerScored(RIGHT);                        // count score
      if (sound == 1 )tone(SOUNDPORT, 300,100  );   
      }
      
      if(ballX >= 305) {
          for(int i=5; i<319 - 4; i+=20) {          // redraw box
            tft.drawLine(319/2,i,319/2,i+20,YELLOW);
          }
          tft.fillRect(3, 3, 4,235,WHITE );  // left
          tft.fillRect(3, 3, 315,4,WHITE ); // top
          tft.fillRect(315, 4, 316,236,WHITE ); // right
          tft.fillRect(4, 235, 316,236,WHITE ); // bottom
        playerScored(LEFT);             // count score
        if (sound == 1 )tone(SOUNDPORT, 300,100  );   
      }
  
    drawTennisScreen();
   }
  
  
  if(gameover == 1) {
    tft.setTextSize(2);
    tft.setCursor(110,110);
    tft.print("GAME OVER");
    delay(4000);
    leftPlayerScore = 0;
    rightPlayerScore = 0;
    state = IN_MENU; 
    gameover = 0;
  }
  
    
}

