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

// 自定义字符: 宠物表情 (每个 5x8 像素)
// 注意: 不能用 const, createChar 需要非 const 指针

byte catHappy[8] = {
  0b00000,
  0b01010,
  0b01110,
  0b00000,
  0b10001,
  0b01110,
  0b00100,
  0b00000
};

byte catSleep[8] = {
  0b00000,
  0b01010,
  0b01110,
  0b00000,
  0b00000,
  0b01110,
  0b00100,
  0b00000
};

byte catSurprised[8] = {
  0b00000,
  0b01010,
  0b01110,
  0b00000,
  0b01010,
  0b00000,
  0b01110,
  0b00000
};

byte catLove[8] = {
  0b00000,
  0b01010,
  0b01110,
  0b00000,
  0b01010,
  0b11111,
  0b01110,
  0b00100
};

byte fish[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

byte bone[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b01110,
  0b00000,
  0b00000,
  0b00000
};

byte paw[8] = {
  0b00000,
  0b01010,
  0b10101,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

// 函数声明
void scanI2CDevices();
void initLCD();
void showPetEmojis();
void showCatAnimation();

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("ESP32-S3-N16R8 宠物表情显示器");
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
  lcd.createChar(0, catHappy);    // 字符 0: 开心猫
  lcd.createChar(1, catSleep);    // 字符 1: 睡觉猫
  lcd.createChar(2, catSurprised);// 字符 2: 惊讶猫
  lcd.createChar(3, catLove);     // 字符 3: 爱心猫
  lcd.createChar(4, fish);        // 字符 4: 鱼
  lcd.createChar(5, bone);        // 字符 5: 骨头
  lcd.createChar(6, paw);         // 字符 6: 猫爪

  Serial.println("自定义宠物表情已注册");
}

void initLCD() {
  Serial.println("\n--- LCD1602 初始化 ---");
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD OK!");
  Serial.println("LCD1602 初始化完成");
  delay(1000);
}

void loop() {
  // 循环展示宠物表情动画
  showCatAnimation();
}

// 展示猫咪表情动画
void showCatAnimation() {

  // 开心
  lcd.clear();
  for (int i = 0; i < LCD_COLS; i++) lcd.write((byte)0);
  lcd.setCursor(0, 1);
  lcd.print("Happy!          ");
  Serial.println("Show: Happy Cat");
  delay(2000);

  // 睡觉 (Zzz)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Sleepy... Zzz   ");
  for (int i = 3; i < LCD_COLS; i++) lcd.write((byte)1);
  Serial.println("Show: Sleepy Cat");
  delay(2000);

  // 惊讶
  lcd.clear();
  for (int i = 0; i < LCD_COLS; i++) lcd.write((byte)2);
  lcd.setCursor(0, 1);
  lcd.print("Surprised!      ");
  Serial.println("Show: Surprised Cat");
  delay(2000);

  // 爱心
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write((byte)3);
  lcd.print(" LOVE! ");
  lcd.write((byte)3);
  lcd.setCursor(0, 1);
  lcd.print("  I Love U  ~     ");
  Serial.println("Show: Love Cat");
  delay(2000);

  // 宠物大餐: 鱼 + 骨头
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pet Dinner:");
  for (int i = 0; i < 5; i++) lcd.write((byte)4);
  lcd.setCursor(0, 1);
  for (int i = 0; i < 5; i++) lcd.write((byte)5);
  Serial.println("Show: Pet Dinner");
  delay(2000);

  // 猫爪排排走
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Walking: ");
  for (int i = 0; i < 7; i++) lcd.write((byte)6);
  lcd.setCursor(0, 1);
  lcd.print("Meow Meow~     ");
  Serial.println("Show: Cat Walk");
  delay(2000);
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
