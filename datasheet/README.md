# AP33772 Datasheet Reference

This directory is intended for storing reference documents related to the AP33772 USB-C Power Delivery sink controller.

## Official Documentation

The AP33772 is a USB Type-C Power Delivery (PD) sink controller manufactured by Diodes Incorporated.

### Where to Find Documentation

1. **Official Datasheet**: Available from Diodes Incorporated website
   - Product Page: https://www.diodes.com/
   - Search for "AP33772" in their product catalog

2. **Key Features** (from public information):
   - USB Type-C and USB PD 3.0 compliant
   - Automatic voltage negotiation (5V, 9V, 12V, 15V, 20V)
   - I2C interface for configuration and monitoring
   - Integrated power path management
   - Over-voltage and over-current protection

3. **Important Specifications**:
   - I2C Address: 0x51 (default)
   - Operating Voltage: Typically 3.3V for logic
   - PD Voltage Range: 5V - 20V
   - Maximum Current: Depends on PD source capability

## Register Map Notes

The library implementation uses the following register addresses (verify with official datasheet):

- `0x00`: Voltage Setting Register
- `0x02`: Status Register
- `0x03`: PDO Number Register
- `0x04`: PDO Information Register
- `0x20`: Voltage Reading Register (16-bit)
- `0x21`: Current Reading Register (16-bit)

**Note**: Always refer to the official datasheet for accurate register addresses and bit definitions, as implementations may vary by hardware revision.

## Additional Resources

- Application notes and reference designs may be available from the manufacturer
- Community forums and development boards often have additional implementation examples
- GitHub repositories with working implementations can provide practical insights

## Important Notice

⚠️ **Always refer to the official datasheet** when implementing hardware designs or modifying the library. Register addresses, bit definitions, and electrical specifications must match your specific hardware revision.

## License

Documentation and datasheets are copyrighted by their respective manufacturers. This reference file is provided for educational purposes only.
