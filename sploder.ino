/*
SPLODER - a connected button for blowing stuff up. Or whatever else you want to do with it.

Check the README.md for info on the Sploder circuit.

Copyright (C) 2015 George White <stonehippo@gmail.com>. All rights reserved.

See https://raw.githubusercontent.com/stonehippo/sploder/master/LICENSE.txt for license details.
*/

// Override enabling of logging and debug output
#define DEBUG false

#include <FiniteStateMachine.h>
#include <SPI.h>
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

// Global to monitor the state of the nrf8001 Bluetooth board
aci_evt_opcode_t bleLastStatus = ACI_EVT_DISCONNECTED;

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
  BluetoothLESerial.begin();
}

void loop() {
  // What's up, Bluetooth?
  BluetoothLESerial.pollACI();
  //check the status of the Bluetooth LE connection
  aci_evt_opcode_t bleStatus = BluetoothLESerial.getState();
  if (bleStatus != bleLastStatus) {
    if (bleStatus == ACI_EVT_DEVICE_STARTED) {
      info("BLE advertising started");
    }
    if (bleStatus == ACI_EVT_CONNECTED) {
      info("BLE connected");
      // Immediately send the current state of the FSM
      // Have to use the library print() helper, as the current status has not been updated yet
      if (stateMachine.isInState(readyState)) {
        BluetoothLESerial.print("ready");
      }
      if (stateMachine.isInState(armedState)) {
        BluetoothLESerial.print("armed");
      }
      if (stateMachine.isInState(firingState)) {
        BluetoothLESerial.print("firing");
      }
    }
    if (bleStatus == ACI_EVT_DISCONNECTED) {
      info("BLE disconnected");
    }
    bleLastStatus = bleStatus;
  }
  
  // Time to handle the state machine
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
  blePrint("ready");
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
  armedStatus();
  if (armed) {
    blePrint("armed");
  }
}
void updateArmedState() {
  armedStatus();
  digitalWrite(ARMED_LED, HIGH);  
  if (!armed) {
    blePrint("disarmed");
    stateMachine.transitionTo(readyState);
  }
}
void leaveArmedState() {
  digitalWrite(ARMED_LED, LOW);
}

// -------------- Firing State ---------------
void enterFiringState () {
  blePrint("firing");
  digitalWrite(FIRING_LED, HIGH);

  startTimer(timerFiringState);
}
void updateFiringState() {
  if(isTimerExpired(timerFiringState, 1000)) {
    stateMachine.transitionTo(armedState);
  }
}
void leaveFiringState() {
  blePrint("fired");
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

// Bluetooth UART helpers
boolean bleIsConnected() {
  if (bleLastStatus == ACI_EVT_CONNECTED) {
    return true;
  }
  return false;
}

/*
  The nrf8001 breakout has a maximum send buffer of 20 bytes, so we need to
  convert message strings into a byte array for buffering.
  
  I'm also using my own helper so I can check the status of the modem easily
  to prevent hanging when there's no connection.
*/
void blePrint(String message) {
  if (bleIsConnected()) {
    uint8_t sendBuffer[20];
    message.getBytes(sendBuffer, 20);
    char sendBufferSize = min(20, message.length());
    
    BluetoothLESerial.write(sendBuffer, sendBufferSize);
  }
  
  // echo out the message to the serial console, regardless of BLE status
  note(message);
}
