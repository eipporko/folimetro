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
//For Arduino Pro Micro
#include <SoftwareSerial.h>

#define DEBOUNCE_TIME          150
#define NUM_OF_LEDS            9
#define CONFIRM_DURATION       300
#define BLINK_DELAY            200
#define TIMEOUT                1000

//Pins
#define BLUETOOTH_ENABLE_PIN   14
#define BLUETOOTH_TX_PIN       10
#define BLUETOOTH_RX_PIN       16
#define BLUETOOTH_S_WINDOW     25

#define BUTTON_DOWN_PIN        0 
#define BUTTON_UP_PIN          1

#define PIN_A                  3
#define PIN_B                  4
#define PIN_C                  5
#define PIN_D                  6

#define SENSOR_PIN             A0

SoftwareSerial btSerial(BLUETOOTH_TX_PIN, BLUETOOTH_RX_PIN);

const int controlPin[4] = {PIN_A, PIN_B, PIN_C, PIN_D}; // 4 pins used  for binary output antes (3,4,5,6)
const int muxTable[NUM_OF_LEDS+1][4] = {
  // s0, s1, s2, s3     channel
  {0,  0,  0,  0}, // 0
  {1,  0,  0,  0}, // 1
  {0,  1,  0,  0}, // 2
  {1,  1,  0,  0}, // 3
  {0,  0,  1,  0}, // 4
  {1,  0,  1,  0}, // 5
  {0,  1,  1,  0}, // 6
  {1,  1,  1,  0}, // 7
  {0,  0,  0,  1}, // 8
  {1,  0,  0,  1}, // 9
};

volatile bool decreaseButtonPressed = false;
volatile byte level = 4;
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
      level += 1;
    }

    lastTime = now;
  }
}


void decreaseLevel() {

  if (!decreaseButtonPressed) {
  static unsigned long lastTime = 0;
  unsigned long now = millis();

  lastInterruptTime = now;

  if (now - lastTime > DEBOUNCE_TIME) {

    if (stateProgram != CHANGE_LEVEL_STATE) {
      changeStateProgram(CHANGE_LEVEL_STATE);
      return;
    }
  
    if (level > 0) {
     level -= 1;
    }
     
    lastTime = now;
   }
  }
}


void blinkAllLedsAnimation() {
  for (int i = 0; i < 2; i++) {
    unsigned long timeToReach = millis() + 150;
    while (millis() < timeToReach) {
      for (int j = 0; j < NUM_OF_LEDS; j++) {
       turnOnLed(j);
      }
    }
    turnOffLeds();
    delay (150);
  }
}


void enableBluetoothAnimation() {
  byte middleLed = 4;
  byte animation_steps = 5;
  byte num_of_leds = 0;
  for (byte i = 0; i < animation_steps; i++) {
    unsigned long timeToReach = millis() + CONFIRM_DURATION/(animation_steps-1);
    num_of_leds++;
    while (millis() < timeToReach) {
      for (int j = 0; j < num_of_leds; j++) {
        turnOnLed(middleLed + j);
        delay(2);
        turnOnLed(middleLed - j);
        delay(2);
       }
    }
  }
  blinkAllLedsAnimation();
}


void disableBluetoothAnimation() {  
  byte middleLed = 4;
  byte animation_steps = 5;
  byte num_of_leds = 6;
  blinkAllLedsAnimation();
  for (byte i = 0; i < animation_steps; i++) {
    unsigned long timeToReach = millis() + CONFIRM_DURATION/(animation_steps-1);
    num_of_leds--;
    while (millis() < timeToReach) {
      for (int j = 0; j < num_of_leds; j++) {
        turnOnLed(middleLed + j);
        delay(2);
        turnOnLed(middleLed - j);
        delay(2);
       }
    }
  }
}


void turnOnLed(int numOfLed)
{
  if (numOfLed < 0) {
    turnOnLed(0);
  }
  else if (numOfLed >= NUM_OF_LEDS) {
    turnOnLed(NUM_OF_LEDS - 1);
  }
  else {
    digitalWrite(controlPin[0], muxTable[numOfLed][0]);
    digitalWrite(controlPin[1], muxTable[numOfLed][1]);
    digitalWrite(controlPin[2], muxTable[numOfLed][2]);
    digitalWrite(controlPin[3], muxTable[numOfLed][3]);
  }
}


void turnOffLeds() {
  digitalWrite(controlPin[0], 0);
  digitalWrite(controlPin[1], 0);
  digitalWrite(controlPin[2], 1);
  digitalWrite(controlPin[3], 1);
}

void changeStateProgram(ProgramState_t newState) {
  stateProgram = newState;
  turnOffLeds();
}


void mainState() {
  short pressure = analogRead(SENSOR_PIN); 
  byte ledIndex = map(pressure, 0, 1023, 0, (NUM_OF_LEDS-1)*2);
  turnOnLed(ledIndex - level);
  
  btSerial.print('#');
  btSerial.println(pressure);
  delay(BLUETOOTH_S_WINDOW);
  
  if (stateProgram != MAIN_STATE)
    turnOffLeds();
}


void changeLevelState() {
  turnOnLed(level);
  delay(BLINK_DELAY);
  turnOffLeds();
  delay(BLINK_DELAY);

  if (millis() - lastInterruptTime > TIMEOUT){
    changeStateProgram(MAIN_STATE);
  }

  if (stateProgram != CHANGE_LEVEL_STATE)
    turnOffLeds();
}


void setup() {
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode (BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode (BLUETOOTH_ENABLE_PIN, OUTPUT);
  pinMode(BUTTON_UP_PIN, INPUT);
  pinMode(BUTTON_DOWN_PIN, INPUT);

  digitalWrite(BLUETOOTH_ENABLE_PIN, LOW);
  
  btSerial.begin(9600);
  Serial.begin(9600);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON_UP_PIN), increaseLevel, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN_PIN), decreaseLevel, RISING);

  for (int i = 0; i < 4; i++)
  {
    pinMode(controlPin[i], OUTPUT);//  establece pin como salida
  }
}


void toggleBluetooth() {
  bool blEnabled = digitalRead(BLUETOOTH_ENABLE_PIN);
  if (blEnabled) {
    digitalWrite(BLUETOOTH_ENABLE_PIN, LOW);
    disableBluetoothAnimation();
  }
  else {
    digitalWrite(BLUETOOTH_ENABLE_PIN, HIGH);
    enableBluetoothAnimation();
  }
}


void loop() {
  unsigned long timePressed = 0;

  //Check if the Decrease Button is Pressed in a long way in
  //order to enable/disable bluetooth dongle
  if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
    unsigned long now = millis();
    while (digitalRead(BUTTON_DOWN_PIN) == LOW) {
      if (millis() - now > 500) {
        decreaseButtonPressed = true;
        toggleBluetooth();
        break;
      }
    }
    decreaseButtonPressed = false;
  }
  
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
