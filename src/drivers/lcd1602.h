#ifndef LCD1602_H
#define LCD1602_H

#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <Wire.h>

// LCD1602 默认配置
#ifndef LCD_ADDR
#define LCD_ADDR 0x27
#endif

#ifndef LCD_COLS
#define LCD_COLS 16
#endif

#ifndef LCD_ROWS
#define LCD_ROWS 2
#endif

#ifndef LCD_SDA
#define LCD_SDA 44
#endif

#ifndef LCD_SCL
#define LCD_SCL 43
#endif

// 自定义字符索引
enum CustomChar {
    CHAR_CAT_HAPPY = 0,
    CHAR_CAT_SLEEP,
    CHAR_CAT_SURPRISED,
    CHAR_CAT_LOVE,
    CHAR_FISH,
    CHAR_BONE,
    CHAR_PAW
};

class LCD1602 {
public:
    static LCD1602& getInstance() {
        static LCD1602 instance;
        return instance;
    }

    void begin(uint8_t sda = LCD_SDA, uint8_t scl = LCD_SCL);
    void clear();
    void setCursor(uint8_t col, uint8_t row);
    void print(const char* text);
    void print(const String& text);
    
    void writeText(uint8_t row, uint8_t col, const char* text);
    void backlight(bool on);
    void restoreDefault();
    
    void createCustomChars();
    void showAnimation(uint8_t charIndex, const char* message = nullptr);

private:
    LCD1602();
    LCD1602(const LCD1602&) = delete;
    LCD1602& operator=(const LCD1602&) = delete;

    LiquidCrystal_I2C* lcd;
    TwoWire* _wire;
    bool _initialized = false;
};

#endif // LCD1602_H
