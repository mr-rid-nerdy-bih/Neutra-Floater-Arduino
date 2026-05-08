/**
 * PH CONTROL & SIMULATION MODULE
 * Project: Neutra-Floater
 * 
 * This file handles the environmental simulation, sensor filtering, 
 * and automated dosing logic.
 */

// --- PH Configuration & Calibration ---
const float targetPH    = 7.4;      // Ideal pH level for the pool
const float doseThHold  = 0.15;     // Minimum deviation before pump activates
const float pumpFlowRate = 100.0;   // Pump speed (ml per minute)
const float mlPer01PH   = 233.33;   // ml required to lift pH by 0.1
const float smoothFac   = 0.1;      // EMA filter strength (low = more smooth)

// --- Simulation Constants ---
const unsigned long vHour = 1000;   // 1 "Simulated Hour" = 1 real second
const float decayRate     = 0.01;   // Natural pH drop per simulated hour

// --- Operational State Variables ---
bool isDosing             = false;
unsigned long pumpStartTime = 0;
unsigned long pumpDuration  = 0;
unsigned long lastUpdate    = 0;
unsigned long doseLockoutTime = 0;  // mixing delay timer
const unsigned long SETTLING_DELAY = 600000; // 10 min lockout (in ms)

float pendingPhLift     = 0;        // pH increase expected from current dose
float currentPendingMl  = 0;        // Volume being pumped in current cycle
float filteredPH        = 7.0;      // The "stabilized" pH reading used for logic
float envPH             = 7.2;      // The "True" state of the water (Simulation)

// --- Calibration Logic ---
float slope  = -5.70;               // Voltage-to-pH conversion slope
float offset = 7.00;                // Voltage-to-pH conversion offset

/**
 * Main logic loop for pH management.
 * 1. Simulates sensor hardware.
 * 2. Processes "raw" data through EMA filter.
 * 3. Handles environmental decay.
 * 4. Triggers dosing based on filtered values.
 */
void phLoop() {
  // --- 1. HARDWARE SIMULATION ---
  // Create a raw signal with +/- 0.02 random noise to test filter performance
  float noise = (random(-20, 20) / 1000.0);
  int simulatedADC = getSimulatedADC(envPH + noise);

  // --- 2. SIGNAL PROCESSING ---
  // Convert "raw" ADC to pH and apply Exponential Moving Average (EMA)
  float currentPH = getPH(simulatedADC);
  filteredPH = emaFilter(filteredPH, currentPH);

  // --- 3. ENVIRONMENTAL MODELING ---
  // pH naturally drops over time when pump is inactive and water is settled
  if (!isDosing && millis() > doseLockoutTime) {
    if (millis() - lastUpdate >= vHour) {
      envPH -= decayRate;
      
      // Feedback to Serial/Bluetooth
      bt.print("DEBUG | True: "); bt.print(envPH, 2);
      bt.print(" | Filtered: "); bt.println(filteredPH, 2);
      lastUpdate = millis();
    }
  } else {
    lastUpdate = millis(); 
  }

  // --- 4. AUTOMATED DOSING DECISION ---
  float phGap = targetPH - filteredPH;
  
  // Trigger dose if: pH is low AND system is idle AND water is settled
  if (phGap > doseThHold && !isDosing && millis() > doseLockoutTime) { 
    float doseVol = (phGap / 0.1) * mlPer01PH;
    startDose(constrain(doseVol, 0, 1000)); // Cap single dose at 1 Liter
  }
  
  // Monitor pump timers and safety ranges
  updatePumpStatus(filteredPH); 
}

/**
 * Reverses the pH formula to generate a simulated ADC value.
 * Used for testing code logic without a physical pH probe.
 */
int getSimulatedADC(float phValue) {
  float voltage = (phValue - offset) / slope;
  int adc = (voltage / 5.0) * 1023.0;
  return constrain(adc, 0, 1023);
}

/**
 * Activates the peristaltic pump and calculates timing.
 */
void startDose(float ml) {
  // Safety check from eeprom_safety.ino
  if (myData.dailyLimitCounter >= 500.0) {
    bt.println(">>> ERROR: DAILY DOSE LIMIT EXCEEDED (500ml)");
    return; 
  }

  pendingPhLift = (ml / mlPer01PH) * 0.1;
  pumpDuration = (ml / pumpFlowRate) * 60 * 1000;

  bt.print(">>> PUMPING: "); bt.print(ml); bt.println("mL");
  
  // Hardware pins (Defined in Neutra_Floater.ino)
  digitalWrite(p_pump_in3, HIGH);
  digitalWrite(p_pump_in4, LOW);
  analogWrite(p_pump, 140); 
  
  pumpStartTime = millis();
  isDosing = true;
  currentPendingMl = ml; 
}

/**
 * Monitors pump progress and handles emergency shutoffs.
 */
void updatePumpStatus(float pH) {
  // SENSOR SANITY CHECK: Shutdown if reading is unrealistic
  if (pH < 5.0 || pH > 9.0) {
    if (isDosing) {
      analogWrite(p_pump, 0); 
      isDosing = false;
      bt.println(">>> EMERGENCY STOP: PROBE OUT OF RANGE");
    }
    return;
  }

  // CHECK COMPLETION
  if (isDosing && (millis() - pumpStartTime >= pumpDuration)) {
    analogWrite(p_pump, 0); 
    isDosing = false;
    
    // Update the simulation "water" state
    envPH += pendingPhLift; 
    
    // Record to EEPROM
    myData.dailyLimitCounter += currentPendingMl;
    saveSettings(); 

    // Start mixing period
    doseLockoutTime = millis() + SETTLING_DELAY;

    bt.println(">>> DOSE COMPLETE. SETTLING PERIOD ACTIVE.");
    bt.print(">>> TOTAL DOSE TODAY: ");
    bt.print(myData.dailyLimitCounter); bt.println("mL");
  }
}

/**
 * EMA Filter: Smoothes out sensor jitter.
 */
float emaFilter(float pHold, float pHnew) {
  return (smoothFac * pHnew) + ((1.0 - smoothFac) * pHold);
}

/**
 * Formula: pH = (slope * voltage) + offset
 */
float getPH(int rawADC) {
  float voltage = rawADC * (5.0 / 1023.0);
  return (slope * voltage) + offset;
} 

/**
 * Recalibrates the offset based on a known 7.0 pH buffer.
 */
void calibrateSeven(int rawADC) {
  float v7 = rawADC * (5.0 / 1023.0);
  offset = 7.0 - (slope * v7);
  saveSettings(); 
}