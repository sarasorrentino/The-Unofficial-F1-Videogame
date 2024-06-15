/**
 * @file RacingGame.ino
 *
 * @brief Main ino file that includes LCD libraries, header file and setup() and loop() functions
 *
 * @author Sara Sorrentino - Mirko Bellini - Vittoria Longo
 */
 
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
