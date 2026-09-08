#ifndef SERIAL_CONSOLE_H
#define SERIAL_CONSOLE_H

#include <Arduino.h>

#ifndef CMD_BUF_SIZE
#define CMD_BUF_SIZE 128
#endif

class SerialConsole {
public:
    static SerialConsole& getInstance() {
        static SerialConsole instance;
        return instance;
    }

    void begin(uint32_t baud = 115200);
    bool readCommand(char* buffer, int bufferSize);
    void printBanner();
    void printPrompt();

private:
    SerialConsole() = default;
    SerialConsole(const SerialConsole&) = delete;
    SerialConsole& operator=(const SerialConsole&) = delete;

    char cmdBuffer[CMD_BUF_SIZE];
    int cmdIndex = 0;
};

#endif // SERIAL_CONSOLE_H
