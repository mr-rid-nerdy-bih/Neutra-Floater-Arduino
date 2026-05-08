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
      
      // 1. DIRECTION LOGIC: Reverse direction at boundaries
      if (rDir >= 180 || rDir <= 0) {
        rInc *= -1;
      }
      
      // 2. MOVEMENT: Increment position
      rDir += rInc;
      sv.write(rDir);
      
      // 3. SENSING: Get distance from ultrasonic sensor
      long distance = readUltra();
      
      // 4. DATA STREAM: Format for p5.js / Processing apps
      // Output example: "45,120"
      bt.print(rDir);
      bt.print(",");
      bt.println(distance);
      
      lastRadarMove = millis();
    }
  } 
  else {
    // IDLE MODE: Return to center to save power/avoid obstruction
    if (rDir != 90) {
      rDir = 90;
      sv.write(rDir);
    }
  }
}

/**
 * Triggers the Ultrasonic sensor and calculates distance in cm.
 * Uses a non-blocking timeout to prevent system lag.
 */
long readUltra() {
  // Ensure pins are clean before triggering
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Send 10us pulse to trigger pin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // pulseIn waits for the ECHO pin to go HIGH and returns travel time in us
  // Timeout set to 30ms (approx 5 meters max range)
  long duration = pulseIn(echoPin, HIGH, 30000);
  
  // Distance math: (Time [us] * Speed of Sound [0.034 cm/us]) / 2
  long distance = duration * 0.034 / 2;
  
  // Sanity check: If result is out of physical sensor bounds (2cm - 400cm)
  if (distance > 400 || distance <= 0) {
    return 400; // Return max range if no clear echo
  }
  
  return distance;
}