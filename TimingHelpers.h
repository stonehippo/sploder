/*
Copyright @ 2015 George White <stonehippo@gmail.com>

See https://raw.githubusercontent.com/stonehippo/sploder/master/LICENSE.txt for license details.
*/

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
