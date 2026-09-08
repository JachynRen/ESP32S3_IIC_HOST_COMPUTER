#ifndef PCA9685_H
#define PCA9685_H

#include <Arduino.h>
#include "../core/i2c_bus.h"

// PCA9685 默认 I2C 地址
#ifndef PCA9685_ADDR
#define PCA9685_ADDR 0x40
#endif

// PCA9685 寄存器定义
#define PCA9685_MODE1      0x00
#define PCA9685_MODE2      0x01
#define PCA9685_SUBADR1    0x02
#define PCA9685_SUBADR2    0x03
#define PCA9685_SUBADR3    0x04
#define PCA9685_ALLCALLADR 0x05
#define PCA9685_LED0_ON_L  0x06
#define PCA9685_LED0_ON_H  0x07
#define PCA9685_LED0_OFF_L 0x08
#define PCA9685_LED0_OFF_H 0x09
#define PCA9685_ALLLED_ON_L 0xFA
#define PCA9685_ALLLED_ON_H 0xFB
#define PCA9685_ALLLED_OFF_L 0xFC
#define PCA9685_ALLLED_OFF_H 0xFD
#define PCA9685_PRE_SCALE    0xFE

// PCA9685 模式位定义
#define PCA9685_MODE1_RESTART    0x80
#define PCA9685_MODE1_EXTCLK     0x40
#define PCA9685_MODE1_AI         0x20
#define PCA9685_MODE1_SLEEP      0x10
#define PCA9685_MODE1_SUB1       0x08
#define PCA9685_MODE1_SUB2       0x04
#define PCA9685_MODE1_ALLCALL    0x01

#define PCA9685_MODE2_INVRT      0x10
#define PCA9685_MODE2_OUTDRV     0x04

#define PCA9685_FREQ_OSC_INTERNAL 25000000
#define PCA9685_MAX_PWM_VALUE     4096

class PCA9685 {
public:
    PCA9685(uint8_t addr = PCA9685_ADDR);

    bool begin();
    bool reset();
    
    // 频率设置
    void setPWMFreq(float freq);
    
    // 单通道控制 (0-15)
    void setPWM(uint8_t channel, uint16_t on, uint16_t off);
    void setPin(uint8_t channel, uint16_t value, bool invert = false);
    
    // 全通道控制
    void setAllPWM(uint16_t on, uint16_t off);
    
    // 舵机控制 (0-180度映射到PWM)
    void setServo(uint8_t channel, uint16_t angle, uint16_t minPulse = 150, uint16_t maxPulse = 600);
    
    // LED亮度控制 (0-4095)
    void setLED(uint8_t channel, uint16_t brightness);
    
    // 睡眠/唤醒模式
    void sleep();
    void wakeup();
    
    // 输出状态
    void printStatus();
    
    uint8_t getAddress() const { return _addr; }
    bool isInitialized() const { return _initialized; }

private:
    uint8_t _addr;
    bool _initialized = false;
    
    bool writeReg(uint8_t reg, uint8_t value);
    uint8_t readReg(uint8_t reg);
};

#endif // PCA9685_H
