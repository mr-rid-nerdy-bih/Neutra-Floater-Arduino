// Structure to hold the live values in RAM
struct BluetoothData {
  float dist;
  float ph;
  int ang;          // Added for Radar angle
  String err = "NONE";
  
  struct DosingData {
    String msg = "IDLE";
    long due = 0;
    float amt = 0.0;
    String stat = "READY";
  } ds;
};

BluetoothData liveData; 

void btSerial() {
  if (bt.available()) {
    String input = bt.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      // 1. MODES
      if (input == "1") {
        sv_mode = "None";
        liveData.ds.stat = "MANUAL";
        liveData.ds.msg = "IDLE";
      } 
      else if (input == "3") {
        sv_mode = "Radar";
        liveData.ds.stat = "AUTO";
        liveData.ds.msg = "SCANNING";
      }
      // 2. CALIBRATION
      else if (input == "CAL") {
        int currentRaw = analogRead(A0);
        calibrateSeven(currentRaw); 
        
        liveData.err = "NONE";
        liveData.ds.msg = "CAL_DONE";
      }
      // 3. TELEMETRY TRIGGER (Manual override)
      else if (input == "GET") {
        sendTelemetry();
      }
      else {
        liveData.err = "UNKNOWN_CMD";
      }
    }
  }
}

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

  bt.println(json); // This is now the ONLY place bt.println is used
}