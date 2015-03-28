/*
SPLODER - a connected button for blowing stuff up. Or whatever else you want to do with it.

Check the README.md for info on the Sploder circuit.

Copyright @ 2015 George White <stonehippo@gmail.com>

See https://raw.githubusercontent.com/stonehippo/sploder/master/LICENSE.txt for license details.
*/

// Override enabling of logging and debug output
//#define DEBUG false

#include <FiniteStateMachine.h>
#include "LogHelpers.h"
#include "TimingHelpers.h"

// Hold the state of the arming switch
boolean armed = false;

// Pins configs for the buttons and switches
const byte ARMING_SWITCH = 8;
const byte TRIGGER_BUTTON = 3;
const byte ARMED_LED = 4;
const byte FIRING_LED = 5;

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
  pinMode(ARMING_SWITCH, INPUT);
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
  if (digitalRead(ARMING_SWITCH) == LOW) {
    armed = false;
  } else {
    armed = true; 
  }
}
