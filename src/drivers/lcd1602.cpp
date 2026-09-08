#include "lcd1602.h"

// 独立的 I2C 总线实例（用于 LCD）
TwoWire LCDWire = TwoWire(1);

LCD1602::LCD1602() : lcd(nullptr), _wire(nullptr) {}

// 自定义字符: 宠物表情
static byte catHappy[8] = {
  0b00000, 0b01010, 0b01110, 0b00000,
  0b10001, 0b01110, 0b00100, 0b00000
};
static byte catSleep[8] = {
  0b00000, 0b01010, 0b01110, 0b00000,
  0b00000, 0b01110, 0b00100, 0b00000
};
static byte catSurprised[8] = {
  0b00000, 0b01010, 0b01110, 0b00000,
  0b01010, 0b00000, 0b01110, 0b00000
};
static byte catLove[8] = {
  0b00000, 0b01010, 0b01110, 0b00000,
  0b01010, 0b11111, 0b01110, 0b00100
};
static byte fish[8] = {
  0b00000, 0b00100, 0b01110, 0b11111,
  0b01110, 0b00100, 0b00000, 0b00000
};
static byte bone[8] = {
  0b00000, 0b01110, 0b10001, 0b10001,
  0b01110, 0b00000, 0b00000, 0b00000
};
static byte paw[8] = {
  0b00000, 0b01010, 0b10101, 0b11111,
  0b01110, 0b00100, 0b00000, 0b00000
};

void LCD1602::begin(uint8_t sda, uint8_t scl) {
    Serial.println("\n---LCD1602 初始化 ---");
    Serial.printf("SDA: GPIO%d, SCL: GPIO%d (独立I2C总线)\n", sda, scl);
    
    // 启动独立 I2C 总线
    LCDWire.begin(sda, scl);
    LCDWire.setClock(100000);
    _wire = &LCDWire;
    
    // 动态创建 LCD 实例
    lcd = new LiquidCrystal_I2C(LCD_ADDR, LCD_COLS, LCD_ROWS);
    lcd->init();
    lcd->backlight();
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("ESP32-S3 IIC");
    lcd->setCursor(0, 1);
    lcd->print("Console Ready");
    
    createCustomChars();
    
    _initialized = true;
    Serial.println("LCD1602 初始化完成");
    delay(1000);
}

void LCD1602::createCustomChars() {
    lcd->createChar(CHAR_CAT_HAPPY, catHappy);
    lcd->createChar(CHAR_CAT_SLEEP, catSleep);
    lcd->createChar(CHAR_CAT_SURPRISED, catSurprised);
    lcd->createChar(CHAR_CAT_LOVE, catLove);
    lcd->createChar(CHAR_FISH, fish);
    lcd->createChar(CHAR_BONE, bone);
    lcd->createChar(CHAR_PAW, paw);
    Serial.println("自定义宠物表情已注册");
}

void LCD1602::clear() {
    lcd->clear();
}

void LCD1602::setCursor(uint8_t col, uint8_t row) {
    lcd->setCursor(col, row);
}

void LCD1602::print(const char* text) {
    lcd->print(text);
}

void LCD1602::print(const String& text) {
    lcd->print(text);
}

void LCD1602::writeText(uint8_t row, uint8_t col, const char* text) {
    if (row > 1 || col > 15) {
        Serial.println("错误: 行列超出范围 (row: 0-1, col: 0-15)");
        return;
    }
    lcd->setCursor(col, row);
    lcd->print(text);
    Serial.printf("LCD 写入: 行 %d, 列 %d -> %s\n", row, col, text);
}

void LCD1602::backlight(bool on) {
    if (on) {
        lcd->backlight();
    } else {
        lcd->noBacklight();
    }
}

void LCD1602::restoreDefault() {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("ESP32-S3 IIC");
    lcd->setCursor(0, 1);
    lcd->print("Console Ready");
    lcd->backlight();
}

void LCD1602::showAnimation(uint8_t charIndex, const char* message) {
    lcd->clear();
    for (int i = 0; i < LCD_COLS; i++) lcd->write((byte)charIndex);
    if (message) {
        lcd->setCursor(0, 1);
        lcd->print(message);
    }
}
