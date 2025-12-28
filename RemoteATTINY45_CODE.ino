/*
  Project: ATtiny45 - Custom Projector Remote Control
  Author: [Your Name]
  Hardware: ATtiny45 (8MHz Internal Clock)
  
  PIN MAPPING (Physical Pins):
  - Pin 5 (PB0): IR LED Signal Output (Driven via Transistor)
  - Pin 7 (PB2): POWER Button
  - Pin 6 (PB1): INPUT Button
  - Pin 2 (PB3): DOWN Button
  - Pin 3 (PB4): UP Button
*/

#include <Arduino.h>

// --- PIN DEFINITIONS (ATtiny Port Names) ---
const int IR_PIN    = 0; // PB0 (Physical Pin 5)
const int BTN_POWER = 2; // PB2 (Physical Pin 7)
const int BTN_INPUT = 1; // PB1 (Physical Pin 6)
const int BTN_DOWN  = 3; // PB3 (Physical Pin 2)
const int BTN_UP    = 4; // PB4 (Physical Pin 3)

void setup() {
  // Output Configuration
  pinMode(IR_PIN, OUTPUT);
  
  // Input Configuration (Using Internal Pull-ups)
  // [MY NOTE]: Using INPUT_PULLUP eliminates the need for external resistors on the PCB, 
  // saving physical space and reducing component count.
  pinMode(BTN_POWER, INPUT_PULLUP);
  pinMode(BTN_INPUT, INPUT_PULLUP);
  pinMode(BTN_DOWN,  INPUT_PULLUP);
  pinMode(BTN_UP,    INPUT_PULLUP);
}

void loop() {
  // --- BUTTON POLLING ROUTINE ---
  // The logic is inverted (LOW) because of Pull-up resistors.
  
  // 1. POWER (On/Off) -> Command 0x0B
  if (digitalRead(BTN_POWER) == LOW) handleButton(0x0B);

  // 2. INPUT (Source/Menu) -> Command 0x0A
  else if (digitalRead(BTN_INPUT) == LOW) handleButton(0x0A);

  // 3. DOWN -> Command 0x4B
  else if (digitalRead(BTN_DOWN) == LOW) handleButton(0x4B);

  // 4. UP -> Command 0x4A
  else if (digitalRead(BTN_UP) == LOW) handleButton(0x4A);
}

// --- COMMAND HANDLER ---
void handleButton(uint8_t cmd) {
  delay(50); // Simple Debouncing
  
  // [MY NOTE]: Sending the signal 3 times ensures the projector receives it 
  // correctly even if there is IR noise or if the device is in deep sleep.
  for (int i = 0; i < 3; i++) {
    sendCasioPacket(0x84, 0xF4, cmd);
    delay(40); 
  }
  
  // Wait until the button is released to prevent accidental multiple triggers
  while(digitalRead(BTN_POWER) == LOW || digitalRead(BTN_INPUT) == LOW || 
        digitalRead(BTN_DOWN)  == LOW || digitalRead(BTN_UP)    == LOW);
        
  delay(200); // Post-release delay
}

// --- PROTOCOL TRANSMISSION ---
// Structure: [Header] + [Address1] + [Address2] + [Command] + [Inverse Command] + [Stop]
void sendCasioPacket(uint8_t a1, uint8_t a2, uint8_t cmd) {
  mark(9000); space(4500); // Leader Code (AGC Calibration)

  sendByte(a1);       // Custom Address Byte 1
  sendByte(a2);       // Custom Address Byte 2
  sendByte(cmd);      // Command Byte
  sendByte(~cmd);     // Inverse Command (Error Checking)

  mark(560); space(0); // Stop Bit
}

// Data Serialization (LSB First usually, but implementation dependent)
void sendByte(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    // Check bits: Logic 1 = Mark + Long Space, Logic 0 = Mark + Short Space
    if ((data >> i) & 1) { mark(560); space(1690); } 
    else { mark(560); space(560); }
  }
}

// --- CARRIER FREQUENCY GENERATION (38kHz) ---
// [MY NOTE]: This function manually generates the 38kHz carrier wave.
// Period T = 1/38000 ≈ 26 microseconds.
// We need ~13us HIGH and ~13us LOW.
// Using delayMicroseconds(10) accounts for the CPU instruction overhead 
// at 8MHz to achieve the target timing.
void mark(int time) {
  long start = micros();
  while (micros() - start < time) {
    digitalWrite(IR_PIN, HIGH);
    delayMicroseconds(10); 
    digitalWrite(IR_PIN, LOW);
    delayMicroseconds(10);
  }
}

void space(int time) {
  digitalWrite(IR_PIN, LOW);
  delayMicroseconds(time);
}