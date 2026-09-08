#include "i2c_bus.h"

void I2CBus::begin() {
    Serial.println("\n--- I2C 初始化 ---");
    Serial.printf("SDA: GPIO%d, SCL: GPIO%d, 频率: %lu Hz\n", I2C_SDA, I2C_SCL, I2C_FREQUENCY);
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_FREQUENCY);
    Serial.println("I2C 已启动");
}

void I2CBus::scanDevices() {
    uint8_t deviceCount = 0;

    Serial.println("扫描 I2C 地址 (0x03 - 0x77):");

    for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
        Wire.beginTransmission(addr);
        uint8_t error = Wire.endTransmission();

        if (error == 0) {
            Serial.printf("  找到设备: 0x%02X\n", addr);
            deviceCount++;
        }
    }

    if (deviceCount == 0) {
        Serial.println("未找到任何 I2C 设备");
    } else {
        Serial.printf("共找到 %d 个 I2C 设备\n", deviceCount);
    }
}

bool I2CBus::writeRegister(uint8_t addr, uint8_t reg, uint8_t value) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(value);
    uint8_t err = Wire.endTransmission();
    return (err == 0);
}

bool I2CBus::writeRegisters(uint8_t addr, uint8_t reg, const uint8_t* data, uint8_t len) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(data, len);
    uint8_t err = Wire.endTransmission();
    return (err == 0);
}

bool I2CBus::readRegister(uint8_t addr, uint8_t reg, uint8_t* value) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    uint8_t err = Wire.endTransmission();
    
    if (err != 0) return false;
    
    int bytesRead = Wire.requestFrom(addr, (uint8_t)1);
    if (bytesRead > 0) {
        *value = Wire.read();
        return true;
    }
    return false;
}

bool I2CBus::readRegisters(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    uint8_t err = Wire.endTransmission();
    
    if (err != 0) return false;
    
    int bytesRead = Wire.requestFrom(addr, len);
    if (bytesRead > 0) {
        for (int i = 0; i < bytesRead && i < len; i++) {
            data[i] = Wire.read();
        }
        return true;
    }
    return false;
}

bool I2CBus::deviceExists(uint8_t addr) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    return (err == 0);
}
