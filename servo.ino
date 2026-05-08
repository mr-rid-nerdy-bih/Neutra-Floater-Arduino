// --- Radar Configuration ---
int rDir = 60;
int rInc = 10;
unsigned long lastRadarMove = 0;

// HC-SR04 Pins (Ensure these are defined in your main .ino)
// const int trigPin = 9;
// const int echoPin = 10;

void servoMode() {
  if (sv_mode == "Radar") {
    if (millis() - lastRadarMove >= 50) { 
      
      // 1. Boundary Check
      if (rDir >= 180 || rDir <= 0) {
        rInc *= -1;
      }
      
      // 2. Move Servo
      rDir += rInc;
      sv.write(rDir);
      
      // 3. Distance Sensing
      long distance = readUltra();
      
      // 4. Output to Serial/Bluetooth
      // Format: "Angle,Distance" makes it easy to map in a Processing or p5.js app
      bt.print(rDir);
      bt.print(",");
      bt.println(distance);
      
      lastRadarMove = millis();
    }
  } else {
    sv.write(90); // Home position
  }
}

// Function to handle the HC-SR04 Trigger/Echo
long readUltra() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Trigger the sensor with a 10 microsecond HIGH pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin (returns the sound wave travel time in microseconds)
  // We use a timeout of 30000ms so the code doesn't hang if no object is found
  long duration = pulseIn(echoPin, HIGH, 30000);
  
  // Calculate distance: (time * speed of sound) / 2 (there and back)
  // 0.034 cm/us is the speed of sound
  long distance = duration * 0.034 / 2;
  
  // Return 0 or a high number if out of range to prevent logic errors
  if (distance > 400 || distance <= 0) return 400; 
  return distance;
}