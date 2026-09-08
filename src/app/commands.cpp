#include "commands.h"
#include "../core/i2c_bus.h"

void CommandProcessor::process(const char* cmd) {
    // 跳过前导空格
    while (*cmd == ' ') cmd++;
    if (*cmd == '\0') return;

    // 解析命令关键字
    if (strncmp(cmd, "help", 4) == 0) {
        printHelp();

    } else if (strncmp(cmd, "scan", 4) == 0) {
        Serial.println("\n--- I2C 扫描 ---");
        I2CBus::getInstance().scanDevices();

    } else if (strncmp(cmd, "i2c write", 9) == 0 || strncmp(cmd, "i2c w", 7) == 0) {
        uint8_t addr, reg, value;
        if (sscanf(cmd, "%*s %*s 0x%hhx 0x%hhx 0x%hhx", &addr, &reg, &value) == 3 ||
            sscanf(cmd, "%*s %*s %hhu %hhu %hhu", &addr, &reg, &value) == 3) {
            bool ok = I2CBus::getInstance().writeRegister(addr, reg, value);
            Serial.printf("I2C 写: 地址 0x%02X, 寄存器 0x%02X, 值 0x%02X -> %s\n", 
                         addr, reg, value, ok ? "成功" : "失败");
        } else {
            Serial.println("用法: i2c w <addr> <reg> <value>");
        }

    } else if (strncmp(cmd, "i2c read", 8) == 0) {
        uint8_t addr, reg;
        int count;
        if (sscanf(cmd, "%*s %*s 0x%hhx 0x%hhx %d", &addr, &reg, &count) == 3 ||
            sscanf(cmd, "%*s %*s %hhu %hhu %d", &addr, &reg, &count) == 3) {
            Serial.printf("I2C 读: 地址 0x%02X, 寄存器 0x%02X, 长度 %d\n", addr, reg, count);
            uint8_t data[256];
            bool ok = I2CBus::getInstance().readRegisters(addr, reg, data, count);
            if (ok) {
                for (int i = 0; i < count; i++) {
                    Serial.printf("  [%d] 0x%02X (%d)\n", i, data[i], data[i]);
                }
            } else {
                Serial.println("读取失败");
            }
        } else {
            Serial.println("用法: i2c r <addr> <reg> <count>");
        }

    } else if (strncmp(cmd, "i2c dump", 8) == 0) {
        uint8_t addr, reg;
        int count;
        if (sscanf(cmd, "%*s %*s 0x%hhx 0x%hhx %d", &addr, &reg, &count) == 3 ||
            sscanf(cmd, "%*s %*s %hhu %hhu %d", &addr, &reg, &count) == 3) {
            Serial.printf("转储设备 0x%02X 从寄存器 0x%02X 读取 %d 字节:\n", addr, reg, count);
            uint8_t data[256];
            bool ok = I2CBus::getInstance().readRegisters(addr, reg, data, count);
            if (ok) {
                for (int i = 0; i < count; i++) {
                    Serial.printf("0x%02X ", data[i]);
                    if ((i + 1) % 16 == 0) Serial.println();
                }
                Serial.println();
            } else {
                Serial.println("读取失败");
            }
        } else {
            Serial.println("用法: i2c dump <addr> <reg> <count>");
        }

    } else if (strncmp(cmd, "lcd clear", 9) == 0 || strncmp(cmd, "lcd cls", 7) == 0) {
        LCD1602::getInstance().clear();
        Serial.println("LCD 已清空");

    } else if (strncmp(cmd, "lcd bl on", 9) == 0) {
        LCD1602::getInstance().backlight(true);
        Serial.println("LCD 背光已开启");

    } else if (strncmp(cmd, "lcd bl off", 10) == 0) {
        LCD1602::getInstance().backlight(false);
        Serial.println("LCD 背光已关闭");

    } else if (strncmp(cmd, "lcd anim", 8) == 0) {
        Serial.println("播放宠物表情动画...");
        LCD1602::getInstance().clear();
        LCD1602::getInstance().setCursor(0, 0);
        LCD1602::getInstance().print("\x00\x01\x02\x03\x04\x05\x06");
        LCD1602::getInstance().setCursor(0, 1);
        LCD1602::getInstance().print("Pet Animation");
        delay(2000);
        LCD1602::getInstance().restoreDefault();

    } else if (strncmp(cmd, "lcd ", 4) == 0) {
        int row, col;
        char text[64];
        if (sscanf(cmd, "%*s %d %d %63[^\n]", &row, &col, text) == 3) {
            LCD1602::getInstance().writeText(row, col, text);
        } else {
            Serial.println("用法: lcd <row 0-1> <col 0-15> <text>");
        }

    } else if (strncmp(cmd, "pca scan", 8) == 0) {
        pca9685Scan();

    } else if (strncmp(cmd, "pca init", 8) == 0) {
        uint8_t addr;
        if (sscanf(cmd, "%*s %*s 0x%hhx", &addr) == 1 ||
            sscanf(cmd, "%*s %*s %hhu", &addr) == 1) {
            pca9685Init(addr);
        } else {
            Serial.println("用法: pca init <addr>");
            Serial.println("示例: pca init 0x40");
        }

    } else if (strncmp(cmd, "pca pwm", 7) == 0) {
        uint8_t addr, channel;
        uint16_t on, off;
        if (sscanf(cmd, "%*s %*s 0x%hhx %hhu %hu %hu", &addr, &channel, &on, &off) == 4) {
            pca9685SetPWM(addr, channel, on, off);
        } else {
            Serial.println("用法: pca pwm <addr> <ch> <on> <off>");
            Serial.println("示例: pca pwm 0x40 0 0 2048");
        }

    } else if (strncmp(cmd, "pca servo", 9) == 0) {
        uint8_t addr, channel;
        uint16_t angle;
        if (sscanf(cmd, "%*s %*s 0x%hhx %hhu %hu", &addr, &channel, &angle) == 3) {
            pca9685SetServo(addr, channel, angle);
        } else {
            Serial.println("用法: pca servo <addr> <ch> <angle>");
            Serial.println("示例: pca servo 0x40 0 90");
        }

    } else if (strncmp(cmd, "pca led", 7) == 0) {
        uint8_t addr, channel;
        uint16_t brightness;
        if (sscanf(cmd, "%*s %*s 0x%hhx %hhu %hu", &addr, &channel, &brightness) == 3) {
            pca9685SetLED(addr, channel, brightness);
        } else {
            Serial.println("用法: pca led <addr> <ch> <brightness 0-4095>");
            Serial.println("示例: pca led 0x40 0 2048");
        }

    } else if (strncmp(cmd, "pca freq", 8) == 0) {
        uint8_t addr;
        float freq;
        if (sscanf(cmd, "%*s %*s 0x%hhx %f", &addr, &freq) == 2) {
            pca9685SetFreq(addr, freq);
        } else {
            Serial.println("用法: pca freq <addr> <freq>");
            Serial.println("示例: pca freq 0x40 50");
        }

    } else if (strncmp(cmd, "pca status", 10) == 0) {
        uint8_t addr;
        if (sscanf(cmd, "%*s %*s 0x%hhx", &addr) == 1) {
            pca9685Status(addr);
        } else {
            Serial.println("用法: pca status <addr>");
        }

    } else if (strncmp(cmd, "pca sleep", 9) == 0) {
        uint8_t addr;
        if (sscanf(cmd, "%*s %*s 0x%hhx", &addr) == 1) {
            pca9685Sleep(addr);
        } else {
            Serial.println("用法: pca sleep <addr>");
        }

    } else if (strncmp(cmd, "pca wakeup", 10) == 0) {
        uint8_t addr;
        if (sscanf(cmd, "%*s %*s 0x%hhx", &addr) == 1) {
            pca9685Wakeup(addr);
        } else {
            Serial.println("用法: pca wakeup <addr>");
        }

    } else {
        Serial.printf("未知命令: %s\n", cmd);
        Serial.println("输入 help 查看可用命令");
    }

    // I2C操作后恢复LCD
    LCD1602::getInstance().restoreDefault();
}

void CommandProcessor::printHelp() {
    Serial.println("\n========================================");
    Serial.println("  可用命令");
    Serial.println("========================================");
    Serial.println();
    Serial.println("【系统】");
    Serial.println("  help              - 显示此帮助");
    Serial.println("  scan              - 扫描 I2C 总线");
    Serial.println();
    Serial.println("【I2C 读写】");
    Serial.println("  i2c w <addr> <reg> <value>");
    Serial.println("                    - 向 I2C 设备寄存器写入值");
    Serial.println("  i2c r <addr> <reg> <count>");
    Serial.println("                    - 从 I2C 设备寄存器读取数据");
    Serial.println("  i2c dump <addr> <reg> <count>");
    Serial.println("                    - 转储 I2C 设备寄存器 (十六进制)");
    Serial.println();
    Serial.println("【LCD1602 控制】");
    Serial.println("  lcd <row> <col> <text>");
    Serial.println("                    - 在指定位置显示文字");
    Serial.println("  lcd clear         - 清空屏幕");
    Serial.println("  lcd bl on/off     - 背光开关");
    Serial.println("  lcd anim          - 播放宠物表情动画");
    Serial.println();
    Serial.println("【PCA9685 控制】");
    Serial.println("  pca scan          - 扫描 PCA9685 设备");
    Serial.println("  pca init <addr>   - 初始化 PCA9685");
    Serial.println("  pca pwm <addr> <ch> <on> <off>");
    Serial.println("                    - 设置 PWM 输出");
    Serial.println("  pca servo <addr> <ch> <angle>");
    Serial.println("                    - 控制舵机 (0-180度)");
    Serial.println("  pca led <addr> <ch> <brightness>");
    Serial.println("                    - 控制 LED 亮度 (0-4095)");
    Serial.println("  pca freq <addr> <freq>");
    Serial.println("                    - 设置 PWM 频率");
    Serial.println("  pca status <addr> - 查看设备状态");
    Serial.println("  pca sleep <addr>  - 设备休眠");
    Serial.println("  pca wakeup <addr> - 唤醒设备");
    Serial.println();
    Serial.println("【示例】");
    Serial.println("  i2c w 0x27 0x00 0xFF    - 向 0x27 写寄存器 0x00 = 0xFF");
    Serial.println("  lcd 0 0 Hello World     - 第 1 行显示 Hello World");
    Serial.println("  pca init 0x40           - 初始化 PCA9685");
    Serial.println("  pca servo 0x40 0 90     - 舵机0转到90度");
    Serial.println("  pca led 0x40 0 2048     - LED0设置为50%亮度");
    Serial.println();
}

PCA9685* CommandProcessor::getPCA9685(uint8_t addr) {
    for (int i = 0; i < pca9685Count; i++) {
        if (pca9685Devices[i]->getAddress() == addr) {
            return pca9685Devices[i];
        }
    }
    return nullptr;
}

void CommandProcessor::pca9685Scan() {
    Serial.println("\n--- PCA9685 扫描 ---");
    uint8_t commonAddrs[] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47};
    bool found = false;
    
    for (uint8_t addr : commonAddrs) {
        if (I2CBus::getInstance().deviceExists(addr)) {
            Serial.printf("  找到 PCA9685: 0x%02X\n", addr);
            found = true;
        }
    }
    
    if (!found) {
        Serial.println("未找到 PCA9685 设备");
        Serial.println("提示: PCA9685 默认地址为 0x40-0x47");
    }
}

void CommandProcessor::pca9685Init(uint8_t addr) {
    Serial.printf("\n--- 初始化 PCA9685 @ 0x%02X ---\n", addr);
    
    if (pca9685Count >= 8) {
        Serial.println("错误: 已达到最大设备数 (8)");
        return;
    }
    
    PCA9685* pca = new PCA9685(addr);
    if (pca->begin()) {
        pca9685Devices[pca9685Count++] = pca;
        Serial.printf("PCA9685 @ 0x%02X 初始化成功 (索引: %d)\n", addr, pca9685Count - 1);
    } else {
        Serial.printf("PCA9685 @ 0x%02X 初始化失败\n", addr);
        delete pca;
    }
}

void CommandProcessor::pca9685SetPWM(uint8_t addr, uint8_t channel, uint16_t on, uint16_t off) {
    PCA9685* pca = getPCA9685(addr);
    if (!pca) {
        Serial.printf("错误: PCA9685 @ 0x%02X 未初始化，请先执行 pca init 0x%02X\n", addr, addr);
        return;
    }
    
    if (channel > 15) {
        Serial.println("错误: 通道号超出范围 (0-15)");
        return;
    }
    
    pca->setPWM(channel, on, off);
    Serial.printf("PCA9685 @ 0x%02X 通道 %d PWM: ON=%d, OFF=%d\n", addr, channel, on, off);
}

void CommandProcessor::pca9685SetServo(uint8_t addr, uint8_t channel, uint16_t angle) {
    PCA9685* pca = getPCA9685(addr);
    if (!pca) {
        Serial.printf("PCA9685 @ 0x%02X 未初始化，自动初始化...\n", addr);
        pca9685Init(addr);
        pca = getPCA9685(addr);
        if (!pca) {
            Serial.printf("错误: PCA9685 @ 0x%02X 初始化失败\n", addr);
            return;
        }
    }

    if (channel > 15) {
        Serial.println("错误: 通道号超出范围 (0-15)");
        return;
    }

    if (angle > 180) {
        Serial.println("错误: 角度超出范围 (0-180)");
        return;
    }

    pca->setServo(channel, angle);
    Serial.printf("PCA9685 @ 0x%02X 通道 %d 舵机角度: %d°\n", addr, channel, angle);
}

void CommandProcessor::pca9685SetLED(uint8_t addr, uint8_t channel, uint16_t brightness) {
    PCA9685* pca = getPCA9685(addr);
    if (!pca) {
        Serial.printf("错误: PCA9685 @ 0x%02X 未初始化\n", addr);
        return;
    }
    
    if (channel > 15) {
        Serial.println("错误: 通道号超出范围 (0-15)");
        return;
    }
    
    if (brightness > 4095) {
        Serial.println("错误: 亮度超出范围 (0-4095)");
        return;
    }
    
    pca->setLED(channel, brightness);
    Serial.printf("PCA9685 @ 0x%02X 通道 %d LED亮度: %d/%d (%.1f%%)\n", 
                 addr, channel, brightness, 4095, (float)brightness / 4095 * 100);
}

void CommandProcessor::pca9685SetFreq(uint8_t addr, float freq) {
    PCA9685* pca = getPCA9685(addr);
    if (!pca) {
        Serial.printf("错误: PCA9685 @ 0x%02X 未初始化\n", addr);
        return;
    }
    
    pca->setPWMFreq(freq);
    Serial.printf("PCA9685 @ 0x%02X 频率设置为: %.1f Hz\n", addr, freq);
}

void CommandProcessor::pca9685Status(uint8_t addr) {
    PCA9685* pca = getPCA9685(addr);
    if (!pca) {
        Serial.printf("错误: PCA9685 @ 0x%02X 未初始化\n", addr);
        return;
    }
    
    pca->printStatus();
}

void CommandProcessor::pca9685Sleep(uint8_t addr) {
    PCA9685* pca = getPCA9685(addr);
    if (!pca) {
        Serial.printf("错误: PCA9685 @ 0x%02X 未初始化\n", addr);
        return;
    }
    
    pca->sleep();
}

void CommandProcessor::pca9685Wakeup(uint8_t addr) {
    PCA9685* pca = getPCA9685(addr);
    if (!pca) {
        Serial.printf("错误: PCA9685 @ 0x%02X 未初始化\n", addr);
        return;
    }
    
    pca->wakeup();
}
