/*
SPLODER - a connected button for blowing stuff up. Or whatever else you want to do with it.
*/
#include <FiniteStateMachine.h>

// Logging and debug controls
#define DEBUG true

// Pins configs for the buttons and switches
const byte ARMING_SWITCH = 2;
const byte TRIGGER_BUTTON = 3;
const byte ARMED_LED = 4;
const byte FIRING_LED = 5;

// FSM states
State startupState = State(enterStartupState,updateStartupState,leaveStartupState);
State armedState = State(enterArmedState,updateArmedState,leaveArmedState);
State firingState = State(enterFiringState,updateFiringState,leaveFiringState);

// Kicking off the FSM
FSM stateMachine = FSM(startupState);

void setup() {
  startLog();
}

void loop() {
    stateMachine.update();
}

// FSM state callback methods

void enterStartupState() {}
void updateStartupState() {}
void leaveStartupState() {}

void enterArmedState() {}
void updateArmedState() {}
void leaveArmedState() {}

void enterFiringState () {}
void updateFiringState() {}
void leaveFiringState() {}

// Timing helpers
void startTimer(long &timer) {
  timer = millis(); 
}

boolean isTimerExpired(long &timer, int expiration) {
  long current = millis() - timer;
  return current > expiration;
}

void clearTimer(long &timer) {
  timer = 0; 
}

// Logging and debug helpers

void startLog() {
  if (DEBUG) {
    Serial.begin(9600);
    note("Now logging to serial");
  }
}

void logMessage(String message) {
  if (DEBUG) {
    Serial.println(message);
  } 
}

void note(String message) {
  String fullMessage = "NOTE: "+ message;
  logMessage(fullMessage); 
}

void warn(String message) {
  String fullMessage = "WARN: "+ message;
  logMessage(fullMessage); 
}

void error(String message) {
  String fullMessage = "ERROR: "+ message;
  logMessage(fullMessage); 
}
