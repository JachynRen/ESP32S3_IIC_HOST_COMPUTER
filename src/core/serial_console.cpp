#include "serial_console.h"

void SerialConsole::begin(uint32_t baud) {
    Serial.begin(baud);
    delay(500);
}

void SerialConsole::printBanner() {
    Serial.println();
    Serial.println("========================================");
    Serial.println("  ESP32-S3-N16R8 I2C 控制台");
    Serial.println("========================================");
    Serial.printf("芯片: %s, 核心数: %u\n", ESP.getChipModel(), ESP.getChipCores());
    Serial.printf("闪存: %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    Serial.printf("PSRAM: %u MB, 可用: %s\n",
                  ESP.getPsramSize() / (1024 * 1024),
                  psramFound() ? "是" : "否");
}

void SerialConsole::printPrompt() {
    Serial.print("> ");
}

bool SerialConsole::readCommand(char* buffer, int bufferSize) {
    while (Serial.available() > 0) {
        char ch = Serial.read();

        if (ch == '\n' || ch == '\r') {
            cmdBuffer[cmdIndex] = '\0';
            if (cmdIndex > 0) {
                Serial.println();
                strncpy(buffer, cmdBuffer, bufferSize - 1);
                buffer[bufferSize - 1] = '\0';
                cmdIndex = 0;
                return true;
            }
            cmdIndex = 0;
            return false;
        } else if (ch == '\b' || ch == 0x7F) {
            if (cmdIndex > 0) {
                cmdIndex--;
                Serial.print("\b \b");
            }
        } else if (cmdIndex < CMD_BUF_SIZE - 1 && ch >= 0x20 && ch <= 0x7E) {
            cmdBuffer[cmdIndex++] = ch;
            Serial.write(ch);
        }
    }
    return false;
}
