# ESP32 USB-C PD Programmable Power Supply

An ESP32-based programmable power supply using the AP33772 USB-C Power Delivery sink controller. This project enables dynamic voltage selection from 5V to 20V through USB-C PD negotiation.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)
![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)

## Features

✅ **USB-C Power Delivery Support**
- Automatic voltage negotiation (5V, 9V, 12V, 15V, 20V)
- PDO (Power Delivery Object) discovery and selection
- Compatible with USB PD 2.0 and 3.0 adapters

✅ **Real-time Monitoring**
- Live voltage and current readings
- Power calculation and display
- PD negotiation status monitoring

✅ **Interactive Control**
- Serial command interface
- Easy voltage selection
- Status reporting and debugging

✅ **Safety Features**
- Configurable voltage/current/power limits
- Automatic safety monitoring
- Error detection and reporting
- Safe 5V default on startup

✅ **Professional Implementation**
- Well-documented Arduino library
- Multiple example sketches
- Complete firmware application
- Comprehensive API

## Hardware Requirements

### Components

1. **ESP32 Development Board**
   - Any ESP32 board with I2C support
   - Recommended: ESP32 DevKit V1 or similar

2. **AP33772 USB-C PD Sink Module**
   - I2C interface (address: 0x51)
   - USB Type-C connector
   - Available from various suppliers

3. **USB-C PD Power Adapter**
   - Must support USB Power Delivery
   - Minimum 18W recommended
   - 45W-100W adapters provide more voltage options

4. **Connection Materials**
   - Jumper wires or PCB
   - Optional: Terminal blocks for output connections
   - Optional: Case/enclosure

### Wiring Diagram

```
ESP32                     AP33772 Module
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GPIO21 (SDA) ────────────► SDA
GPIO22 (SCL) ────────────► SCL
GND ─────────────────────► GND
3.3V ────────────────────► VCC (if needed)

USB-C PD Adapter ───────► AP33772 USB-C Input
AP33772 Output ──────────► Your Load
```

### Pin Connections

| ESP32 Pin | Function | AP33772 Pin |
|-----------|----------|-------------|
| GPIO21    | SDA      | SDA         |
| GPIO22    | SCL      | SCL         |
| GND       | Ground   | GND         |
| 3.3V      | Logic    | VCC         |

**Note**: Some AP33772 modules have built-in voltage regulation and may not need the 3.3V connection. Check your module's documentation.

## Software Setup

### Arduino IDE Installation

1. **Install ESP32 Board Support**
   ```
   - Open Arduino IDE
   - Go to File → Preferences
   - Add to "Additional Board Manager URLs":
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   - Go to Tools → Board → Boards Manager
   - Search for "ESP32" and install
   ```

2. **Install the AP33772 Library**
   - Copy the `library/AP33772` folder to your Arduino libraries directory:
     - Windows: `Documents\Arduino\libraries\`
     - Mac: `~/Documents/Arduino/libraries/`
     - Linux: `~/Arduino/libraries/`
   - Restart Arduino IDE

3. **Select Board and Port**
   ```
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module
   - Tools → Port → (Select your ESP32's COM port)
   ```

### PlatformIO Installation (Alternative)

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    Wire
```

## Usage

### Quick Start

1. **Wire the hardware** according to the wiring diagram above
2. **Upload the basic example**:
   - Open `examples/BasicPowerDelivery/BasicPowerDelivery.ino`
   - Upload to your ESP32
3. **Open Serial Monitor** at 115200 baud
4. **Observe** voltage negotiation and readings

### Available Examples

#### 1. BasicPowerDelivery
Simple example demonstrating:
- AP33772 initialization
- 12V voltage negotiation
- Reading voltage and current
- Basic status display

**Use case**: Learning the library basics, testing hardware

#### 2. AdvancedPowerControl
Interactive control interface featuring:
- Serial command menu
- Multiple voltage selection
- PDO discovery and display
- Real-time monitoring

**Use case**: Interactive power supply control, testing different voltages

#### 3. ProgrammablePowerSupply (Firmware)
Complete power supply application with:
- Professional UI
- Safety monitoring
- Comprehensive error handling
- Production-ready features

**Use case**: Final application, embedded in your project

### Serial Commands

When using the advanced examples or main firmware:

| Command | Action |
|---------|--------|
| `1` or `V5` | Set voltage to 5V |
| `2` or `V9` | Set voltage to 9V |
| `3` or `V12` | Set voltage to 12V |
| `4` or `V15` | Set voltage to 15V |
| `5` or `V20` | Set voltage to 20V |
| `S` | Show detailed status |
| `P` | List available PDOs |
| `M` | Display menu |
| `R` | Reset to 5V safe mode |
| `H` | Display help |

## Library API Reference

### Initialization

```cpp
#include <AP33772.h>

AP33772 pdSink;  // Create instance

void setup() {
  Wire.begin();           // Initialize I2C
  pdSink.begin(Wire);     // Initialize AP33772
}
```

### Main Functions

```cpp
// Set output voltage
bool setVoltage(uint16_t voltage_mv);
// Example: pdSink.setVoltage(VOLTAGE_12V);

// Read current voltage
uint16_t readVoltage();
// Returns: voltage in millivolts

// Read current draw
uint16_t readCurrent();
// Returns: current in milliamps

// Check PD status
bool isPDActive();
// Returns: true if PD negotiation successful

// Get available PDO count
uint8_t getAvailablePDOs();

// Get PDO information
bool getPDOInfo(uint8_t pdo_index, uint16_t *voltage, uint16_t *current);

// Print debug information
void printDebugInfo();
```

### Voltage Constants

```cpp
VOLTAGE_5V    // 5000 mV
VOLTAGE_9V    // 9000 mV
VOLTAGE_12V   // 12000 mV
VOLTAGE_15V   // 15000 mV
VOLTAGE_20V   // 20000 mV
```

## Safety Information

### ⚠️ Important Safety Guidelines

**ALWAYS follow these safety precautions:**

1. **Verify Before Connecting**
   - Always check voltage with a multimeter before connecting equipment
   - Start at 5V and increase gradually
   - Disconnect load before changing voltage

2. **Current Limiting**
   - Know your load's current requirements
   - Use appropriate wire gauge for expected current
   - Consider adding external current limiting

3. **Temperature Monitoring**
   - Monitor temperature during high power operation
   - Ensure adequate cooling for sustained high power
   - AP33772 module may get warm at high currents

4. **Component Ratings**
   - Never exceed your components' voltage ratings
   - Check power adapter specifications
   - Verify AP33772 module current handling

5. **Electrical Safety**
   - Use insulated enclosure for safety
   - Avoid shorts between output terminals
   - Include fuse protection if needed
   - Follow proper electrical safety practices

6. **Wiring**
   - Use appropriate wire gauge for current levels:
     - 1-2A: 22-24 AWG minimum
     - 2-3A: 20 AWG minimum
     - 3-5A: 18 AWG minimum
   - Ensure solid connections
   - Check for loose wires regularly

### Power Limits

Typical limits (adjust based on your hardware):
- Maximum Voltage: 20V (USB PD limit)
- Maximum Current: 3-5A (depends on adapter and AP33772 module)
- Maximum Power: 60-100W (depends on adapter)

**Note**: Always check your specific AP33772 module's specifications.

## Troubleshooting

### Problem: AP33772 initialization fails

**Solutions:**
- Check I2C wiring (SDA to GPIO21, SCL to GPIO22)
- Verify common ground connection
- Ensure AP33772 module is powered
- Try scanning I2C bus: use I2C scanner sketch
- Check I2C pull-up resistors (usually built into module)

### Problem: PD negotiation not active

**Solutions:**
- Ensure USB-C PD adapter is connected (not regular USB)
- Try a different USB-C cable (must be USB-C to USB-C)
- Verify adapter supports USB Power Delivery
- Check if adapter is in low-power mode

### Problem: Voltage not changing

**Solutions:**
- Requested voltage may not be supported by adapter
- Check adapter specifications for supported voltages
- Verify PD negotiation is active (`isPDActive()`)
- Try requesting a different voltage

### Problem: Incorrect voltage/current readings

**Solutions:**
- Verify register scaling factors in library
- Check against AP33772 datasheet for your module
- Calibrate against known voltage source
- Some modules may use different ADC scaling

### Problem: I2C communication errors

**Solutions:**
- Check wire length (keep I2C wires short)
- Lower I2C clock speed if needed: `Wire.setClock(100000);`
- Verify pull-up resistors on SDA/SCL
- Check for electrical noise interference

## Project Structure

```
ESP32-USB-C-PD-Pocket-Power-Supply-/
│
├── library/
│   └── AP33772/              # Arduino library for AP33772
│       ├── AP33772.h         # Header file
│       ├── AP33772.cpp       # Implementation
│       ├── keywords.txt      # Arduino IDE syntax highlighting
│       └── library.properties # Library metadata
│
├── examples/
│   ├── BasicPowerDelivery/   # Simple usage example
│   │   └── BasicPowerDelivery.ino
│   │
│   └── AdvancedPowerControl/ # Interactive control example
│       └── AdvancedPowerControl.ino
│
├── firmware/
│   └── ProgrammablePowerSupply/  # Complete PSU firmware
│       └── ProgrammablePowerSupply.ino
│
├── datasheet/
│   └── README.md             # AP33772 reference information
│
├── README.md                 # This file
└── LICENSE                   # MIT License
```

## Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

### Areas for Contribution
- Hardware designs and schematics
- Additional examples
- Library improvements
- Documentation enhancements
- Testing with different AP33772 modules

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- ESP32 Arduino Core team
- AP33772 by Diodes Incorporated
- USB Power Delivery specification contributors

## Disclaimer

This project is provided "as-is" without warranty. Users are responsible for:
- Proper hardware assembly
- Electrical safety
- Compliance with local regulations
- Verifying compatibility with their specific components

Always test thoroughly and follow safety guidelines when working with power electronics.

## Support

For issues, questions, or suggestions:
- Open an issue on GitHub
- Check existing issues for solutions
- Review the troubleshooting section above

## Version History

- **v1.0.0** - Initial release
  - AP33772 library implementation
  - Basic and advanced examples
  - Complete firmware application
  - Comprehensive documentation

---

**Made with ❤️ for the maker community**
