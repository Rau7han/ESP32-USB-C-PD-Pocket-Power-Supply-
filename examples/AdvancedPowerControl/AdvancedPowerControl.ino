/*
 * Advanced Power Control Example
 * 
 * This example demonstrates advanced features:
 * - Interactive voltage selection via serial menu
 * - PDO discovery and display
 * - Real-time voltage/current monitoring
 * - Safety features and error handling
 * 
 * Hardware Connections:
 * - ESP32 SDA (GPIO21) -> AP33772 SDA
 * - ESP32 SCL (GPIO22) -> AP33772 SCL
 * - ESP32 GND -> AP33772 GND
 * - ESP32 3.3V -> AP33772 VCC (if needed)
 * 
 * Required: USB-C PD power adapter connected to AP33772
 * 
 * Serial Commands:
 * - '1' or '5': Set voltage to 5V
 * - '2' or '9': Set voltage to 9V
 * - '3' or '12': Set voltage to 12V
 * - '4' or '15': Set voltage to 15V
 * - '5' or '20': Set voltage to 20V
 * - 's': Display status
 * - 'p': List available PDOs
 * - 'h': Display help menu
 */

#include <AP33772.h>

// Create AP33772 instance
AP33772 pdSink;

// Current state
uint16_t currentVoltage = VOLTAGE_5V;
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 1000; // Update every second

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   AP33772 Advanced Power Control      ║");
  Serial.println("║   USB-C PD Programmable Power Supply   ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Initialize I2C and AP33772
  Wire.begin();
  
  Serial.print("\nInitializing AP33772...");
  if (!pdSink.begin()) {
    Serial.println(" FAILED!");
    Serial.println("\n⚠️  ERROR: Cannot communicate with AP33772");
    Serial.println("   Check:");
    Serial.println("   - I2C connections (SDA/SCL)");
    Serial.println("   - Power supply");
    Serial.println("   - I2C address (0x51)");
    while (1) {
      delay(1000);
    }
  }
  Serial.println(" OK!");
  
  delay(500);
  
  // Check PD status
  if (pdSink.isPDActive()) {
    Serial.println("✓ Power Delivery active");
  } else {
    Serial.println("⚠️  Warning: PD not active - check USB-C power source");
  }
  
  // Display available PDOs
  displayAvailablePDOs();
  
  // Display help menu
  displayHelp();
  
  Serial.println("\nReady for commands!");
}

void loop() {
  // Handle serial commands
  if (Serial.available()) {
    handleSerialCommand();
  }
  
  // Periodic status update
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();
    displayStatus();
  }
}

void handleSerialCommand() {
  String command = Serial.readStringUntil('\n');
  command.trim();
  command.toLowerCase();
  
  if (command == "1" || command == "5" || command == "5v") {
    setVoltage(VOLTAGE_5V);
  } else if (command == "2" || command == "9" || command == "9v") {
    setVoltage(VOLTAGE_9V);
  } else if (command == "3" || command == "12" || command == "12v") {
    setVoltage(VOLTAGE_12V);
  } else if (command == "4" || command == "15" || command == "15v") {
    setVoltage(VOLTAGE_15V);
  } else if (command == "5" || command == "20" || command == "20v") {
    setVoltage(VOLTAGE_20V);
  } else if (command == "s" || command == "status") {
    Serial.println("\n=== Detailed Status ===");
    pdSink.printDebugInfo();
  } else if (command == "p" || command == "pdo") {
    displayAvailablePDOs();
  } else if (command == "h" || command == "help") {
    displayHelp();
  } else if (command.length() > 0) {
    Serial.println("Unknown command. Type 'h' for help.");
  }
}

void setVoltage(uint16_t voltage_mv) {
  Serial.print("\nRequesting ");
  Serial.print(voltage_mv / 1000.0, 1);
  Serial.print("V...");
  
  if (pdSink.setVoltage(voltage_mv)) {
    currentVoltage = voltage_mv;
    Serial.println(" SUCCESS!");
    delay(500);
    displayStatus();
  } else {
    Serial.println(" FAILED!");
    Serial.println("⚠️  Voltage not available or negotiation failed");
  }
}

void displayStatus() {
  uint16_t voltage = pdSink.readVoltage();
  uint16_t current = pdSink.readCurrent();
  float power = (voltage / 1000.0) * (current / 1000.0);
  
  Serial.println("\n┌─────────────────────────────┐");
  Serial.print("│ Voltage: ");
  Serial.print(voltage / 1000.0, 2);
  Serial.print(" V");
  Serial.print("          │");
  Serial.println();
  
  Serial.print("│ Current: ");
  Serial.print(current / 1000.0, 3);
  Serial.print(" A");
  Serial.print("         │");
  Serial.println();
  
  Serial.print("│ Power:   ");
  Serial.print(power, 2);
  Serial.print(" W");
  Serial.print("          │");
  Serial.println();
  
  Serial.print("│ PD:      ");
  Serial.print(pdSink.isPDActive() ? "Active" : "Inactive");
  Serial.print("        │");
  Serial.println();
  
  Serial.println("└─────────────────────────────┘");
}

void displayAvailablePDOs() {
  Serial.println("\n=== Available Power Delivery Objects ===");
  
  uint8_t numPDOs = pdSink.getAvailablePDOs();
  Serial.print("Total PDOs: ");
  Serial.println(numPDOs);
  
  for (uint8_t i = 0; i < numPDOs && i < 7; i++) {
    uint16_t voltage, current;
    if (pdSink.getPDOInfo(i, &voltage, &current)) {
      Serial.print("PDO ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(voltage / 1000.0, 1);
      Serial.print("V @ ");
      Serial.print(current / 1000.0, 2);
      Serial.print("A (");
      Serial.print((voltage / 1000.0) * (current / 1000.0), 1);
      Serial.println("W)");
    }
  }
  Serial.println("=====================================");
}

void displayHelp() {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║           Command Menu                 ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.println("║ Voltage Selection:                     ║");
  Serial.println("║   1 or 5  - Set 5V                     ║");
  Serial.println("║   2 or 9  - Set 9V                     ║");
  Serial.println("║   3 or 12 - Set 12V                    ║");
  Serial.println("║   4 or 15 - Set 15V                    ║");
  Serial.println("║   5 or 20 - Set 20V                    ║");
  Serial.println("║                                        ║");
  Serial.println("║ Information:                           ║");
  Serial.println("║   s - Display detailed status          ║");
  Serial.println("║   p - List available PDOs              ║");
  Serial.println("║   h - Show this help menu              ║");
  Serial.println("╚════════════════════════════════════════╝");
}
