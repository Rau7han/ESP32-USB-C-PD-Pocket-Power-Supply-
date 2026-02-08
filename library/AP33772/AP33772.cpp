/*
 * AP33772 USB-C PD Sink Controller Library for Arduino
 * 
 * Implementation file for AP33772 library
 * 
 * Author: ESP32 USB-C PD Project
 * License: MIT
 */

#include "AP33772.h"

AP33772::AP33772(uint8_t i2cAddress) {
    _i2cAddress = i2cAddress;
    _wire = nullptr;
}

bool AP33772::begin(TwoWire &wire) {
    _wire = &wire;
    _wire->begin();
    
    // Check if device is present
    _wire->beginTransmission(_i2cAddress);
    uint8_t error = _wire->endTransmission();
    
    if (error == 0) {
        delay(100); // Give device time to initialize
        return true;
    }
    return false;
}

bool AP33772::setVoltage(uint16_t voltage_mv) {
    if (_wire == nullptr) return false;
    
    uint8_t pdo_index = voltageToPDO(voltage_mv);
    
    // Write PDO index to voltage set register
    if (!writeRegister(AP33772_REG_VOLTAGE_SET, pdo_index)) {
        return false;
    }
    
    // Wait for voltage negotiation
    delay(500);
    
    // Check if negotiation was successful
    return isPDActive();
}

uint16_t AP33772::readVoltage() {
    uint16_t voltage = 0;
    if (!readRegister16(AP33772_REG_VOLTAGE, &voltage)) {
        return 0;
    }
    
    // Convert from device units to mV
    // Assuming 100mV per unit (adjust based on datasheet)
    return voltage * 100;
}

uint16_t AP33772::readCurrent() {
    uint16_t current = 0;
    if (!readRegister16(AP33772_REG_CURRENT, &current)) {
        return 0;
    }
    
    // Convert from device units to mA
    // Assuming 50mA per unit (adjust based on datasheet)
    return current * 50;
}

uint8_t AP33772::getStatus() {
    uint8_t status = 0;
    readRegister(AP33772_REG_STATUS, &status);
    return status;
}

bool AP33772::isPDActive() {
    uint8_t status = getStatus();
    return (status & STATUS_PD_SUCCESS) != 0;
}

uint8_t AP33772::getAvailablePDOs() {
    uint8_t pdo_num = 0;
    readRegister(AP33772_REG_PDO_NUM, &pdo_num);
    return pdo_num;
}

bool AP33772::getPDOInfo(uint8_t pdo_index, uint16_t *voltage, uint16_t *current) {
    if (voltage == nullptr || current == nullptr) return false;
    if (pdo_index > 6) return false;
    
    // Select PDO index
    if (!writeRegister(AP33772_REG_PDO_INFO, pdo_index)) {
        return false;
    }
    
    delay(10);
    
    // Read PDO information (implementation depends on actual register format)
    // This is a simplified example
    uint16_t pdo_data = 0;
    if (!readRegister16(AP33772_REG_PDO_INFO + 1, &pdo_data)) {
        return false;
    }
    
    // Parse PDO data (format depends on datasheet)
    *voltage = (pdo_data & 0x3FF) * 50;  // Example: 10 bits for voltage
    *current = ((pdo_data >> 10) & 0x3F) * 50;  // Example: 6 bits for current
    
    return true;
}

void AP33772::printDebugInfo() {
    Serial.println("=== AP33772 Debug Info ===");
    Serial.print("I2C Address: 0x");
    Serial.println(_i2cAddress, HEX);
    
    Serial.print("PD Active: ");
    Serial.println(isPDActive() ? "Yes" : "No");
    
    Serial.print("Voltage: ");
    Serial.print(readVoltage());
    Serial.println(" mV");
    
    Serial.print("Current: ");
    Serial.print(readCurrent());
    Serial.println(" mA");
    
    Serial.print("Status: 0x");
    Serial.println(getStatus(), HEX);
    
    Serial.print("Available PDOs: ");
    Serial.println(getAvailablePDOs());
    
    Serial.println("=========================");
}

bool AP33772::writeRegister(uint8_t reg, uint8_t value) {
    if (_wire == nullptr) return false;
    
    _wire->beginTransmission(_i2cAddress);
    _wire->write(reg);
    _wire->write(value);
    uint8_t error = _wire->endTransmission();
    
    return (error == 0);
}

bool AP33772::readRegister(uint8_t reg, uint8_t *value) {
    if (_wire == nullptr || value == nullptr) return false;
    
    _wire->beginTransmission(_i2cAddress);
    _wire->write(reg);
    uint8_t error = _wire->endTransmission(false);
    
    if (error != 0) return false;
    
    uint8_t bytesRead = _wire->requestFrom(_i2cAddress, (uint8_t)1);
    if (bytesRead != 1) return false;
    
    *value = _wire->read();
    return true;
}

bool AP33772::readRegister16(uint8_t reg, uint16_t *value) {
    if (_wire == nullptr || value == nullptr) return false;
    
    _wire->beginTransmission(_i2cAddress);
    _wire->write(reg);
    uint8_t error = _wire->endTransmission(false);
    
    if (error != 0) return false;
    
    uint8_t bytesRead = _wire->requestFrom(_i2cAddress, (uint8_t)2);
    if (bytesRead != 2) return false;
    
    uint8_t lowByte = _wire->read();
    uint8_t highByte = _wire->read();
    
    *value = (highByte << 8) | lowByte;
    return true;
}

uint8_t AP33772::voltageToPDO(uint16_t voltage_mv) {
    // Map voltage to PDO index
    // Standard USB-C PD voltages
    if (voltage_mv <= 5000) return 0;       // 5V
    else if (voltage_mv <= 9000) return 1;  // 9V
    else if (voltage_mv <= 12000) return 2; // 12V
    else if (voltage_mv <= 15000) return 3; // 15V
    else if (voltage_mv <= 20000) return 4; // 20V
    return 0; // Default to 5V
}
