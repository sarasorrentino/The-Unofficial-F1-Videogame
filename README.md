# **Unofficial F1 Game: a racing game using MSP432 LaunchPad**

**Unofficial F1 Game** is a racing game created to run on a Texas Instruments MSP-EXP432P401R TI LaunchPad using the Educational BoosterPack MKII. 

The player will be able to:
1. choose **favorite car** between Ferrari, RedBull and McLaren
2. select the **difficulty level** between Rookie, Champion and Legend
3. choose between two **drive modes**: Joystick or Accelerometer

The game consists in driving the car as far as possible avoiding collision with the enemies that consist of pseudo-random falling blocks. If the player crashes his car, the game is over and he can either restart the game with the same options or go back to settings.

## **Requirements**
### **Hardware Requirements**
Unofficial F1 Game requires:
* Texas Instruments MSP-EXP432P401R LaunchPad Development Kit
<p align="center">

  <img width="605" alt="Texas Instruments MSP-EXP432P401R LaunchPad" src="https://user-images.githubusercontent.com/116173653/219174839-cdb2b76e-4b0b-4451-aec3-d8710efcbfb5.png">

</p>

* [Texas Instruments Educational BoosterPack MKII](https://www.ti.com/tool/BOOSTXL-EDUMKII)

<p align="center">

  <img width="644" alt="Texas Instruments Educational BoosterPack MKII" src="https://user-images.githubusercontent.com/116173653/219175090-fb048ad8-1814-4b8e-997b-5a3a2e760bf2.png">

</p>

### **Software Requirements**
In order to develop and upload the code, we used ENERGIA, an open source & community-driven integrated development environment (IDE) & software framework provided by Texas Instruments. 

Learn more about ENERGIA and download [here](https://www.ti.com/tool/ENERGIA).

## **Project Layout**
```
racingGame.ino
    └── racingGame.h
        └── displayLogo.h
        └── playMusic.h
```

## **Build Process**
### **Verify**
    
1. The Energia environment performs some small **transformations** to make sure that the code is correct C or C++. 
2. Code is then passed to a **compiler**, which turns the high level code into object files. 
3. Code gets combined with the **standard Energia libraries** that provide basic functions like digitalWrite() or digitalRead().

<p align="center">
  <img width="500" alt="Verify Button" src="https://user-images.githubusercontent.com/116173653/219178347-2b6a327d-f1b9-4572-86dc-4223a920230d.png">
</p>

<p align="center">
  Verify button in ENERGIA IDE
</p>

### **Upload**
The result is a single [**Intel hex file**](https://en.wikipedia.org/wiki/Intel_HEX) that is then uploaded to the board over the USB connection.

<p align="center">
  <img width="500" alt="Upload Button" src="https://user-images.githubusercontent.com/116173653/219180117-3f183ad3-f3d7-424c-889e-1ea8fce8f996.png">
</p>

<p align="center">
  Upload button in ENERGIA IDE
</p>

Read more on ENERGIA Build Process [here](https://energia.nu/guide/guide_buildprocess/). 

## **Code Explaination**
### **racingGame.ino**
* #### **setup()**
    The **setup()** is a mandatory section of every ENERGIA (or Arduino-like) program. Any code that is inside the setup() is executed only once in the beginning of the program after each powerup or reset of the board.
    This function is useful to configure the Arduino, to initialize variables, pin modes, start using libraries.
* #### **loop()**
    The loop() function is the main function of all sketches. 

```
#include "racingGame.h"

void setup() {
  // Initialize LCD screen
  analogReadResolution(12);
  myScreen.begin();  
}

void loop() {
  if(current_state < NUM_STATES){
    (*fsm[current_state].state_function)();
  }
}
```

<p align="center">
  Code Extract #1: Project setup() and loop() functions
</p>

**NOTE:** You need to include both **setup()** and **loop()** functions in your sketch, even if you don’t need them for anything.

### **displayLogo.h**
The **DisplayLogo.h header file** contains all the necessary in order to print the initial game logo on the display. 
The function print every point of the logo_pixel array, created by [converting a PNG image into a C array](https://github.com/silkeh/png2c). 

```
static const uint16_t logo_pixel[] = {0x0, 0x0, 0x0, ...};

void displayLogo(){
  
  uint16_t c;
    
  for (uint16_t i=0; i<x_logo; i++) {
    for (uint16_t j=0; j<y_logo; j++) {
      if ((i < myScreen.screenSizeX()) && (j < myScreen.screenSizeY())) {
        c = logo_pixel[i*y_logo + j];
        myScreen.point(i, j, c);
      }
    }
  }
}
```

<p align="center">
  Code Extract #2: displayLogo() function from displayLogo.h
</p>

<p align="center">
  <img width="248" alt="Game Logo" src="https://user-images.githubusercontent.com/116173653/219431651-e7625ef1-11cc-4ce9-90e0-9549b60f3fe1.png">
</p>

<p align="center">
  Game starting logo
</p>

### **playMusic.h**
The **playMusic.h header file** contains all the necessary to play the introduction game theme and sounds using the **buzzer**, a device capable of reproducing sounds when electric current is passed through it. 

In order to play notes using the buzzer it is necessary to define all frequencies related to them. 

```
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147

int melody[] = {NOTE_D4,NOTE_D4, ...};
int noteDurations[] = {3,3,4, ...};

void playMusic(){
  pinMode(buzzerPin,OUTPUT);
  for (int thisNote = 0; thisNote < N_NOTES-1; thisNote++) {
    int noteDuration = 1000/noteDurations[thisNote];
    tone(buzzerPin, melody[thisNote],noteDuration);

    int pauseBetweenNotes = noteDuration + 50; //Delay between notes
    delay(pauseBetweenNotes);
    
    noTone(buzzerPin); //Stop the tone playing
  }
}
```

<p align="center">
  Code Extract #3: playMusic() function from playMusic.h
</p>

A list of notes-frequencies can be found in **ENERGIA IDE** at File → Examples → 09.EducationalBP_MKII → BuzzerBirthdayTune. 

### **racingGame.h**
The **racingGame.h header file** contains all functions in order to run the game using a Final State Machine as defined below. 

<p align="center">
  <img width="878" alt="FSM Graph" src="https://user-images.githubusercontent.com/116173653/220112097-fb00eb42-ee1d-4f83-ae5c-bac0c6177b73.png">
</p>

<p align="center">
  FSM Graph Rapresentation
</p>

```
//----------FSM Definition----------
#define NUM_STATES 9

// FSM State Declaration
typedef enum{
  STATE_INIT,
  STATE_CMD_MENU,
  STATE_SEL_CAR, 
  ...
}State_t;

// FSM Declaration
typedef struct{
  State_t state;
  void (*state_function)(void);
}StateMachine_t;

// FSM Function Declaration
void fn_STATE_INIT(void);
void fn_STATE_CMD_MENU(void);
void fn_STATE_SEL_CAR(void);
...

State_t current_state = STATE_INIT; //Set initial FSM state

// FSM Definition
StateMachine_t fsm[] = {
  {STATE_INIT, fn_STATE_INIT},
  {STATE_CMD_MENU, fn_STATE_CMD_MENU},
  {STATE_SEL_CAR, fn_STATE_SEL_CAR},
  ...
};
```

<p align="center">
  Extract Code #4: FSM declaration and definition from racingGame.h
</p>

## **Creators Contributions**
* **Sara Sorrentino:** Car accelerometer-motion, FSM implementation
* **Mirko Bellini:** Settings menu, Car joystick-motion
* **Vittoria Longo:** Blocks spawn and motion, Blocks-car collision

## **Links**
* [Power Point Presentation](https://docs.google.com/presentation/u/0/d/1aTuP5uJBlS1OylWt2JKeoc457EgEPMtVmH0oqIPdbS0/edit)
* [YouTube Video](https://youtu.be/xK9JzrQKpS0) 
