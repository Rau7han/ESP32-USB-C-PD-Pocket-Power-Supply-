/*
 * ESP32 USB-C PD Programmable Power Supply
 * 
 * A complete programmable power supply firmware featuring:
 * - USB-C Power Delivery voltage negotiation (5V-20V)
 * - Real-time voltage and current monitoring
 * - Interactive serial control interface
 * - Safety features and error handling
 * - Automatic PDO discovery
 * - Power calculation and display
 * 
 * Hardware Requirements:
 * - ESP32 Development Board
 * - AP33772 USB-C PD Sink Module
 * - USB-C PD Power Adapter (supporting multiple voltages)
 * 
 * Connections:
 * - ESP32 GPIO21 (SDA) -> AP33772 SDA
 * - ESP32 GPIO22 (SCL) -> AP33772 SCL
 * - ESP32 GND -> AP33772 GND
 * - ESP32 3.3V -> AP33772 VCC (if required by module)
 * - USB-C PD Adapter -> AP33772 USB-C Input
 * 
 * Author: ESP32 USB-C PD Project
 * License: MIT
 * 
 * ⚠️  SAFETY WARNING:
 * - Always verify voltage before connecting sensitive equipment
 * - Use appropriate current limiting for your application
 * - Monitor temperature during high power operation
 * - Never exceed component ratings
 */

#include <AP33772.h>

// Create AP33772 instance
AP33772 pdSink;

// Configuration
#define SERIAL_BAUD_RATE 115200
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define STATUS_UPDATE_INTERVAL 2000  // ms
#define SAFETY_CHECK_INTERVAL 500    // ms

// State management
struct PowerSupplyState {
  uint16_t targetVoltage;
  uint16_t currentVoltage;
  uint16_t currentCurrent;
  bool pdActive;
  bool safetyAlert;
  unsigned long lastUpdate;
  unsigned long lastSafetyCheck;
};

PowerSupplyState psState = {
  VOLTAGE_5V,  // Default to 5V
  0,
  0,
  false,
  false,
  0,
  0
};

// Safety limits (adjust based on your requirements)
#define MAX_CURRENT_MA 3000  // 3A max
#define MAX_POWER_MW 60000   // 60W max
#define MAX_VOLTAGE_MV 20000 // 20V max

void setup() {
  // Initialize Serial
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  
  // Display startup banner
  displayBanner();
  
  // Initialize I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Serial.println("I2C initialized");
  
  // Initialize AP33772
  Serial.print("Initializing AP33772 PD controller...");
  if (!pdSink.begin(Wire)) {
    Serial.println(" FAILED!");
    displayError("AP33772 initialization failed");
    displayTroubleshootingHelp();
    while (1) {
      delay(1000);
    }
  }
  Serial.println(" OK!");
  
  // Check PD status
  delay(500);
  psState.pdActive = pdSink.isPDActive();
  
  if (psState.pdActive) {
    Serial.println("✓ Power Delivery negotiation successful");
  } else {
    Serial.println("⚠️  Warning: PD negotiation not complete");
    Serial.println("   Ensure USB-C PD adapter is connected");
  }
  
  // Discover and display available power profiles
  discoverPowerProfiles();
  
  // Set initial voltage (5V for safety)
  Serial.println("\nSetting initial voltage to 5V (safe default)...");
  setVoltageWithSafety(VOLTAGE_5V);
  
  // Display control menu
  displayMenu();
  
  Serial.println("\n✓ Power Supply Ready!");
  Serial.println("Enter command (type 'h' for help):");
}

void loop() {
  // Handle serial commands
  if (Serial.available()) {
    handleCommand();
  }
  
  // Periodic status update
  if (millis() - psState.lastUpdate >= STATUS_UPDATE_INTERVAL) {
    psState.lastUpdate = millis();
    updateReadings();
    displayStatusCompact();
  }
  
  // Safety monitoring
  if (millis() - psState.lastSafetyCheck >= SAFETY_CHECK_INTERVAL) {
    psState.lastSafetyCheck = millis();
    performSafetyCheck();
  }
}

void displayBanner() {
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════════════╗");
  Serial.println("║  ESP32 USB-C PD Programmable Power Supply      ║");
  Serial.println("║  Version 1.0                                   ║");
  Serial.println("║  Using AP33772 PD Sink Controller             ║");
  Serial.println("╚════════════════════════════════════════════════╝");
  Serial.println();
}

void displayMenu() {
  Serial.println("\n╔════════════════════════════════════════════════╗");
  Serial.println("║              Control Menu                      ║");
  Serial.println("╠════════════════════════════════════════════════╣");
  Serial.println("║ Voltage Control:                               ║");
  Serial.println("║   1 or V5  - Set output to 5V                  ║");
  Serial.println("║   2 or V9  - Set output to 9V                  ║");
  Serial.println("║   3 or V12 - Set output to 12V                 ║");
  Serial.println("║   4 or V15 - Set output to 15V                 ║");
  Serial.println("║   5 or V20 - Set output to 20V                 ║");
  Serial.println("║                                                ║");
  Serial.println("║ Information & Control:                         ║");
  Serial.println("║   S - Show detailed status                     ║");
  Serial.println("║   P - List available power profiles (PDOs)     ║");
  Serial.println("║   M - Show this menu                           ║");
  Serial.println("║   R - Reset to 5V (safe mode)                  ║");
  Serial.println("║   D - Toggle debug output                      ║");
  Serial.println("║   H - Display help and safety information      ║");
  Serial.println("╚════════════════════════════════════════════════╝");
}

void handleCommand() {
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toUpperCase();
  
  Serial.print("\n> Command: ");
  Serial.println(cmd);
  
  if (cmd == "1" || cmd == "V5" || cmd == "5V") {
    setVoltageWithSafety(VOLTAGE_5V);
  } else if (cmd == "2" || cmd == "V9" || cmd == "9V") {
    setVoltageWithSafety(VOLTAGE_9V);
  } else if (cmd == "3" || cmd == "V12" || cmd == "12V") {
    setVoltageWithSafety(VOLTAGE_12V);
  } else if (cmd == "4" || cmd == "V15" || cmd == "15V") {
    setVoltageWithSafety(VOLTAGE_15V);
  } else if (cmd == "5" || cmd == "V20" || cmd == "20V") {
    setVoltageWithSafety(VOLTAGE_20V);
  } else if (cmd == "S" || cmd == "STATUS") {
    displayDetailedStatus();
  } else if (cmd == "P" || cmd == "PDO" || cmd == "PROFILES") {
    discoverPowerProfiles();
  } else if (cmd == "M" || cmd == "MENU") {
    displayMenu();
  } else if (cmd == "R" || cmd == "RESET") {
    Serial.println("Resetting to safe mode (5V)...");
    setVoltageWithSafety(VOLTAGE_5V);
  } else if (cmd == "D" || cmd == "DEBUG") {
    pdSink.printDebugInfo();
  } else if (cmd == "H" || cmd == "HELP") {
    displayHelp();
  } else if (cmd.length() > 0) {
    Serial.println("❌ Unknown command. Type 'M' for menu or 'H' for help.");
  }
}

void setVoltageWithSafety(uint16_t voltage_mv) {
  // Safety check
  if (voltage_mv > MAX_VOLTAGE_MV) {
    Serial.println("❌ ERROR: Requested voltage exceeds safety limit!");
    return;
  }
  
  Serial.print("Setting voltage to ");
  Serial.print(voltage_mv / 1000.0, 1);
  Serial.print("V...");
  
  if (pdSink.setVoltage(voltage_mv)) {
    psState.targetVoltage = voltage_mv;
    Serial.println(" ✓ SUCCESS");
    delay(500);
    updateReadings();
    displayStatusCompact();
  } else {
    Serial.println(" ❌ FAILED");
    Serial.println("Possible reasons:");
    Serial.println("  - Requested voltage not supported by adapter");
    Serial.println("  - Communication error with AP33772");
    Serial.println("  - PD negotiation failed");
  }
}

void updateReadings() {
  psState.currentVoltage = pdSink.readVoltage();
  psState.currentCurrent = pdSink.readCurrent();
  psState.pdActive = pdSink.isPDActive();
}

void displayStatusCompact() {
  float voltage = psState.currentVoltage / 1000.0;
  float current = psState.currentCurrent / 1000.0;
  float power = voltage * current;
  
  Serial.println("\n┌──────────────────────────────────────┐");
  Serial.print("│ Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V");
  for (int i = 0; i < (28 - String(voltage, 2).length()); i++) Serial.print(" ");
  Serial.println("│");
  
  Serial.print("│ Current: ");
  Serial.print(current, 3);
  Serial.print(" A");
  for (int i = 0; i < (28 - String(current, 3).length()); i++) Serial.print(" ");
  Serial.println("│");
  
  Serial.print("│ Power:   ");
  Serial.print(power, 2);
  Serial.print(" W");
  for (int i = 0; i < (28 - String(power, 2).length()); i++) Serial.print(" ");
  Serial.println("│");
  
  Serial.print("│ Status:  ");
  Serial.print(psState.pdActive ? "Active   " : "Inactive ");
  Serial.print(psState.safetyAlert ? "⚠️  ALERT" : "✓ OK    ");
  Serial.println("    │");
  Serial.println("└──────────────────────────────────────┘");
}

void displayDetailedStatus() {
  Serial.println("\n╔════════════════════════════════════════════════╗");
  Serial.println("║           Detailed System Status               ║");
  Serial.println("╚════════════════════════════════════════════════╝");
  
  updateReadings();
  
  Serial.print("Target Voltage:  ");
  Serial.print(psState.targetVoltage / 1000.0, 1);
  Serial.println(" V");
  
  Serial.print("Current Voltage: ");
  Serial.print(psState.currentVoltage / 1000.0, 2);
  Serial.println(" V");
  
  Serial.print("Current Draw:    ");
  Serial.print(psState.currentCurrent / 1000.0, 3);
  Serial.println(" A");
  
  float power = (psState.currentVoltage / 1000.0) * (psState.currentCurrent / 1000.0);
  Serial.print("Power Output:    ");
  Serial.print(power, 2);
  Serial.println(" W");
  
  Serial.print("PD Status:       ");
  Serial.println(psState.pdActive ? "✓ Active" : "❌ Inactive");
  
  Serial.print("Safety Status:   ");
  Serial.println(psState.safetyAlert ? "⚠️  ALERT" : "✓ OK");
  
  Serial.println("\n--- AP33772 Chip Status ---");
  pdSink.printDebugInfo();
}

void discoverPowerProfiles() {
  Serial.println("\n╔════════════════════════════════════════════════╗");
  Serial.println("║        Available Power Profiles (PDOs)         ║");
  Serial.println("╚════════════════════════════════════════════════╝");
  
  uint8_t numPDOs = pdSink.getAvailablePDOs();
  Serial.print("Total profiles available: ");
  Serial.println(numPDOs);
  Serial.println();
  
  if (numPDOs == 0) {
    Serial.println("⚠️  No PDOs detected. Check PD adapter connection.");
    return;
  }
  
  for (uint8_t i = 0; i < numPDOs && i < 7; i++) {
    uint16_t voltage, current;
    if (pdSink.getPDOInfo(i, &voltage, &current)) {
      float v = voltage / 1000.0;
      float c = current / 1000.0;
      float p = v * c;
      
      Serial.print("Profile ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(v, 1);
      Serial.print("V @ ");
      Serial.print(c, 2);
      Serial.print("A (Max ");
      Serial.print(p, 1);
      Serial.println("W)");
    }
  }
  Serial.println("════════════════════════════════════════════════");
}

void performSafetyCheck() {
  updateReadings();
  
  uint32_t power_mw = (uint32_t)psState.currentVoltage * psState.currentCurrent / 1000;
  
  bool wasAlert = psState.safetyAlert;
  psState.safetyAlert = false;
  
  // Check current limit
  if (psState.currentCurrent > MAX_CURRENT_MA) {
    psState.safetyAlert = true;
    if (!wasAlert) {
      Serial.println("\n⚠️  SAFETY ALERT: Current exceeds limit!");
      Serial.print("Current: ");
      Serial.print(psState.currentCurrent / 1000.0, 3);
      Serial.print("A (Limit: ");
      Serial.print(MAX_CURRENT_MA / 1000.0, 1);
      Serial.println("A)");
    }
  }
  
  // Check power limit
  if (power_mw > MAX_POWER_MW) {
    psState.safetyAlert = true;
    if (!wasAlert) {
      Serial.println("\n⚠️  SAFETY ALERT: Power exceeds limit!");
      Serial.print("Power: ");
      Serial.print(power_mw / 1000.0, 2);
      Serial.print("W (Limit: ");
      Serial.print(MAX_POWER_MW / 1000.0, 1);
      Serial.println("W)");
    }
  }
  
  // Check PD status
  if (!psState.pdActive) {
    psState.safetyAlert = true;
    if (!wasAlert) {
      Serial.println("\n⚠️  WARNING: PD not active!");
    }
  }
}

void displayHelp() {
  Serial.println("\n╔════════════════════════════════════════════════╗");
  Serial.println("║              Help & Safety Info                ║");
  Serial.println("╚════════════════════════════════════════════════╝");
  Serial.println("\n📖 About:");
  Serial.println("This is a programmable USB-C PD power supply based on");
  Serial.println("ESP32 and AP33772 PD sink controller. It negotiates");
  Serial.println("voltage with USB-C PD adapters (5V-20V).");
  
  Serial.println("\n⚙️  Usage:");
  Serial.println("1. Connect USB-C PD adapter to AP33772 module");
  Serial.println("2. Power supply will start at safe 5V default");
  Serial.println("3. Use serial commands to select desired voltage");
  Serial.println("4. Monitor voltage/current readings continuously");
  
  Serial.println("\n⚠️  SAFETY WARNINGS:");
  Serial.println("• Always verify voltage before connecting equipment");
  Serial.println("• Use current limiting for sensitive devices");
  Serial.println("• Monitor temperature during high power operation");
  Serial.println("• Never exceed component voltage/current ratings");
  Serial.println("• Disconnect load before changing voltage");
  Serial.println("• Use proper gauge wires for high current");
  
  Serial.println("\n🔧 Troubleshooting:");
  Serial.println("• No PD active: Check USB-C cable and adapter");
  Serial.println("• Voltage not changing: Adapter may not support it");
  Serial.println("• Comm error: Check I2C wiring (SDA/SCL)");
  Serial.println("• Low current: Check power adapter rating");
  
  Serial.println("\n📊 Current Safety Limits:");
  Serial.print("• Max Voltage: ");
  Serial.print(MAX_VOLTAGE_MV / 1000.0, 0);
  Serial.println("V");
  Serial.print("• Max Current: ");
  Serial.print(MAX_CURRENT_MA / 1000.0, 1);
  Serial.println("A");
  Serial.print("• Max Power:   ");
  Serial.print(MAX_POWER_MW / 1000.0, 0);
  Serial.println("W");
  
  Serial.println("\n════════════════════════════════════════════════");
}

void displayError(const char* message) {
  Serial.println("\n╔════════════════════════════════════════════════╗");
  Serial.println("║                    ERROR                       ║");
  Serial.println("╚════════════════════════════════════════════════╝");
  Serial.println(message);
  Serial.println("════════════════════════════════════════════════");
}

void displayTroubleshootingHelp() {
  Serial.println("\n🔧 Troubleshooting Steps:");
  Serial.println("1. Check I2C connections:");
  Serial.print("   - SDA connected to GPIO");
  Serial.println(I2C_SDA_PIN);
  Serial.print("   - SCL connected to GPIO");
  Serial.println(I2C_SCL_PIN);
  Serial.println("   - Common ground connection");
  Serial.println("2. Verify AP33772 power supply");
  Serial.println("3. Check I2C address (default: 0x51)");
  Serial.println("4. Ensure proper I2C pull-up resistors");
  Serial.println("5. Try different I2C pins if needed");
}
