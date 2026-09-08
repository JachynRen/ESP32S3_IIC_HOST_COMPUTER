#include "pca9685.h"

PCA9685::PCA9685(uint8_t addr) : _addr(addr) {}

bool PCA9685::begin() {
    Serial.println("\n--- PCA9685 初始化 ---");
    Serial.printf("地址: 0x%02X\n", _addr);
    
    if (!I2CBus::getInstance().deviceExists(_addr)) {
        Serial.printf("错误: 未找到 PCA9685 (0x%02X)\n", _addr);
        return false;
    }
    
    reset();
    setPWMFreq(50); // 默认50Hz，适合舵机
    _initialized = true;
    
    Serial.println("PCA9685 初始化完成");
    return true;
}

bool PCA9685::reset() {
    Serial.println("PCA9685 软件复位");
    return writeReg(PCA9685_MODE1, PCA9685_MODE1_RESTART);
}

bool PCA9685::writeReg(uint8_t reg, uint8_t value) {
    return I2CBus::getInstance().writeRegister(_addr, reg, value);
}

uint8_t PCA9685::readReg(uint8_t reg) {
    uint8_t value = 0;
    if (I2CBus::getInstance().readRegister(_addr, reg, &value)) {
        return value;
    }
    return 0xFF;
}

void PCA9685::setPWMFreq(float freq) {
    // 计算预分频值
    freq *= 0.9; // 补偿频率偏差
    uint8_t prescale = (PCA9685_FREQ_OSC_INTERNAL / 4096.0 / freq) - 1;
    
    uint8_t oldmode = readReg(PCA9685_MODE1);
    uint8_t newmode = (oldmode & 0x7F) | PCA9685_MODE1_SLEEP;
    
    writeReg(PCA9685_MODE1, newmode); // 进入睡眠模式
    writeReg(PCA9685_PRE_SCALE, prescale); // 设置预分频
    writeReg(PCA9685_MODE1, oldmode); // 恢复原模式
    
    delay(5);
    writeReg(PCA9685_MODE1, oldmode | PCA9685_MODE1_RESTART); // 重启
    
    Serial.printf("PCA9685 频率设置为: %.1f Hz (prescale=%d)\n", freq, prescale);
}

void PCA9685::setPWM(uint8_t channel, uint16_t on, uint16_t off) {
    if (channel > 15) return;
    
    uint8_t base = PCA9685_LED0_ON_L + 4 * channel;
    writeReg(base, on & 0xFF);
    writeReg(base + 1, on >> 8);
    writeReg(base + 2, off & 0xFF);
    writeReg(base + 3, off >> 8);
}

void PCA9685::setPin(uint8_t channel, uint16_t value, bool invert) {
    if (channel > 15) return;
    
    value = constrain(value, 0, PCA9685_MAX_PWM_VALUE);
    
    if (invert) {
        value = PCA9685_MAX_PWM_VALUE - value;
    }
    
    if (value == 0) {
        setPWM(channel, 0, 4096); // 完全关闭
    } else if (value >= PCA9685_MAX_PWM_VALUE) {
        setPWM(channel, 4096, 0); // 完全开启
    } else {
        setPWM(channel, 0, value);
    }
}

void PCA9685::setAllPWM(uint16_t on, uint16_t off) {
    writeReg(PCA9685_ALLLED_ON_L, on & 0xFF);
    writeReg(PCA9685_ALLLED_ON_H, on >> 8);
    writeReg(PCA9685_ALLLED_OFF_L, off & 0xFF);
    writeReg(PCA9685_ALLLED_OFF_H, off >> 8);
}

void PCA9685::setServo(uint8_t channel, uint16_t angle, uint16_t minPulse, uint16_t maxPulse) {
    if (channel > 15) return;
    
    angle = constrain(angle, 0, 180);
    
    // 将角度映射到PWM值
    uint16_t pulse = map(angle, 0, 180, minPulse, maxPulse);
    setPWM(channel, 0, pulse);
}

void PCA9685::setLED(uint8_t channel, uint16_t brightness) {
    if (channel > 15) return;
    
    brightness = constrain(brightness, 0, PCA9685_MAX_PWM_VALUE);
    setPin(channel, brightness);
}

void PCA9685::sleep() {
    uint8_t mode = readReg(PCA9685_MODE1);
    writeReg(PCA9685_MODE1, mode | PCA9685_MODE1_SLEEP);
    Serial.println("PCA9685 进入睡眠模式");
}

void PCA9685::wakeup() {
    uint8_t mode = readReg(PCA9685_MODE1);
    writeReg(PCA9685_MODE1, mode & ~PCA9685_MODE1_SLEEP);
    Serial.println("PCA9685 已唤醒");
}

void PCA9685::printStatus() {
    Serial.println("\n--- PCA9685 状态 ---");
    Serial.printf("地址: 0x%02X\n", _addr);
    Serial.printf("初始化: %s\n", _initialized ? "是" : "否");
    
    if (_initialized) {
        uint8_t mode1 = readReg(PCA9685_MODE1);
        uint8_t prescale = readReg(PCA9685_PRE_SCALE);
        
        Serial.printf("MODE1: 0x%02X\n", mode1);
        Serial.printf("预分频: %d\n", prescale);
        
        float freq = PCA9685_FREQ_OSC_INTERNAL / 4096.0 / (prescale + 1) / 0.9;
        Serial.printf("频率: %.1f Hz\n", freq);
        Serial.printf("睡眠模式: %s\n", (mode1 & PCA9685_MODE1_SLEEP) ? "是" : "否");
    }
    Serial.println();
}
