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

#include <math.h>

#define SENSOR_RESOLUTION 255
#define WINDOW_RESOLUTION 128
#define DEBOUNCE_TIME     150
#define NUM_OF_LEDS       9
#define CONFIRM_DELAY     50
#define BLINK_DELAY       200
#define TIMEOUT           1000

//Pins
#define BUTTON_UP_PIN     0
#define BUTTON_DOWN_PIN   1 

#define LED_MINUS4_PIN    2
#define LED_MINUS3_PIN    3
#define LED_MINUS2_PIN    4
#define LED_MINUS1_PIN    5
#define LED_ZERO_PIN      6
#define LED_PLUS1_PIN     7
#define LED_PLUS2_PIN     8
#define LED_PLUS3_PIN     9
#define LED_PLUS4_PIN     10

#define SENSOR_PIN        18

short ledPins[9] = {
  LED_MINUS4_PIN,
  LED_MINUS3_PIN,
  LED_MINUS2_PIN,
  LED_MINUS1_PIN,
  LED_ZERO_PIN,
  LED_PLUS1_PIN,
  LED_PLUS2_PIN,
  LED_PLUS3_PIN,
  LED_PLUS4_PIN,
};

short pressure = 0;
volatile byte defaultLevel = 4;
volatile unsigned long setting_time = 0;

typedef enum ProgramState {RUNNING, SET_LEVEL} ProgramState_t;
ProgramState_t stateProgram = RUNNING;

void increaseLevel() { 
  if (stateProgram != SET_LEVEL)
    changeStateProgram(SET_LEVEL);
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  setting_time = interrupt_time;
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIME) {
    if (defaultLevel < NUM_OF_LEDS-1) {
      digitalWrite(ledPins[defaultLevel], LOW);
      defaultLevel += 1;
    }

    last_interrupt_time = interrupt_time;
  }
}

void decreaseLevel() {
  if (stateProgram != SET_LEVEL)
    changeStateProgram(SET_LEVEL);
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  setting_time = interrupt_time;
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIME) { 
    if (defaultLevel > 0) {
      digitalWrite(ledPins[defaultLevel], LOW);
      defaultLevel -= 1;
    }
      
    last_interrupt_time = interrupt_time;
  }
}

void confirmAnimation() {
  byte middle_led = NUM_OF_LEDS/2;
  for (byte i = 0; i <= middle_led; i++) {
    digitalWrite(ledPins[middle_led - i], HIGH);
    digitalWrite(ledPins[middle_led + i], HIGH);
    delay(CONFIRM_DELAY);
  }
  resetLeds();
}

void resetLeds() {
  for (byte i = 0; i < NUM_OF_LEDS; i++){
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void changeStateProgram(ProgramState_t newState) {
  stateProgram = newState;
  resetLeds();
}

void setup() {
  Serial.begin(9600);

  resetLeds();
    
  pinMode(BUTTON_UP_PIN, INPUT);
  pinMode(BUTTON_DOWN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_UP_PIN), increaseLevel, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN_PIN), decreaseLevel, RISING);
}

void runningLoop() {
  pressure = analogRead(SENSOR_PIN);    
  pressure = map(pressure, 0, 1023, 0, SENSOR_RESOLUTION);
  Serial.println(pressure);  
}

void setLevelLoop() {
  digitalWrite(ledPins[defaultLevel], HIGH);
  delay(BLINK_DELAY);
  digitalWrite(ledPins[defaultLevel], LOW);
  delay(BLINK_DELAY);

  if (millis() - setting_time > TIMEOUT){
    confirmAnimation();
    changeStateProgram(RUNNING);
  }
}

void loop() {
  switch (stateProgram) {
    case RUNNING:
      runningLoop();
      break;
    case SET_LEVEL:
      setLevelLoop();
    default:
      break;
  }
}
