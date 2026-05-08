/**
 * MAIN CONTROLLER: Neutra-Floater
 * This file initializes all hardware and runs the main loop.
 * 
 * Hardware Map:
 * - Bluetooth (HC-05): D2 (RX), D3 (TX)
 * - L298N Pump: D6 (PWM), D7 (IN3), D8 (IN4)
 * - Servo Motor: D9
 * - Ultrasonic: D10 (Trig), D11 (Echo)
 * - pH Sensor: A0
 */

#include <SoftwareSerial.h>
#include <Servo.h>
#include <EEPROM.h>

// 1. OBJECT DECLARATIONS
SoftwareSerial bt(2, 3); // RX, TX for Bluetooth
Servo sv;

// 2. PIN ASSIGNMENTS
const int p_sv       = 9;  // Servo PWM
const int p_pump     = 6;  // L298N ENB (Speed)
const int p_pump_in3 = 7;  // L298N IN3 (Dir)
const int p_pump_in4 = 8;  // L298N IN4 (Dir)
const int trigPin    = 10; // Ultrasonic Trigger
const int echoPin    = 11; // Ultrasonic Echo

// 3. GLOBAL STATE
String sv_mode = "None";   // Modes: "None", "Radar"
// envPH is defined in ph.ino, but initialized here for the simulation start
extern float envPH;        

void setup() {
  // Communication
  Serial.begin(115200);
  bt.begin(9600);
  
  // Pin Modes
  pinMode(p_pump, OUTPUT);
  pinMode(p_pump_in3, OUTPUT);
  pinMode(p_pump_in4, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initial Hardware State
  digitalWrite(p_pump, LOW); // Ensure pump is OFF at boot
  sv.write(90);              // Move servo to center before attaching
  sv.attach(p_sv);
  
  // EEPROM Initial Load (Defined in eeprom_safety.ino)
  loadSettings(); 

  Serial.println(">>> NEUTRA-FLOATER ONLINE");
  bt.println("SYSTEM: Connection Established.");
}

void loop() {
  // Tasks are modularized across .ino files
  eepromLoop();   // Safety timers and memory saves
  btSerial();     // Command parsing
  phLoop();       // Simulation, filtering, and dosing
  servoMode();    // Radar sweeping and distance sensing
}