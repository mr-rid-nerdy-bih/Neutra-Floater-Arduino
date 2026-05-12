// Structure to hold the live values in RAM
struct BluetoothData {
  float dist;
  float ph;
  String err = "NONE";      // Standardized to All-Caps
  
  struct DosingData {
    String msg = "IDLE";    // Standardized to All-Caps
    long due = 0;           // Numbers only as requested
    float amt = 0.0;
    String stat = "READY";  // Standardized to All-Caps
  } ds;
};

BluetoothData liveData; 

void sendTelemetry() {
  String json = "{";
  json += F("\"d\":"); json += String(liveData.dist, 1);
  json += F(",\"p\":"); json += String(liveData.ph, 2);
  json += F(",\"e\":\""); json += liveData.err;
  json += F("\",\"s\":{");
  json += F("\"m\":\""); json += liveData.ds.msg;
  json += F("\",\"t\":"); json += String(liveData.ds.due);
  json += F(",\"a\":"); json += String(liveData.ds.amt, 1);
  json += F(",\"st\":\""); json += liveData.ds.stat;
  json += F("\"}}");

  bt.println(json);
}

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