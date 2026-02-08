/*
 * AP33772 USB-C PD Sink Controller Library for Arduino
 * 
 * This library provides an interface to control the AP33772 USB-C PD sink
 * controller via I2C communication for voltage negotiation and monitoring.
 * 
 * Author: ESP32 USB-C PD Project
 * License: MIT
 */

#ifndef AP33772_H
#define AP33772_H

#include <Arduino.h>
#include <Wire.h>

// AP33772 I2C Address
#define AP33772_I2C_ADDR 0x51

// AP33772 Register Addresses
#define AP33772_REG_VOLTAGE       0x20  // Voltage register (16-bit)
#define AP33772_REG_CURRENT       0x21  // Current register (16-bit)
#define AP33772_REG_VOLTAGE_SET   0x00  // Voltage setting register
#define AP33772_REG_STATUS        0x02  // Status register
#define AP33772_REG_PDO_NUM       0x03  // PDO number register
#define AP33772_REG_PDO_INFO      0x04  // PDO information register

// Voltage options (in mV)
#define VOLTAGE_5V    5000
#define VOLTAGE_9V    9000
#define VOLTAGE_12V   12000
#define VOLTAGE_15V   15000
#define VOLTAGE_20V   20000

// Status bits
#define STATUS_PD_SUCCESS    0x01
#define STATUS_VOLTAGE_READY 0x02

class AP33772 {
public:
    /**
     * Constructor
     * @param i2cAddress I2C address of the AP33772 (default: 0x51)
     */
    AP33772(uint8_t i2cAddress = AP33772_I2C_ADDR);
    
    /**
     * Initialize the AP33772 and I2C communication
     * @param wire Reference to TwoWire object (default: Wire)
     * @return true if initialization successful
     */
    bool begin(TwoWire &wire = Wire);
    
    /**
     * Request a specific voltage from the PD source
     * @param voltage_mv Voltage in millivolts (5000, 9000, 12000, 15000, 20000)
     * @return true if voltage negotiation successful
     */
    bool setVoltage(uint16_t voltage_mv);
    
    /**
     * Read the current output voltage
     * @return Voltage in millivolts
     */
    uint16_t readVoltage();
    
    /**
     * Read the current output current
     * @return Current in milliamps
     */
    uint16_t readCurrent();
    
    /**
     * Get the status of PD negotiation
     * @return Status byte
     */
    uint8_t getStatus();
    
    /**
     * Check if PD negotiation was successful
     * @return true if PD is active
     */
    bool isPDActive();
    
    /**
     * Get number of available PDOs from source
     * @return Number of PDOs (0-7)
     */
    uint8_t getAvailablePDOs();
    
    /**
     * Get information about a specific PDO
     * @param pdo_index PDO index (0-6)
     * @param voltage Pointer to store voltage in mV
     * @param current Pointer to store max current in mA
     * @return true if PDO info retrieved successfully
     */
    bool getPDOInfo(uint8_t pdo_index, uint16_t *voltage, uint16_t *current);
    
    /**
     * Print debug information to Serial
     */
    void printDebugInfo();

private:
    uint8_t _i2cAddress;
    TwoWire *_wire;
    
    /**
     * Write a byte to a register
     * @param reg Register address
     * @param value Byte value to write
     * @return true if write successful
     */
    bool writeRegister(uint8_t reg, uint8_t value);
    
    /**
     * Read a byte from a register
     * @param reg Register address
     * @param value Pointer to store read byte
     * @return true if read successful
     */
    bool readRegister(uint8_t reg, uint8_t *value);
    
    /**
     * Read 16-bit value from register
     * @param reg Register address
     * @param value Pointer to store 16-bit value
     * @return true if read successful
     */
    bool readRegister16(uint8_t reg, uint16_t *value);
    
    /**
     * Convert voltage to PDO index
     * @param voltage_mv Voltage in millivolts
     * @return PDO index
     */
    uint8_t voltageToPDO(uint16_t voltage_mv);
};

#endif // AP33772_H
