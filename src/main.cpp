#include <Arduino.h>
#include "core/i2c_bus.h"
#include "core/serial_console.h"
#include "app/commands.h"

void setup() {
    // 初始化串口控制台
    SerialConsole::getInstance().begin();
    SerialConsole::getInstance().printBanner();

    // 初始化 I2C 总线
    I2CBus::getInstance().begin();
    I2CBus::getInstance().scanDevices();

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
