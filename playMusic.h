/**
 * @file playMusic.h
 *
 * @brief Header file that contains all music/sound related functions
 *
 * @author Sara Sorrentino - Mirko Bellini - Vittoria Longo
 */

/** 
 * Definition of notes to be played by the buzzer
 */
#define N_NOTES 37

#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

#define buzzerPin 40

/** 
 * Definition of notes sequence to be played
 */
int melody[] = {
   NOTE_D4,NOTE_D4,NOTE_E4,NOTE_F4,NOTE_D4,NOTE_C4,NOTE_C4,NOTE_A3,NOTE_B3,
   NOTE_D4,NOTE_D4,NOTE_E4,NOTE_F4, NOTE_D4,NOTE_C4,NOTE_C4,NOTE_F4,NOTE_G4,
   NOTE_G4,NOTE_G4,NOTE_A4,NOTE_AS4,NOTE_G4, NOTE_F4,NOTE_F4,NOTE_D4,NOTE_E4,
   NOTE_G4,NOTE_G4,NOTE_A4,NOTE_AS4,NOTE_G4,NOTE_F4,NOTE_F4,NOTE_AS4,NOTE_C5 
};

/** 
 * Definition of notes duration to be played
 */
int noteDurations[] = {
  3,3,4,4,2,2,2,2,1,
  3,3,4,4,2,2,2,2,1,
  3,3,4,4,2,2,2,2,1,
  3,3,4,4,2,2,2,2,1
};

/** Play music function
 * 
 * Play the intro music using the melody and note durations
 * 
 */
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

/** CountDown function
 * 
 * Diplay a countdown (3, 2, 1, GO!) on LCD and play sound 
 * 
 */
void countDown(){
    myScreen.clear(whiteColour);
    for(int i = 3; i > 0; i--){
      myScreen.gText(myScreen.screenSizeX()/2-25,myScreen.screenSizeY()/2-25, (String)i, redColour, whiteColour, 7, 7);
      int noteDuration = 1000/2;
      tone(buzzerPin, NOTE_D4, noteDuration);
      delay(1000);
      myScreen.clear(whiteColour);
    }
    myScreen.gText(2,myScreen.screenSizeY()/2-25, "GO!", redColour, whiteColour, 7, 7);
    int noteDuration = 1000/1;
    tone(buzzerPin, NOTE_G4,noteDuration);
    delay(1000);
    noTone(buzzerPin);

    delay(1);
    myScreen.clear(blackColour);
}
