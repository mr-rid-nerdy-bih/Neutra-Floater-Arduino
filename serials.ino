void btSerial() {
  if (bt.available()) {
    // Read the whole message until the newline
    String input = bt.readStringUntil('\n');
    input.trim(); // Remove spaces and \n

    if (input.length() > 0) {
      Serial.print("DEBUG: Received -> ");
      Serial.println(input);

      // Convert the string to an integer
      int num = input.toInt(); 

      if (num == 3) {
        sv_mode = "Radar";
        Serial.println("SYSTEM: Mode set to RADAR");
      } 
      else if (num == 1) {
        sv_mode = "None";
        Serial.println("SYSTEM: Mode set to NONE");
      }
      else {
        Serial.println("DEBUG: Number recognized but no mode assigned.");
      }
    }
  }
}