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

// Managing debounce of the button used to FIRE!
long lastDebounceTime = 0;
long debounceDelay = 250;

// We'll hold timing information about certain states here
long timerStartupState = 0;
long timerFiringState = 0;

// Status of the ARMED switch
boolean armed = false;

// FSM states
State startupState = State(enterStartupState,updateStartupState,leaveStartupState);
State readyState = State(enterReadyState,updateReadyState,leaveReadyState);
State armedState = State(enterArmedState,updateArmedState,leaveArmedState);
State firingState = State(enterFiringState,updateFiringState,leaveFiringState);

// Kicking off the FSM
FSM stateMachine = FSM(startupState);

void setup() {
  pinMode(TRIGGER_BUTTON, INPUT_PULLUP);
  pinMode(ARMING_SWITCH, INPUT_PULLUP);
  pinMode(ARMED_LED, OUTPUT);
  pinMode(FIRING_LED, OUTPUT);
  
  attachInterrupt(1,fireEvent,HIGH);

  startLog();
}

void fireEvent() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (stateMachine.isInState(armedState)) {
      stateMachine.transitionTo(firingState);
    }
    lastDebounceTime = millis();
  }
}

void loop() {
    stateMachine.update();
}

// FSM state callback methods

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
  if (!armed) {
    stateMachine.transitionTo(readyState);
  }
  digitalWrite(ARMED_LED, HIGH);
}
void leaveArmedState() {
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
