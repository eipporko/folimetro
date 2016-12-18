/*

   FOLIMETRO

   Copyright (c) David Antunez Gonzalez 2016 <dantunezglez@gmail.com>

   All rights reserved.

   FOLIMETRO is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this library.

*/
#define DEBOUNCE_TIME     150
#define NUM_OF_LEDS       11
#define CONFIRM_DURATION  250
#define BLINK_DELAY       200
#define TIMEOUT           1000

//Pins
#define BUTTON_DOWN_PIN   0 
#define BUTTON_UP_PIN     1 

#define LED_MINUS5_PIN    2
#define LED_MINUS4_PIN    3
#define LED_MINUS3_PIN    4
#define LED_MINUS2_PIN    5
#define LED_MINUS1_PIN    6
#define LED_ZERO_PIN      7
#define LED_PLUS1_PIN     8
#define LED_PLUS2_PIN     9
#define LED_PLUS3_PIN     10
#define LED_PLUS4_PIN     16
#define LED_PLUS5_PIN     14

#define SENSOR_PIN        18

short ledPins[NUM_OF_LEDS] = {
  LED_MINUS5_PIN,
  LED_MINUS4_PIN,
  LED_MINUS3_PIN,
  LED_MINUS2_PIN,
  LED_MINUS1_PIN,
  LED_ZERO_PIN,
  LED_PLUS1_PIN,
  LED_PLUS2_PIN,
  LED_PLUS3_PIN,
  LED_PLUS4_PIN,
  LED_PLUS5_PIN
};

volatile byte level = 5;
volatile unsigned long lastInterruptTime = 0;

typedef enum ProgramState {MAIN_STATE, CHANGE_LEVEL_STATE} ProgramState_t;
ProgramState_t stateProgram = MAIN_STATE;


void increaseLevel() {
  static unsigned long lastTime = 0;
  unsigned long now = millis();
  lastInterruptTime = now;

  if (now - lastTime > DEBOUNCE_TIME) {
   
    if (stateProgram != CHANGE_LEVEL_STATE) {
      changeStateProgram(CHANGE_LEVEL_STATE);
      return;
    }
    
    if (level < NUM_OF_LEDS-1) {
      digitalWrite(ledPins[level], LOW);
      level += 1;
    }

    lastTime = now;
  }
}


void decreaseLevel() {
  static unsigned long lastTime = 0;
  unsigned long now = millis();
  lastInterruptTime = now;

  if (now - lastTime > DEBOUNCE_TIME) {

    if (stateProgram != CHANGE_LEVEL_STATE) {
      changeStateProgram(CHANGE_LEVEL_STATE);
      return;
    }
  
    if (level > 0) {
      digitalWrite(ledPins[level], LOW);
      level -= 1;
    }
      
    lastTime = now;
  }
}


void confirmAnimation() {
  byte difference = NUM_OF_LEDS - level;
  byte steps = max(difference, NUM_OF_LEDS + 1 - difference);
  for (byte i = 0; i < steps; i++) {
    if (level - i >= 0)
      digitalWrite(ledPins[level - i], HIGH); 
    if (level + i < NUM_OF_LEDS) 
      digitalWrite(ledPins[level + i], HIGH);
    delay(CONFIRM_DURATION/(steps-1));
  }
  resetLeds();
}


void resetLeds() {
  for (byte i = 0; i < NUM_OF_LEDS; i++){
    digitalWrite(ledPins[i], LOW);
  }
}


void turnOnLed(short ledIndex) {
  static byte lastLedIndex = 0;
  ledIndex = max (ledIndex, 0);
  ledIndex = min (ledIndex, NUM_OF_LEDS-1); 
  digitalWrite(ledPins[lastLedIndex], LOW);
  digitalWrite(ledPins[ledIndex], HIGH);
  lastLedIndex = ledIndex;
}


void changeStateProgram(ProgramState_t newState) {
  stateProgram = newState;
  resetLeds();
}


void mainState() {
  short pressure = analogRead(SENSOR_PIN); 
  byte ledIndex = map(pressure, 0, 1023, 0, (NUM_OF_LEDS-1)*2);
  turnOnLed(ledIndex - level);
   
  if (stateProgram != MAIN_STATE)
    resetLeds();
}


void changeLevelState() {
  digitalWrite(ledPins[level], HIGH);
  delay(BLINK_DELAY);
  digitalWrite(ledPins[level], LOW);
  delay(BLINK_DELAY);

  if (millis() - lastInterruptTime > TIMEOUT){
    confirmAnimation();
    changeStateProgram(MAIN_STATE);
  }

  if (stateProgram != CHANGE_LEVEL_STATE)
    resetLeds();
}


void setup() {
  for (byte i = 0; i < NUM_OF_LEDS; i++){
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
    
  pinMode(BUTTON_UP_PIN, INPUT);
  pinMode(BUTTON_DOWN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_UP_PIN), increaseLevel, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN_PIN), decreaseLevel, RISING);
}


void loop() {
  switch (stateProgram) {
    case MAIN_STATE:
      mainState();
      break;
    case CHANGE_LEVEL_STATE:
      changeLevelState();
    default:
      break;
  }
}
