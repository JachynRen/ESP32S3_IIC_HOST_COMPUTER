#include "i2c_bus.h"

void I2CBus::begin() {
    Serial.println("\n--- I2C 初始化 ---");
    Serial.printf("SDA: GPIO%d, SCL: GPIO%d, 频率: %lu Hz\n", I2C_SDA, I2C_SCL, I2C_FREQUENCY);
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_FREQUENCY);
    Wire.setTimeout(50); // 50ms超时,确保慢设备能响应
    Serial.println("I2C 已启动");
}

void I2CBus::scanDevices(bool fast) {
    uint8_t deviceCount = 0;
    unsigned long start = millis();
    uint8_t foundAddrs[128];  // 存储找到的地址(最多128个)

    Serial.println("开始扫描...");
    Serial.printf("I2C 频率: %lu Hz, 超时: %lu ms\n", I2C_FREQUENCY, Wire.getTimeout());

    if (fast) {
        // 快速模式: 只扫描常见设备地址
        uint8_t commonAddrs[] = {
            0x40, 0x41, 0x42, 0x43,       // PCA9685
            0x44, 0x45, 0x46, 0x47,       // PCA9685 (扩展)
            0x68, 0x69,                   // MPU6050/DS3231
            0x3C, 0x3D,                   // OLED
            0x76, 0x77                    // BMP280/BME280
        };

        Serial.printf("扫描 %d 个常见地址...\n", sizeof(commonAddrs));
        for (uint8_t addr : commonAddrs) {
            Wire.beginTransmission(addr);
            uint8_t error = Wire.endTransmission();
            if (error == 0) {
                foundAddrs[deviceCount] = addr;
                deviceCount++;
            } else if (error != 2) {  // error 2 = 无响应(正常)
                Serial.printf("  地址 0x%02X 错误码: %d\n", addr, error);
            }
        }
    } else {
        // 完整模式: 扫描全部地址
        Serial.println("完整扫描 0x03-0x77...");
        for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
            Wire.beginTransmission(addr);
            uint8_t error = Wire.endTransmission();
            if (error == 0) {
                foundAddrs[deviceCount] = addr;
                deviceCount++;
            }
        }
    }

    unsigned long elapsed = millis() - start;

    // 扫描完成后一次性输出结果
    if (fast) {
        Serial.println("\n--- 快速扫描结果 ---");
    } else {
        Serial.println("\n--- 完整扫描结果 ---");
    }

    for (int i = 0; i < deviceCount; i++) {
        Serial.printf("  ✓ 找到设备: 0x%02X\n", foundAddrs[i]);
    }

    if (deviceCount == 0) {
        Serial.println("✗ 未找到任何 I2C 设备");
        Serial.println("\n排查建议:");
        Serial.println("  1. 检查 I2C 接线 (SDA/SCL 是否接反)");
        Serial.println("  2. 确认设备供电正常");
        Serial.println("  3. 检查是否需要上拉电阻");
        Serial.println("  4. 尝试降低 I2C 频率 (当前 400kHz)");
    } else {
        Serial.printf("✓ 共找到 %d 个 I2C 设备\n", deviceCount);
    }
    Serial.printf("扫描耗时: %lu ms\n", elapsed);
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
