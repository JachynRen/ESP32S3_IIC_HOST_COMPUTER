#include <Arduino.h>
#include "core/i2c_bus.h"
#include "core/serial_console.h"
#include "drivers/lcd1602.h"
#include "app/commands.h"

void setup() {
    // 初始化串口控制台
    SerialConsole::getInstance().begin();
    SerialConsole::getInstance().printBanner();

    // 初始化 I2C 总线
    I2CBus::getInstance().begin();
    I2CBus::getInstance().scanDevices();

    // 初始化 LCD1602 (独立 I2C 总线，不与 PCA9685 冲突)
    // LCD: GPIO44/43, PCA9685: GPIO8/9
    LCD1602::getInstance().begin(6, 7);

    // 打印就绪信息
    Serial.println("\n--- 就绪 ---");
    Serial.println("输入 help 查看可用命令");
    SerialConsole::getInstance().printPrompt();
}

void loop() {
    static char cmdBuffer[CMD_BUF_SIZE];

    // 处理串口输入
    if (SerialConsole::getInstance().readCommand(cmdBuffer, CMD_BUF_SIZE)) {
        CommandProcessor::getInstance().process(cmdBuffer);
        SerialConsole::getInstance().printPrompt();
    }
}
