// ******************* Timing helpers ******************* 
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
