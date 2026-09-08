#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <Wire.h>
#include <Arduino.h>

// I2C 引脚定义 (PCA9685 / 通用设备)
#ifndef I2C_SDA
#define I2C_SDA 8
#endif

#ifndef I2C_SCL
#define I2C_SCL 9
#endif

#ifndef I2C_FREQUENCY
#define I2C_FREQUENCY 100000
#endif

class I2CBus {
public:
    static I2CBus& getInstance() {
        static I2CBus instance;
        return instance;
    }

    void begin();
    void scanDevices(bool fast = true);
    
    bool writeRegister(uint8_t addr, uint8_t reg, uint8_t value);
    bool writeRegisters(uint8_t addr, uint8_t reg, const uint8_t* data, uint8_t len);
    bool readRegister(uint8_t addr, uint8_t reg, uint8_t* value);
    bool readRegisters(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len);
    
    bool deviceExists(uint8_t addr);

private:
    I2CBus() = default;
    I2CBus(const I2CBus&) = delete;
    I2CBus& operator=(const I2CBus&) = delete;
};

#endif // I2C_BUS_H
