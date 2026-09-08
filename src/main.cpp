#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// I2C 引脚定义
#define I2C_SDA 8
#define I2C_SCL 9

// LCD1602 I2C 地址
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// I2C 频率
#define I2C_FREQUENCY 100000

// 串口命令缓冲区
#define CMD_BUF_SIZE 128
char cmdBuffer[CMD_BUF_SIZE];
int cmdIndex = 0;

// 自定义字符: 宠物表情
byte catHappy[8] = {
  0b00000, 0b01010, 0b01110, 0b00000,
  0b10001, 0b01110, 0b00100, 0b00000
};
byte catSleep[8] = {
  0b00000, 0b01010, 0b01110, 0b00000,
  0b00000, 0b01110, 0b00100, 0b00000
};
byte catSurprised[8] = {
  0b00000, 0b01010, 0b01110, 0b00000,
  0b01010, 0b00000, 0b01110, 0b00000
};
byte catLove[8] = {
  0b00000, 0b01010, 0b01110, 0b00000,
  0b01010, 0b11111, 0b01110, 0b00100
};
byte fish[8] = {
  0b00000, 0b00100, 0b01110, 0b11111,
  0b01110, 0b00100, 0b00000, 0b00000
};
byte bone[8] = {
  0b00000, 0b01110, 0b10001, 0b10001,
  0b01110, 0b00000, 0b00000, 0b00000
};
byte paw[8] = {
  0b00000, 0b01010, 0b10101, 0b11111,
  0b01110, 0b00100, 0b00000, 0b00000
};

// 函数声明
void scanI2CDevices();
void initLCD();
void showCatAnimation();
void processCommand(char* cmd);
void printHelp();
void i2cWrite(byte addr, byte reg, byte value);
void i2cRead(byte addr, byte reg, int count);
void lcdRestore();
void lcdWrite(int row, int col, const char* text);
void lcdBacklight(bool on);

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("========================================");
  Serial.println("  ESP32-S3-N16R8 I2C 控制台");
  Serial.println("========================================");
  Serial.printf("芯片: %s, 核心数: %u\n", ESP.getChipModel(), ESP.getChipCores());
  Serial.printf("闪存: %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  Serial.printf("PSRAM: %u MB, 可用: %s\n",
                ESP.getPsramSize() / (1024 * 1024),
                psramFound() ? "是" : "否");

  // 初始化 I2C
  Serial.println("\n--- I2C 初始化 ---");
  Serial.printf("SDA: GPIO%d, SCL: GPIO%d, 频率: %lu Hz\n", I2C_SDA, I2C_SCL, I2C_FREQUENCY);
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(I2C_FREQUENCY);
  Serial.println("I2C 已启动");

  // 扫描 I2C 设备
  scanI2CDevices();

  // 初始化 LCD
  initLCD();

  // 注册自定义字符
  lcd.createChar(0, catHappy);
  lcd.createChar(1, catSleep);
  lcd.createChar(2, catSurprised);
  lcd.createChar(3, catLove);
  lcd.createChar(4, fish);
  lcd.createChar(5, bone);
  lcd.createChar(6, paw);

  Serial.println("自定义宠物表情已注册");
  Serial.println("\n--- 就绪 ---");
  Serial.println("输入 help 查看可用命令");
  Serial.print("> ");
}

void initLCD() {
  Serial.println("\n--- LCD1602 初始化 ---");
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32-S3 IIC");
  lcd.setCursor(0, 1);
  lcd.print("Console Ready");
  Serial.println("LCD1602 初始化完成");
  delay(1000);
}

void loop() {
  // 处理串口输入
  while (Serial.available() > 0) {
    char ch = Serial.read();

    if (ch == '\n' || ch == '\r') {
      // 回车换行，执行命令
      cmdBuffer[cmdIndex] = '\0';
      if (cmdIndex > 0) {
        Serial.println();
        processCommand(cmdBuffer);
        Serial.print("> ");
      }
      cmdIndex = 0;
    } else if (ch == '\b' || ch == 0x7F) {
      // 退格
      if (cmdIndex > 0) {
        cmdIndex--;
        Serial.print("\b \b");
      }
    } else if (cmdIndex < CMD_BUF_SIZE - 1 && ch >= 0x20 && ch <= 0x7E) {
      // 可打印字符
      cmdBuffer[cmdIndex++] = ch;
      Serial.write(ch);
    }
  }

  // 可以在这里添加其他周期性任务
  delay(10);
}

// 解析并执行串口命令
void processCommand(char* cmd) {
  // 跳过前导空格
  while (*cmd == ' ') cmd++;
  if (*cmd == '\0') return;

  // 解析命令关键字
  if (strncmp(cmd, "help", 4) == 0) {
    printHelp();

  } else if (strncmp(cmd, "scan", 4) == 0) {
    Serial.println("\n--- I2C 扫描 ---");
    scanI2CDevices();

  } else if (strncmp(cmd, "i2c write", 9) == 0 || strncmp(cmd, "i2c w", 7) == 0) {
    // i2c write <addr> <reg> <value>
    // 例如: i2c w 0x27 0x00 0xFF
    byte addr, reg, value;
    if (sscanf(cmd, "%*s %*s 0x%hhx 0x%hhx 0x%hhx", &addr, &reg, &value) == 3 ||
        sscanf(cmd, "%*s %*s %hhu %hhu %hhu", &addr, &reg, &value) == 3) {
      i2cWrite(addr, reg, value);
    } else {
      Serial.println("错误: 格式不正确");
      Serial.println("用法: i2c write <addr> <reg> <value>");
      Serial.println("示例: i2c w 0x27 0x00 0xFF");
    }

  } else if (strncmp(cmd, "i2c read", 8) == 0) {
    // i2c read <addr> <reg> <count>
    // 例如: i2c read 0x27 0x00 16
    byte addr;
    byte reg;
    int count;
    if (sscanf(cmd, "%*s %*s 0x%hhx 0x%hhx %d", &addr, &reg, &count) == 3 ||
        sscanf(cmd, "%*s %*s %hhu %hhu %d", &addr, &reg, &count) == 3) {
      i2cRead(addr, reg, count);
    } else {
      Serial.println("错误: 格式不正确");
      Serial.println("用法: i2c read <addr> <reg> <count>");
      Serial.println("示例: i2c r 0x27 0x00 16");
    }

  } else if (strncmp(cmd, "lcd", 3) == 0) {
    // lcd <row> <col> <text>
    // 例如: lcd 0 0 Hello
    int row, col;
    char text[64];
    if (sscanf(cmd, "%*s %d %d %63[^\n]", &row, &col, text) == 3) {
      lcdWrite(row, col, text);
    } else {
      Serial.println("错误: 格式不正确");
      Serial.println("用法: lcd <row 0-1> <col 0-15> <text>");
      Serial.println("示例: lcd 0 0 Hello World");
    }

  } else if (strncmp(cmd, "lcd clear", 9) == 0 || strncmp(cmd, "lcd cls", 7) == 0) {
    lcd.clear();
    Serial.println("LCD 已清空");

  } else if (strncmp(cmd, "lcd bl on", 9) == 0) {
    lcdBacklight(true);
    Serial.println("LCD 背光已开启");

  } else if (strncmp(cmd, "lcd bl off", 10) == 0) {
    lcdBacklight(false);
    Serial.println("LCD 背光已关闭");

  } else if (strncmp(cmd, "lcd anim", 8) == 0) {
    Serial.println("播放宠物表情动画...");
    showCatAnimation();

  } else if (strncmp(cmd, "i2c dump", 8) == 0) {
    // i2c dump <addr> <reg> <count> - 以十六进制格式打印寄存器
    byte addr;
    byte reg;
    int count;
    if (sscanf(cmd, "%*s %*s 0x%hhx 0x%hhx %d", &addr, &reg, &count) == 3 ||
        sscanf(cmd, "%*s %*s %hhu %hhu %d", &addr, &reg, &count) == 3) {
      Serial.printf("转储设备 0x%02X 从寄存器 0x%02X 读取 %d 字节:\n", addr, reg, count);
      Wire.beginTransmission(addr);
      Wire.write(reg);
      byte err = Wire.endTransmission();
      if (err == 0) {
        int bytesRead = Wire.requestFrom(addr, (byte)count);
        if (bytesRead > 0) {
          for (int i = 0; i < bytesRead; i++) {
            byte val = Wire.read();
            Serial.printf("0x%02X ", val);
            if ((i + 1) % 16 == 0) Serial.println();
          }
          Serial.println();
        } else {
          Serial.println("错误: 读取失败");
        }
      } else {
        Serial.printf("错误: 设备无响应 (error=%d)\n", err);
      }
    } else {
      Serial.println("用法: i2c dump <addr> <reg> <count>");
      Serial.println("示例: i2c dump 0x27 0x00 16");
    }

  } else {
    Serial.printf("未知命令: %s\n", cmd);
    Serial.println("输入 help 查看可用命令");
  }

  // 所有 I2C 操作后恢复 LCD
  lcdRestore();
}

// 恢复 LCD 显示和背光
void lcdRestore() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32-S3 IIC");
  lcd.setCursor(0, 1);
  lcd.print("Console Ready");
  lcd.backlight();
}

// 打印帮助信息
void printHelp() {
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
  Serial.println("【示例】");
  Serial.println("  i2c write 0x27 0x00 0xFF    - 向 0x27 写寄存器 0x00 = 0xFF");
  Serial.println("  i2c read 0x27 0x00 8       - 从 0x27 读 8 字节");
  Serial.println("  lcd 0 0 Hello World     - 第 1 行显示 Hello World");
  Serial.println("  lcd 1 0 I2C Console     - 第 2 行显示 I2C Console");
  Serial.println();
}

// I2C 写寄存器
void i2cWrite(byte addr, byte reg, byte value) {
  Serial.printf("I2C 写: 地址 0x%02X, 寄存器 0x%02X, 值 0x%02X\n", addr, reg, value);

  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  byte err = Wire.endTransmission();

  if (err == 0) {
    Serial.println("写入成功");
  } else {
    Serial.printf("写入失败 (error=%d)\n", err);
  }
}

// I2C 读寄存器
void i2cRead(byte addr, byte reg, int count) {
  Serial.printf("I2C 读: 地址 0x%02X, 寄存器 0x%02X, 长度 %d\n", addr, reg, count);

  Wire.beginTransmission(addr);
  Wire.write(reg);
  byte err = Wire.endTransmission();

  if (err == 0) {
    int bytesRead = Wire.requestFrom(addr, (byte)count);
    if (bytesRead > 0) {
      Serial.print("读取到 ");
      Serial.print(bytesRead);
      Serial.println(" 字节:");
      for (int i = 0; i < bytesRead; i++) {
        byte val = Wire.read();
        Serial.printf("  [%d] 0x%02X (%d)\n", i, val, val);
      }
    } else {
      Serial.println("读取失败: 无数据");
    }
  } else {
    Serial.printf("读取失败 (error=%d)\n", err);
  }
}

// LCD 写文字
void lcdWrite(int row, int col, const char* text) {
  if (row < 0 || row > 1 || col < 0 || col > 15) {
    Serial.println("错误: 行列超出范围 (row: 0-1, col: 0-15)");
    return;
  }
  lcd.setCursor(col, row);
  lcd.print(text);
  Serial.printf("LCD 写入: 行 %d, 列 %d -> %s\n", row, col, text);
}

// LCD 背光控制
void lcdBacklight(bool on) {
  if (on) {
    lcd.backlight();
  } else {
    lcd.noBacklight();
  }
}

// 扫描 I2C 设备
void scanI2CDevices() {
  byte deviceCount = 0;

  Serial.println("扫描 I2C 地址 (0x03 - 0x77):");

  for (byte addr = 0x03; addr <= 0x77; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.printf("  找到设备: 0x%02X\n", addr);
      deviceCount++;
    }
  }

  if (deviceCount == 0) {
    Serial.println("未找到任何 I2C 设备");
  } else {
    Serial.printf("共找到 %d 个 I2C 设备\n", deviceCount);
  }
}

// 播放猫咪表情动画
void showCatAnimation() {
  lcd.clear();
  for (int i = 0; i < LCD_COLS; i++) lcd.write((byte)0);
  lcd.setCursor(0, 1);
  lcd.print("Happy!          ");
  Serial.println("Show: Happy Cat");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Sleepy... Zzz   ");
  for (int i = 3; i < LCD_COLS; i++) lcd.write((byte)1);
  Serial.println("Show: Sleepy Cat");
  delay(2000);

  lcd.clear();
  for (int i = 0; i < LCD_COLS; i++) lcd.write((byte)2);
  lcd.setCursor(0, 1);
  lcd.print("Surprised!      ");
  Serial.println("Show: Surprised Cat");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write((byte)3);
  lcd.print(" LOVE! ");
  lcd.write((byte)3);
  lcd.setCursor(0, 1);
  lcd.print("  I Love U  ~     ");
  Serial.println("Show: Love Cat");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pet Dinner:");
  for (int i = 0; i < 5; i++) lcd.write((byte)4);
  lcd.setCursor(0, 1);
  for (int i = 0; i < 5; i++) lcd.write((byte)5);
  Serial.println("Show: Pet Dinner");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Walking: ");
  for (int i = 0; i < 7; i++) lcd.write((byte)6);
  lcd.setCursor(0, 1);
  lcd.print("Meow Meow~     ");
  Serial.println("Show: Cat Walk");
  delay(1000);

  // 恢复到控制台状态
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32-S3 IIC");
  lcd.setCursor(0, 1);
  lcd.print("Console Ready");
}
