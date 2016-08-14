void fireEvent();

void enterStartupState();
void updateStartupState();
void leaveStartupState();

void enterReadyState();
void updateReadyState();
void leaveReadyState();

void enterArmedState();
void updateArmedState();
void leaveArmedState();

void enterFiringState();
void updateFiringState();
void leaveFiringState();

void armedStatus();
void firingLEDOff();
void firingLEDOn();
void beginPulsingFiringLED();
boolean isFiringLEDOn();
void pulseFiringLED();
void endPulsingFiringLED();

boolean bleIsConnected();
void blePrint(String message);
