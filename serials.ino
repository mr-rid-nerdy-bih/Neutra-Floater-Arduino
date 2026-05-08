void btSerial() {
  if (bt.available()) {
    String input = bt.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      Serial.print("DEBUG: Received -> ");
      Serial.println(input);

      // Handle Numeric Modes (1=None, 3=Radar)
      if (input == "1") {
        sv_mode = "None";
        Serial.println("SYSTEM: Mode set to NONE");
      } 
      else if (input == "3") {
        sv_mode = "Radar";
        Serial.println("SYSTEM: Mode set to RADAR");
      }
      // Handle Calibration Command (e.g., "CAL")
      else if (input == "CAL") {
        int currentRaw = analogRead(A0);
        calibrateSeven(currentRaw);
        bt.print("SUCCESS: Calibrated at Offset ");
        bt.println(offset);
      }
      else {
        Serial.println("DEBUG: Unknown Command.");
      }
    }
  }
}