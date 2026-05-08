// 1. PIN CONFIGURATION & LIBRARIES
#include <SoftwareSerial.h>
#include <Servo.h>
#include <EEPROM.h>

// Updated Pins based on our unified pin map
SoftwareSerial bt(2, 3); // RX, TX
Servo sv;

const int p_sv = 9;      // Servo moved to D9 (Hardware PWM)
const int p_pump = 6;    // ENB pin
const int p_pump_in3 = 7; // IN3 pin
const int p_pump_in4 = 8; // IN4 pin
String sv_mode = "None"; // Fixed: changed int to String to match usage
float envPH = 7.5;
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  delay(500);
  bt.begin(9600);
  delay(500);
  
  pinMode(p_pump, OUTPUT);
  pinMode(p_pump_in3, OUTPUT);
  pinMode(p_pump_in4, OUTPUT);
  
  // Set the direction once (it never needs to change for a pump)
  digitalWrite(p_pump_in3, HIGH);
  digitalWrite(p_pump_in4, LOW);

  sv.attach(p_sv);
  
  Serial.println("System Ready. Pins Initialized.");
}

void loop() {
  eepromLoop();
  btSerial();
  phLoop();
  servoMode();
}