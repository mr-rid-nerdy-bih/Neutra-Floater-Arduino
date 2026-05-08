unsigned long lastResetTime = 0;
const unsigned long ONE_DAY = 86400000; // 24 hours in milliseconds

struct Settings {
  float slope;
  float offset;
  float dailyLimitCounter;
};

Settings myData;

void saveSettings() {
  EEPROM.put(0, myData); // Save the entire struct starting at address 0
}

void loadSettings() {
  EEPROM.get(0, myData);
  
  // Sanity check: if EEPROM is empty (returns NaN), set defaults
  if (isnan(myData.slope)) {
    myData.slope = -5.70;
    myData.offset = 15.0;
    myData.dailyLimitCounter = 0;
    saveSettings();
  }
}

void eepromLoop() {
  unsigned long currentMillis = millis();

  // Reset the daily dose counter every 24 hours
  if (currentMillis - lastResetTime >= ONE_DAY) {
    lastResetTime = currentMillis;
    myData.dailyLimitCounter = 0;
    saveSettings();
  }
}