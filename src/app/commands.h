#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>
#include "../drivers/pca9685.h"

class CommandProcessor {
public:
    static CommandProcessor& getInstance() {
        static CommandProcessor instance;
        return instance;
    }

    void process(const char* cmd);
    void printHelp();
    
    // PCA9685 相关命令
    void pca9685Scan();
    void pca9685Init(uint8_t addr);
    void pca9685SetPWM(uint8_t addr, uint8_t channel, uint16_t on, uint16_t off);
    void pca9685SetServo(uint8_t addr, uint8_t channel, uint16_t angle);
    void pca9685SetLED(uint8_t addr, uint8_t channel, uint16_t brightness);
    void pca9685SetFreq(uint8_t addr, float freq);
    void pca9685Status(uint8_t addr);
    void pca9685Sleep(uint8_t addr);
    void pca9685Wakeup(uint8_t addr);

private:
    CommandProcessor() = default;
    CommandProcessor(const CommandProcessor&) = delete;
    CommandProcessor& operator=(const CommandProcessor&) = delete;
    
    PCA9685* getPCA9685(uint8_t addr);
    
    // 存储PCA9685实例 (支持多个设备)
    PCA9685* pca9685Devices[8];
    uint8_t pca9685Count = 0;
};

#endif // COMMANDS_H
