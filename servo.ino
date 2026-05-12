/**
 * RADAR & SERVO CONTROL MODULE
 * Handles the sweeping motion and distance sensing via HC-SR04.
 */

// --- Radar Configuration ---
int rDir = 90;               // Current angle (start at center)
int rInc = 5;                // Increment size (Smaller = smoother sweep)
unsigned long lastRadarMove = 0;
const int radarInterval = 50; // Delay between steps (ms)

/**
 * Sweeps the servo back and forth when in "Radar" mode.
 * Automatically sends "Angle,Distance" data over Bluetooth.
 */
void servoMode() {
  if (sv_mode == "Radar") {
    if (millis() - lastRadarMove >= radarInterval) { 
      
      // 1. DIRECTION LOGIC
      if (rDir >= 180 || rDir <= 0) {
        rInc *= -1;
      }
      
      // 2. MOVEMENT
      rDir += rInc;
      sv.write(rDir);
      
      // 3. SENSING
      long distance = readUltra();
      
      // 4. DATA UPDATE: Store values in the dictionary for the next JSON broadcast
      liveData.dist = distance;
      // Note: We'll add 'ang' to the struct in the serials file
      liveData.ang = rDir; 
      
      lastRadarMove = millis();
    }
  } 
  else {
    // IDLE MODE: Return to center
    if (rDir != 90) {
      rDir = 90;
      sv.write(rDir);
      liveData.ang = 90;
    }
  }
}

long readUltra() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  
  if (distance > 400 || distance <= 0) {
    return 400; 
  }
  
  return distance;
}