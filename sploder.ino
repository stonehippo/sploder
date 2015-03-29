/*
SPLODER - a connected button for blowing stuff up. Or whatever else you want to do with it.

Check the README.md for info on the Sploder circuit.

Copyright (C) 2015 George White <stonehippo@gmail.com>. All rights reserved.

See https://raw.githubusercontent.com/stonehippo/sploder/master/LICENSE.txt for license details.

This application makes use of a modified version of the Firmata library and some code herein is adapted
from the Adafruit StandardFirmata modified to work with their BLE library. The following copyrights and license
apply to that code.

  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2011 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2014 Limor Fried/Kevin Townsend  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
See https://raw.githubusercontent.com/stonehippo/sploder/master/FIRMATA_LICENSE.txt for license details on this code.
*/

// Override enabling of logging and debug output
//#define DEBUG false

#include <FiniteStateMachine.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BLE_Firmata.h>
#include "Adafruit_BLE_UART.h"
#include "LogHelpers.h"
#include "TimingHelpers.h"

// Hold the state of the arming switch
boolean armed = false;

// Pins configs for the buttons and switches
const byte ARMING_SWITCH = 8;
const byte TRIGGER_BUTTON = 3;
const byte ARMED_LED = 4;
const byte FIRING_LED = 5;

// Pin configs for the Adafruit nrf8001 breakout board
const byte BLE_REQUEST = 10;
const byte BLE_READY = 2;
const byte BLE_RESET = 9;

// Create a Bluetooth LE UART instance to set up the basic connection
Adafruit_BLE_UART BluetoothLESerial = Adafruit_BLE_UART(BLE_REQUEST, BLE_READY, BLE_RESET);

// Set up Firmata so we can chat with an app
Adafruit_BLE_FirmataClass firmata(BluetoothLESerial);

// Managing debounce of the button used to FIRE!
long lastDebounceTime = 0;
long debounceDelay = 250;

// We'll hold timing information about certain states here
long timerStartupState = 0;
long timerFiringState = 0;

// FSM states
State startupState = State(enterStartupState,updateStartupState,leaveStartupState);
State readyState = State(enterReadyState,updateReadyState,leaveReadyState);
State armedState = State(enterArmedState,updateArmedState,leaveArmedState);
State firingState = State(enterFiringState,updateFiringState,leaveFiringState);

// Kicking off the FSM
FSM stateMachine = FSM(startupState);

// ******************* BASIC ARDUINO SETUP & LOOP ******************* 

void setup() {
  pinMode(TRIGGER_BUTTON, INPUT_PULLUP);
  pinMode(ARMING_SWITCH, INPUT_PULLUP);
  pinMode(ARMED_LED, OUTPUT);
  pinMode(FIRING_LED, OUTPUT);
  
  attachInterrupt(1,fireEvent,HIGH);
  startLog();
}

void loop() {
    stateMachine.update();
}


// ******************* INTERRUPT EVENTS ******************* 

void fireEvent() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (stateMachine.isInState(armedState)) {
      stateMachine.transitionTo(firingState);
    }
    lastDebounceTime = millis();
  }
}

// ******************* FSM state callback methods ******************* 

// -------------- Startup State ---------------
void enterStartupState() {
  note("starting up");
  startTimer(timerStartupState);
}
void updateStartupState() {
  if(isTimerExpired(timerStartupState, 1000)) {
    stateMachine.transitionTo(readyState);
  }
}
void leaveStartupState() {
  note("startup complete");
  clearTimer(timerStartupState);
}

// -------------- Ready State ---------------
void enterReadyState() {
  note("ready");
}
void updateReadyState() {
  armedStatus();
  if (armed) {
    stateMachine.transitionTo(armedState); 
  }
}
void leaveReadyState() {}

// -------------- Armed State ---------------
void enterArmedState() {
  note("armed");
}
void updateArmedState() {
  armedStatus();
  digitalWrite(ARMED_LED, HIGH);  
  if (!armed) {
    stateMachine.transitionTo(readyState);
  }
}
void leaveArmedState() {
  note("disarmed");
  digitalWrite(ARMED_LED, LOW);
}

// -------------- Firing State ---------------
void enterFiringState () {
  note("firing!");
  digitalWrite(FIRING_LED, HIGH);

  startTimer(timerFiringState);
}
void updateFiringState() {
  if(isTimerExpired(timerFiringState, 1000)) {
    stateMachine.transitionTo(readyState);
  }
}
void leaveFiringState() {
  note("fired!");
  digitalWrite(FIRING_LED, LOW);
  clearTimer(timerFiringState);
}

// ******************* HELPERS ******************* 

// Switch state helpers
void armedStatus() {
  if (digitalRead(ARMING_SWITCH) == HIGH) {
    armed = false;
  } else {
    armed = true; 
  }
}
