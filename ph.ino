// --- PH Configuration ---
const float targetPH = 7.4;
const float doseThHold = 0.15;
const float pumpFlowRate = 100.0;     // ml per minute
const unsigned long vHour = 1000;     // Simulated hour for decay
const float decayRate = 0.01;         // Natural pH drop over "vHour"
const float mlPer01PH = 233.33;       // Chemical requirement constant
const float smoothFac = 0.1;          // EMA Filter strength (0.0 to 1.0)

// --- Operational Variables ---
bool isDosing = false;
unsigned long pumpStartTime = 0;
unsigned long pumpDuration = 0;
unsigned long lastUpdate = 0;
unsigned long doseLockoutTime = 0;    // Tracks the "settling" period
const unsigned long SETTLING_DELAY = 600000; // 10 min wait after dose (in ms)

float pendingPhLift = 0;
float currentPendingMl = 0;
float filteredPH = 7.0;               // Global filtered value for stability

// --- Calibration (Updated via getPH / calibrateSeven) ---
float slope = -5.70;                  // Default theoretical slope
float offset = 7.00;                  // Default offset

void phLoop() {
  // 1. Read and Filter Sensor Data
  // Assuming 'phPin' is defined in main ino.
  int rawADC = analogRead(A0); 
  float currentPH = getPH(rawADC);
  filteredPH = emaFilter(filteredPH, currentPH);

  // 2. Simtime Decay (Environmental modeling)
  // ONLY runs if the pump is OFF and we aren't in a settling period
  if (!isDosing && millis() > doseLockoutTime) {
    if (millis() - lastUpdate >= vHour) {
      envPH -= decayRate;
      bt.print("Sim pH: "); 
      bt.println(envPH, 2);
      lastUpdate = millis();
    }
  } else {
    lastUpdate = millis(); 
  }

  // 3. Logic: Should we start a dose?
  float phGap = targetPH - envPH;
  
  // Check if pH is low, we aren't already dosing, and the water has settled
  if (phGap > doseThHold && !isDosing && millis() > doseLockoutTime) { 
    float doseVol = (phGap / 0.1) * mlPer01PH;
    startDose(constrain(doseVol, 0, 1000)); // Limit single dose to 1L
  }
  
  // 4. Maintenance: Handle pump timers and safety
  updatePumpStatus(envPH); // Pass envPH for simulation or filteredPH for real world
}

void startDose(float ml) {
  // --- STAGE 6: SAFETY LIMIT (from eeprom_safety.ino) ---
  if (myData.dailyLimitCounter >= 500.0) { // Using 500ml as a hard limit
    bt.println(">>> ERROR: DAILY DOSE LIMIT REACHED.");
    return; 
  }

  pendingPhLift = (ml / mlPer01PH) * 0.1;
  pumpDuration = (ml / pumpFlowRate) * 60 * 1000;

  bt.print(">>> PUMP START: "); bt.print(ml); bt.println("mL");
  
  // Motor Driver Control
  digitalWrite(p_pump_in3, HIGH);
  digitalWrite(p_pump_in4, LOW);
  analogWrite(p_pump, 140); 
  
  pumpStartTime = millis();
  isDosing = true;
  currentPendingMl = ml; 
}

void updatePumpStatus(float pH) {
  // --- STAGE 5: SENSOR SANITY CHECK ---
  // If sensor reads impossible values, kill the pump immediately
  if (pH < 5.0 || pH > 9.0) {
    if (isDosing) {
      analogWrite(p_pump, 0); 
      isDosing = false;
      bt.println(">>> EMERGENCY STOP: PROBE OUT OF RANGE");
    }
    return;
  }

  // --- STAGE 6: DOSING COMPLETION & PERSISTENCE ---
  if (isDosing && (millis() - pumpStartTime >= pumpDuration)) {
    analogWrite(p_pump, 0); 
    isDosing = false;
    
    envPH += pendingPhLift; 
    
    // Update daily total in the struct and save to EEPROM
    myData.dailyLimitCounter += currentPendingMl;
    saveSettings(); // Function defined in eeprom_safety.ino

    // Start the lockout timer so we don't dose again immediately
    doseLockoutTime = millis() + SETTLING_DELAY;

    bt.println(">>> DOSE COMPLETE. LOCKING SYSTEM FOR MIXING.");
    bt.print(">>> TOTAL ML TODAY: ");
    bt.println(myData.dailyLimitCounter);
  }
}

// Exponential Moving Average Filter
float emaFilter(float pHold, float pHnew) {
  return (smoothFac * pHnew) + ((1.0 - smoothFac) * pHold);
}

// Convert voltage to pH using calibration constants
float getPH(int rawADC) {
  float voltage = rawADC * (5.0 / 1023.0);
  return (slope * voltage) + offset;
} 

// Set new offset based on a pH 7.0 buffer solution
void calibrateSeven(int rawADC) {
  float v7 = rawADC * (5.0 / 1023.0);
  // Re-calculate offset based on current voltage and known slope
  offset = 7.0 - (slope * v7);
  saveSettings(); // Persist the new calibration
}