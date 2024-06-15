/**
 * @file header.h
 *
 * @brief Header file that contains all music/sound related functions
 *
 * @author Sara Sorrentino - Mirko Bellini - Vittoria Longo
 */
 
#include <LCD_screen.h>
#include <LCD_screen_font.h>
#include <LCD_utilities.h>
#include <Screen_HX8353E.h>
Screen_HX8353E myScreen;

#include "displayLogo.h"
#include "playMusic.h"

/** 
 * Definition of colors used to draw on the screen
 */
#define greyColour myScreen.calculateColour(128, 128, 128)
#define orangeColour myScreen.calculateColour(255, 128, 0)
#define cyanColour myScreen.calculateColour(0, 255, 255)
#define magentaColour myScreen.calculateColour(255, 0, 255)
#define violetColour myScreen.calculateColour(138, 43, 226)
#define pinkColour myScreen.calculateColour(245, 185, 185)
#define yellowColour myScreen.calculateColour(255, 255, 0)
#define springGreen myScreen.calculateColour(0, 255, 127)
#define deepPinkColour myScreen.calculateColour(255, 20, 147)
#define turquoiseColour myScreen.calculateColour(0, 206, 209)
#define darkGreenColour myScreen.calculateColour(0, 100, 0)
#define peachPuffColour myScreen.calculateColour(255, 218, 155)

int colors[10]={cyanColour, magentaColour, violetColour, pinkColour, yellowColour, springGreen, deepPinkColour, turquoiseColour, darkGreenColour, peachPuffColour};

/** 
 * Definition of analog pin constans
 */
const uint8_t joystickX = 2; //X-axis pin of the joystick
const uint8_t joystickY = 26; //Y_axis pin of the joystick
const uint8_t xpin = 23; //X-axis pin of the accelerometer
const uint8_t ypin = 24; //Y-axis pin of the accelerometer
const uint8_t redLED = 39;

/** Buttons Management
 * 
 * 1. Variables definition
 * 2. CheckButtons --> loop that check for both button S1 and S2 and sets triggeredButton variable by consequence
 * 
 */
const uint8_t buttonOne = 33;
const uint8_t buttonTwo = 32;

uint8_t buttonOneState = 0;
uint8_t buttonTwoState = 0;

uint8_t triggeredButton = 0;

void checkButtons(){
  while(1){
    if(digitalRead(buttonOne) == LOW){ triggeredButton = 1; break; } 
    if(digitalRead(buttonTwo) == LOW){ triggeredButton = 2; break; }
  }
}

//----------------------------------------Game Management---------------------------------------
/**
 * Drawing constants
 */
const uint8_t grassWidth = 15; //Width of grass on the side of the road
const uint8_t carHeight = 5; //Height of the car from the bottom edge of the screen
const uint8_t carWidth = 10; //Width of the car
const uint8_t carLength = 22; //Lenght of the car
const uint8_t tyreDim = 5; //Tyre dimension (square)
const uint8_t blockDim = 10; //Blocks dimension (square)
const uint8_t offset = 30; //Y-Axis offset

/**
 * Blocks constant definition
 */
#define MAX_BLOCKS 7
uint8_t x_block[MAX_BLOCKS], y_block[MAX_BLOCKS], b_color[MAX_BLOCKS];
uint8_t current_n_blocks = 0;

/**
 * Car coordinates definition
 */
uint8_t x00, y00;
uint8_t x = myScreen.screenSizeX()/2;
uint8_t y = myScreen.screenSizeY()-offset;

/**
 * Game variables definition
 */
uint8_t record = 0; //Score record of the session (until settings reset)
uint8_t score = 0; //Current game score
uint8_t tmp_score = 0;
uint8_t timer = 0; //Time to wait before random spawning
uint8_t vel = 0; //Block's falling velocity
bool collision = false;

//----------------------------------------Selection Menu----------------------------------------
#define N_cars 3
#define N_diff 3
#define N_modes 2

uint8_t cursor = 0;

String carOptions[N_cars] = {"- Ferrari","- RedBull","- McLaren"};
String difficultyOptions[N_diff] = {"- Rookie","- Champion","- Legend"};
String modeOptions[N_modes] = {"- Joystick","- Accelerometer"};

int carColor = redColour;
uint8_t vel00 = 1; //Block's initial falling velocity (1, 2, 3)
uint8_t waitTime = 40; //Time between spawn try (40, 35, 35)
uint8_t upperRandom = 30; //Upper bound of spawn probability (30, 30, 30)
uint8_t collectPoints = 5; //Points after which the speed increases (5, 6, 5)
uint8_t blocksNumber = 5; //Max number of blocks on screen (5, 7, 7)
bool driveMode = true; //Drive mode: true = analog, false = accelerometer

//--------------------------------------FSM Definition--------------------------------------
#define NUM_STATES 9

/**
 * FSM State Declaration
 */
typedef enum{
  STATE_INIT,
  STATE_CMD_MENU,
  STATE_SEL_CAR, 
  STATE_SEL_DIFF,
  STATE_SEL_MODE,
  STATE_CMD_GAME,
  STATE_INIT_GAME,
  STATE_GAME,
  STATE_GAME_OVER
}State_t;

/**
 * FSM Declaration
 */
typedef struct{
  State_t state;
  void (*state_function)(void);
}StateMachine_t;

/**
 * FSM Function Declaration
 */
void fn_STATE_INIT(void);
void fn_STATE_CMD_MENU(void);
void fn_STATE_SEL_CAR(void);
void fn_STATE_SEL_DIFF(void);
void fn_STATE_SEL_MODE(void);
void fn_STATE_CMD_GAME(void);
void fn_STATE_INIT_GAME(void);
void fn_STATE_GAME(void);
void fn_STATE_GAME_OVER(void);

State_t current_state = STATE_INIT; //Set initial FSM state

/**
 * FSM Definition
 */
StateMachine_t fsm[] = {
  {STATE_INIT, fn_STATE_INIT},
  {STATE_CMD_MENU, fn_STATE_CMD_MENU},
  {STATE_SEL_CAR, fn_STATE_SEL_CAR},
  {STATE_SEL_DIFF, fn_STATE_SEL_DIFF},
  {STATE_SEL_MODE, fn_STATE_SEL_MODE},
  {STATE_CMD_GAME, fn_STATE_CMD_GAME},
  {STATE_INIT_GAME, fn_STATE_INIT_GAME},
  {STATE_GAME, fn_STATE_GAME},
  {STATE_GAME_OVER, fn_STATE_GAME_OVER}
};

/** FSM Function Definitions
 * 
 * 1. STATE_INIT --> init state where displayLogo and playMusic functions are called
 * 2. CMD_MENU --> command menu state shows on LCD display commands in order to move through settings menu
 * 3. SEL_CAR --> select car state displays three car option to choose
 * 4. SEL_DIFF --> select difficulty state displays three difficulty option to choose
 * 5. SEL_MODE --> select mode state displays two drive mode option to choose
 * 6. CMD_GAME --> command game state shows on LCD display game commands 
 * 7. INIT_GAME --> init game state initiates all game variables, diplays the countdown and prints game background
 * 8. GAME --> game state is the actual game. Player car and enemy's blocks are displayed 
 * 9. GAME_OVER --> game over state waits for user input: S1 goes back to select car state, S2 restarts game with same settings
 * 
 */
void fn_STATE_INIT(){
  displayLogo();
  playMusic();
  current_state = STATE_CMD_MENU;
}

void fn_STATE_CMD_MENU(){
  //Print how to navigate settings
  myScreen.clear(whiteColour);
  myScreen.setFontSolid(false);
  myScreen.gText(10,15, "Menu Cmds", redColour, whiteColour, 2, 2);
  myScreen.gText(1,45, "- Move the analog to", blackColour);
  myScreen.gText(1,55, "  select the option.", blackColour);
  myScreen.gText(1,70, "- Press S1 to go to", blackColour);
  myScreen.gText(1,80, "  previous page.", blackColour);
  myScreen.gText(1,95, "- Press S2 to go to", blackColour);
  myScreen.gText(1,105, "  next page.", blackColour);
  myScreen.gText(102, myScreen.screenSizeY()-10, "Next", blackColour, yellowColour);

  while(1){
    //Manage "next" button
    buttonTwoState = digitalRead(buttonTwo); //Read the state of ButtonTwo (S2)
    if(buttonTwoState == LOW){  //If S2 is pressed, "next" text background turns yellow
      myScreen.setFontSolid(true);
      myScreen.gText(102, myScreen.screenSizeY()-10, "Next", blackColour, yellowColour);
      myScreen.setFontSolid(false);
      current_state = STATE_SEL_CAR; //Go to next page
      return;
    }
  }
}

void fn_STATE_SEL_CAR(){
  //After game over and S1, reset record 
  record = 0;

  //Print titles and buttons
  myScreen.clear(whiteColour);
  myScreen.gText(15,15, "SETTINGS", redColour, whiteColour, 2, 2);
  myScreen.gText(1,45, "Choose your car:", blackColour);
  myScreen.gText(2, myScreen.screenSizeY()-10, "Back", blackColour);
  myScreen.gText(102, myScreen.screenSizeY()-10, "Next", blackColour);

  cursor = 0;
 
  while(1){  
    
    //Select option with analog
    if(map(analogRead(joystickY), 0, 4096, 0, 100) < 20){
      if(cursor < N_cars-1){ cursor++; }
      else{ cursor = N_cars; }
    }
    else if(map(analogRead(joystickY), 0, 4096, 0, 100) > 80){
      if(cursor>0){cursor--;}
      else{cursor = 0;}
    }
    
    //Highlight selected option
    myScreen.setFontSolid(true);
    if(cursor == 0){
      myScreen.gText(5,60+15*0, carOptions[0], blackColour, yellowColour);        
      myScreen.gText(5,60+15*1, carOptions[1], blackColour, whiteColour);
      myScreen.gText(5,60+15*2, carOptions[2], blackColour, whiteColour);
    }
    else if(cursor == 1){
      myScreen.gText(5,60+15*1, carOptions[1], blackColour, yellowColour);
      myScreen.gText(5,60+15*0, carOptions[0], blackColour, whiteColour);
      myScreen.gText(5,60+15*2, carOptions[2], blackColour, whiteColour);
    }
    else{
      myScreen.gText(5,60+15*2, carOptions[2], blackColour, yellowColour);
      myScreen.gText(5,60+15*1, carOptions[1], blackColour, whiteColour);
      myScreen.gText(5,60+15*0, carOptions[0], blackColour, whiteColour);
    }
    myScreen.setFontSolid(false);
    
    //Manage "back" button
    buttonOneState = digitalRead(buttonOne); //Read the state of ButtonOne (S1)
    if(buttonOneState == LOW){  //If S1 is pressed, "back" text background turns yellow
      myScreen.setFontSolid(true);
      myScreen.gText(2, myScreen.screenSizeY()-10, "Back", blackColour, yellowColour);
      myScreen.setFontSolid(false);
      current_state = STATE_CMD_MENU;
      return;
    } 
    //Manage "next" button
    buttonTwoState = digitalRead(buttonTwo); //Read the state of ButtonTwo (S2)
    if(buttonTwoState == LOW){  //If S2 is pressed, "next" text background turns yellow
      myScreen.setFontSolid(true);
      myScreen.gText(102, myScreen.screenSizeY()-10, "Next", blackColour, yellowColour);
      myScreen.setFontSolid(false);
      
      if(cursor == 0){ carColor = redColour; }else if(cursor == 1){ carColor = blueColour; } else{ carColor = orangeColour; } //Set car colour based on cursor (selected option)
      
      current_state = STATE_SEL_DIFF;
      return;
    }
  }
}

void fn_STATE_SEL_DIFF(){
  //Print titles and buttons
  myScreen.clear(whiteColour);
  myScreen.gText(15,15, "SETTINGS", redColour, whiteColour, 2, 2);
  myScreen.gText(1,45, "Select difficulty:", blackColour, yellowColour);
  myScreen.gText(2, myScreen.screenSizeY()-10, "Back", blackColour);
  myScreen.gText(102, myScreen.screenSizeY()-10, "Next", blackColour);

  cursor = 0;
  while(1){    
    //Select option with analog
    if(map(analogRead(joystickY), 0, 4096, 0, 100) < 20){
      if(cursor < N_diff-1){ cursor++; }
      else{ cursor = N_diff; }
    }
    else if(map(analogRead(joystickY), 0, 4096, 0, 100) > 80){
      if(cursor>0){ cursor--; }
      else{ cursor = 0; }
    }

    //Highlight selected option
    myScreen.setFontSolid(true);
    if(cursor == 0){
      myScreen.gText(5,60+15*0, difficultyOptions[0], blackColour, yellowColour);        
      myScreen.gText(5,60+15*1, difficultyOptions[1], blackColour, whiteColour);
      myScreen.gText(5,60+15*2, difficultyOptions[2], blackColour, whiteColour);
    }
    else if(cursor == 1){
      myScreen.gText(5,60+15*1, difficultyOptions[1], blackColour, yellowColour);
      myScreen.gText(5,60+15*0, difficultyOptions[0], blackColour, whiteColour);
      myScreen.gText(5,60+15*2, difficultyOptions[2], blackColour, whiteColour);
    }
    else{
      myScreen.gText(5,60+15*2, difficultyOptions[2], blackColour, yellowColour);
      myScreen.gText(5,60+15*1, difficultyOptions[1], blackColour, whiteColour);
      myScreen.gText(5,60+15*0, difficultyOptions[0], blackColour, whiteColour);
    }
    myScreen.setFontSolid(false);
    
    //Manage "back" button
    buttonOneState = digitalRead(buttonOne); //Read the state of ButtonOne (S1)
    if(buttonOneState == LOW){  //If S1 is pressed, "back" text background turns yellow
      myScreen.setFontSolid(true);
      myScreen.gText(2, myScreen.screenSizeY()-10, "Back", blackColour, yellowColour);
      myScreen.setFontSolid(false);
      current_state = STATE_SEL_CAR;
      return;
    } 
    //Manage "next" button
    buttonTwoState = digitalRead(buttonTwo); //Read the state of ButtonTwo (S2)
    if(buttonTwoState == LOW){  //If S2 is pressed, "next" text background turns yellow
      myScreen.setFontSolid(true);
      myScreen.gText(102, myScreen.screenSizeY()-10, "Next", blackColour, yellowColour);
      myScreen.setFontSolid(false);
      
      switch(cursor){ //Set difficulty based on cursor (selected option)
        case 0:
          vel00 = 1;
          waitTime = 40;
          upperRandom = 30;
          collectPoints = 5;
          blocksNumber = 5;
          break;
        case 1:
          vel00 = 2;
          waitTime = 35;
          upperRandom = 30;
          collectPoints = 6;
          blocksNumber = 7;
          break;
        case 2:
          vel00 = 3;
          waitTime = 35;
          upperRandom = 40;
          collectPoints = 5;
          blocksNumber = 7;
          break;
      }
      current_state = STATE_SEL_MODE;
      return;
    }
  }
}

void fn_STATE_SEL_MODE(){
  //Print titles and buttons
  myScreen.clear(whiteColour);
  myScreen.gText(15,15, "SETTINGS", redColour, whiteColour, 2, 2);
  myScreen.gText(2,45, "Select drive mode:", blackColour, yellowColour);
  myScreen.gText(2, myScreen.screenSizeY()-10, "Back", blackColour);
  myScreen.gText(102, myScreen.screenSizeY()-10, "Next", blackColour);

  cursor = 0;
  while(1){    
    //Select option with analog
    if(map(analogRead(joystickY), 0, 4096, 0, 100) < 20){
      if(cursor < N_modes-1){ cursor++; }
      else{ cursor = N_modes; }
    }
    else if(map(analogRead(joystickY), 0, 4096, 0, 100) > 80){
      if(cursor>0){ cursor--; }
      else{ cursor = 0; }
    }

    //Highlight selected option
    myScreen.setFontSolid(true);
    if(cursor == 0){
      myScreen.gText(5,60+15*0, modeOptions[0], blackColour, yellowColour);        
      myScreen.gText(5,60+15*1, modeOptions[1], blackColour, whiteColour);
    }
    else if(cursor == 1){
      myScreen.gText(5,60+15*1, modeOptions[1], blackColour, yellowColour);
      myScreen.gText(5,60+15*0, modeOptions[0], blackColour, whiteColour);
    }
    myScreen.setFontSolid(false);
    
    //Manage "back" button
    buttonOneState = digitalRead(buttonOne); //Read the state of ButtonOne (S1)
    if(buttonOneState == LOW){  //If S1 is pressed, "back" text background turns yellow
      myScreen.setFontSolid(true);
      myScreen.gText(2, myScreen.screenSizeY()-10, "Back", blackColour, yellowColour);
      myScreen.setFontSolid(false);
      current_state = STATE_SEL_DIFF;
      return;
    } 
    //Manage "next" button
    buttonTwoState = digitalRead(buttonTwo); //Read the state of ButtonTwo (S2)
    if(buttonTwoState == LOW){  //If S2 is pressed, "next" text background turns yellow
      myScreen.setFontSolid(true);
      myScreen.gText(102, myScreen.screenSizeY()-10, "Next", blackColour, yellowColour);
      myScreen.setFontSolid(false);
      
      if(cursor == 0){ driveMode = true; } else{ driveMode = false; } //Set drive mode based on cursor (selected option)
      
      current_state = STATE_CMD_GAME;
      return;
    }
  }
}

void fn_STATE_CMD_GAME(){
  //Print how to navigate settings
  myScreen.clear(whiteColour);
  myScreen.setFontSolid(false);
  myScreen.gText(10,15, "Game Cmds", redColour, whiteColour, 2, 2);
  myScreen.gText(7,45, "Avoid the obstacles", blackColour);
  myScreen.gText(15,55, "and earn as many", blackColour);
  myScreen.gText(10,65, "points as you can!", blackColour);
  myScreen.gText(1,85, "S1: switch drive mode", blackColour);
  myScreen.gText(1,95, "S2: restart race", blackColour);
  
  //Print "Back" and "PLAY" buttons
  myScreen.gText(2, myScreen.screenSizeY()-10, "Back", blackColour);
  myScreen.gText((myScreen.screenSizeX()/2)+13,myScreen.screenSizeY()-10, "PLAY", blackColour, yellowColour, 2);

  cursor = 0;
  while(1){
    //Manage "back" button
    buttonOneState = digitalRead(buttonOne); //Read the state of ButtonOne (S1)
    if(buttonOneState == LOW){  //If S1 is pressed, "back" text background turns yellow
      myScreen.setFontSolid(true);
      myScreen.gText(3,myScreen.screenSizeY()-10, "Back", blackColour, yellowColour);
      myScreen.setFontSolid(false);
      current_state = STATE_SEL_MODE;
      return;
    } 
    //Manage "PLAY" button
    buttonTwoState = digitalRead(buttonTwo); //Read the state of ButtonTwo (S2)
    if(buttonTwoState == LOW){  //If S2 is pressed, "next" text background turns yellow
      myScreen.setFontSolid(true);
      myScreen.gText((myScreen.screenSizeX()/2)+13,myScreen.screenSizeY()-10, "PLAY", blackColour, redColour, 2);
      myScreen.setFontSolid(false);      
      current_state = STATE_INIT_GAME;
      return;
    }
  }
}

void fn_STATE_INIT_GAME(){
  pinMode(redLED, OUTPUT); //Set redLED as OUTPUT
  x00 = grassWidth+tyreDim; //Setup car zero-position

  //Initialise blocks spawning positions
  for(int i = 0; i < blocksNumber; i++){
    x_block[i]=random(16, 103);
    y_block[i]=0;
  }

  //Reset game variables
  score = 0;  
  tmp_score = 0;
  timer = 0;
  vel = vel00;
  current_n_blocks = 0;
  collision = false;

  //Launch countdown
  countDown();

  //Setup background
  myScreen.setPenSolid(true);
  myScreen.dRectangle(0, 0, grassWidth, myScreen.screenSizeY(), greenColour);
  myScreen.dRectangle(myScreen.screenSizeX()-grassWidth, 0, grassWidth, myScreen.screenSizeY(), greenColour);
  
  current_state = STATE_GAME;
}

void fn_STATE_GAME(){
 
  while(1){
    //Manage buttons
    buttonOneState = digitalRead(buttonOne);
    buttonTwoState = digitalRead(buttonTwo);

    if(buttonOneState == LOW){ if(driveMode==true){ driveMode = false; } else{ driveMode = true; }} //ButtonOne (S1) = Switch between drive modes
    if(buttonTwoState == LOW){ current_state = STATE_INIT_GAME; break;} //ButtonTwo (S2) = Reset the game
    
    //---------------------------------------------------------------CAR MOTION---------------------------------------------------------------
    //Map analogRead based on drive mode
    if(driveMode == 1){ x = map(analogRead(joystickX), 0, 4096, 0, 128); y = map(analogRead(joystickY), 0, 4096, 128, 0)+offset; }
    else{ x = map(analogRead(xpin), 1250, 2850, 0, 128); y = map(analogRead(ypin), 2850, 1250, 0, 128); }

    //Borders delimination
    if(x < grassWidth+tyreDim){ x = grassWidth+tyreDim; }
    if(x > myScreen.screenSizeX()-(grassWidth+carWidth+tyreDim)){ x = myScreen.screenSizeX()-(grassWidth+carWidth+tyreDim); }
    if(y > myScreen.screenSizeY()-carLength){ y =  myScreen.screenSizeY()-carLength; }
    
    //Draw car
    if ((x00 != x) && (y00 != y) && !collision) { //Draws only if position changes
      myScreen.dRectangle(x00, y00, carWidth, carLength, blackColour); //Delete car body
      myScreen.dRectangle(x00-tyreDim, y00, tyreDim, tyreDim, blackColour); //Delete left front wheel
      myScreen.dRectangle(x00-tyreDim, y00+carLength-tyreDim, tyreDim, tyreDim, blackColour); //Delete left back wheel
      myScreen.dRectangle(x00+carWidth, y00, tyreDim, tyreDim, blackColour); //Delete right front wheel
      myScreen.dRectangle(x00+carWidth, y00+carLength-tyreDim, tyreDim, tyreDim, blackColour); //Delete right back wheel
      
      myScreen.dRectangle(x, y, carWidth, carLength, carColor); //Draw car body
      myScreen.dRectangle(x-tyreDim, y, tyreDim, tyreDim, greyColour); //Draw left front wheel
      myScreen.dRectangle(x-tyreDim, y+carLength-tyreDim, tyreDim, tyreDim, greyColour); //Draw left back wheel
      myScreen.dRectangle(x+carWidth, y, tyreDim, tyreDim, greyColour); //Draw right front wheel
      myScreen.dRectangle(x+carWidth, y+carLength-tyreDim, tyreDim, tyreDim, greyColour); //Draw right back wheel
      
      x00 = x;
      y00 = y;
    }
    
    //------------------------------------------BLOCKS SPAWNING----------------------------------------------------------------------------------------------
    uint8_t r = timer % waitTime;
    
    if(current_n_blocks<blocksNumber && (r == 0)){ //Every n=waitTime cycles...
      if(random(100) > upperRandom){ //...possibility of a block to spawn 
        current_n_blocks++;
        timer=0;
      }
    }
    timer++;

    //-------------------------------------------------------------BLOCKS MOTION--------------------------------------------------------------
     
    for(int i = 0; i < current_n_blocks; i++){

      if (!collision) {
        y_block[i] = y_block[i] + vel;
        myScreen.dRectangle(x_block[i], y_block[i]-vel, blockDim, vel, blackColour); //Erase previous block (old position)
        myScreen.dRectangle(x_block[i], y_block[i], blockDim, blockDim, colors[i]);  //Draws block (new position)
      }
      if(y_block[i] >= myScreen.screenSizeY()){ //If block reaces bottom of the screen...
        score ++; //Update score
        tmp_score++;
        y_block[i]=0;
        x_block[i]=random(grassWidth, (myScreen.screenSizeX()-grassWidth - blockDim));
      }

      if(tmp_score == (collectPoints+vel)){ vel++; tmp_score=0; digitalWrite(redLED, HIGH); delay(100); digitalWrite(redLED, LOW); } //Every n=collectPoints points earned, increase block's falling velocity and blink redLED

      //---------------------------------------------------COLLISION----------------------------------------------------
      if(((y_block[i]+blockDim) >= y00) && (y_block[i]  <= (y00+carLength))){ //Y-Axis collision check
        if(((x_block[i]+blockDim) >= (x00 - tyreDim)) && (x_block[i] <= (x00+tyreDim+carWidth))){ //X-Axis collision check
          //If collision occurred, go to game over state
          collision = true;
          current_state = STATE_GAME_OVER;
          return;
        }
      }
    }

    //Print score
    myScreen.dRectangle(0, 0, grassWidth, grassWidth, greenColour); 
    myScreen.gText(1, 2, (String)score, redColour, greenColour);
        
    delay(100);
  }
}

void fn_STATE_GAME_OVER(){ 
  //Print GAME OVER 
  myScreen.setFontSolid(true);
  myScreen.gText(27, 30, "GAME", redColour, 3, 3); 
  myScreen.gText(27, 45, "OVER", redColour, 3, 3);

  //Print score and (new) record
  if(score > record) record = score;
  myScreen.gText(15, (myScreen.screenSizeY()/2+10), "Score:" + (String)score, redColour, 2, 2);
  myScreen.gText(10, (myScreen.screenSizeY()/2+25), "Record:" + (String)record, redColour, 2, 2);
  myScreen.setFontSolid(false);

  //Wait for buttons to be triggered...
  checkButtons();
  if(triggeredButton == 1){ current_state = STATE_SEL_CAR; return; }
  if(triggeredButton == 2){ current_state = STATE_INIT_GAME; return; }
}
