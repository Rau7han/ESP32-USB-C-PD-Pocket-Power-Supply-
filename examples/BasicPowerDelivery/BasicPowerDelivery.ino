/*
 * Basic Power Delivery Example
 * 
 * This example demonstrates basic usage of the AP33772 library to:
 * - Initialize the AP33772 USB-C PD sink
 * - Request a specific voltage (12V)
 * - Read and display voltage and current
 * 
 * Hardware Connections:
 * - ESP32 SDA (GPIO21) -> AP33772 SDA
 * - ESP32 SCL (GPIO22) -> AP33772 SCL
 * - ESP32 GND -> AP33772 GND
 * - ESP32 3.3V -> AP33772 VCC (if needed)
 * 
 * Required: USB-C PD power adapter connected to AP33772
 */

#include <AP33772.h>

// Create AP33772 instance
AP33772 pdSink;

void setup() {
  // Initialize Serial for debug output
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("AP33772 Basic Power Delivery Example");
  Serial.println("====================================");
  
  // Initialize I2C and AP33772
  Wire.begin();
  
  if (!pdSink.begin()) {
    Serial.println("ERROR: Failed to initialize AP33772!");
    Serial.println("Check I2C connections and power.");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("AP33772 initialized successfully!");
  delay(500);
  
  // Display available PDOs
  Serial.println("\nChecking available Power Delivery Objects...");
  uint8_t numPDOs = pdSink.getAvailablePDOs();
  Serial.print("Available PDOs: ");
  Serial.println(numPDOs);
  
  // Request 12V from PD source
  Serial.println("\nRequesting 12V from PD source...");
  if (pdSink.setVoltage(VOLTAGE_12V)) {
    Serial.println("SUCCESS: 12V negotiated!");
  } else {
    Serial.println("WARNING: Failed to negotiate 12V");
    Serial.println("Power source may not support 12V");
  }
  
  delay(1000);
  
  // Display current status
  pdSink.printDebugInfo();
}

void loop() {
  // Read and display voltage and current every 2 seconds
  uint16_t voltage = pdSink.readVoltage();
  uint16_t current = pdSink.readCurrent();
  
  Serial.println("\n--- Current Status ---");
  Serial.print("Voltage: ");
  Serial.print(voltage / 1000.0, 2);
  Serial.println(" V");
  
  Serial.print("Current: ");
  Serial.print(current / 1000.0, 3);
  Serial.println(" A");
  
  Serial.print("Power: ");
  Serial.print((voltage / 1000.0) * (current / 1000.0), 2);
  Serial.println(" W");
  
  Serial.print("PD Active: ");
  Serial.println(pdSink.isPDActive() ? "Yes" : "No");
  
  delay(2000);
}
