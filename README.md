# ESP32S3 IIC 上位机

基于 ESP32-S3-N16R8 的 I2C 主机项目，使用 PlatformIO + Arduino 框架。

## 硬件配置

| 项目 | 参数 |
|------|------|
| 开发板 | ESP32-S3-N16R8 (16MB Flash + 8MB PSRAM) |
| 平台 | Espressif32 (PlatformIO) |
| 框架 | Arduino |
| I2C SDA | GPIO8 |
| I2C SCL | GPIO9 |
| I2C 频率 | 100kHz |
| 串口波特率 | 115200 |

### 引脚定义

```
┌─────────────────────────────────┐
│       ESP32-S3-N16R8            │
│                                 │
│  I2C_SDA: GPIO8  ──── SDA      │
│  I2C_SCL: GPIO9  ──── SCL      │
│  GND           ──── GND        │
│  5V / 3V3      ──── VCC        │
│                                 │
│  USB (UART/JTAG)               │
│  COM3 (设备管理器中查看)        │
└─────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│    LCD1602 + PCF8574 I2C 转接板  │
│    地址: 0x27                   │
└─────────────────────────────────┘
```

## 开发环境

### 1. 安装 VS Code

从 [Visual Studio Code](https://code.visualstudio.com/) 下载并安装。

### 2. 安装 PlatformIO IDE

1. 打开 VS Code
2. 点击左侧扩展商店（或 `Ctrl+Shift+X`）
3. 搜索 **PlatformIO IDE** 并安装
4. 安装完成后底部状态栏会出现 PlatformIO 图标

### 3. 连接开发板

1. 使用 **支持数据传输** 的 USB 线连接 ESP32-S3
2. 在设备管理器中查看对应的 COM 端口
   - Windows: `devmgmt.msc` → 端口 (COM 和 LPT)
   - 查找 "USB Serial Device (COMx)" 或 "Silicon Labs CP210x"
3. 在 `platformio.ini` 中配置端口：
   ```ini
   upload_port = COM3
   monitor_port = COM3
   ```

### 4. 项目结构

```
ESP32S3_IIC_HOST_COMPUTER/
├── .gitignore
├── platformio.ini      # 项目配置 (端口、编译参数、依赖库)
├── README.md           # 项目说明
└── src/
    └── main.cpp        # 主程序
```

## 编译与烧录

### VS Code 方式 (推荐)

| 操作 | 方法 |
|------|------|
| 编译 | 点击底部 **PlatformIO: Build** 或 `Ctrl+Alt+B` |
| 烧录 | 点击底部 **PlatformIO: Upload** 或 `Ctrl+Alt+U` |
| 串口监视器 | 点击底部 **PlatformIO: Monitor** 或 `Ctrl+Alt+S` |
| 清理 | `Ctrl+Alt+P` → PlatformIO: Clean |

### 命令行方式

在 VS Code 终端中执行：

```bash
# 编译
platformio run

# 编译并烧录 (需要按住 BOOT 按钮)
platformio run --target upload

# 仅编译不烧录
platformio run --target build

# 串口监视器
platformio device monitor --port COM3 --baud 115200

# 清理构建产物
platformio run --target clean

# 列出可用串口
platformio device list
```

### 烧录注意事项

ESP32-S3 烧录时需要进入**下载模式**：

1. **按住**开发板上的 **BOOT** 按钮不要松开
2. 执行上传命令 (PlatformIO: Upload)
3. 等看到 `Connecting...` 和上传进度条后再松开 BOOT 按钮

如果上传失败提示 `Wrong boot mode detected`，说明没有进入下载模式，请重试上述步骤。

如果提示 `port is busy` 或 `拒绝访问`，说明串口被其他程序占用，请先关闭串口监视器或其他终端。

## I2C 设备支持

### 已支持设备

| 设备 | 地址 | 说明 |
|------|------|------|
| LCD1602 + PCF8574 | 0x27 | 16x2 字符液晶，I2C 转接板 |
| LCD1602 + PCF8574A | 0x3F | 部分转接板使用此地址 |

### I2C 扫描

程序启动时和每 10 秒会自动扫描 I2C 总线上的设备，串口监视器会输出扫描结果：

```
扫描 I2C 地址 (0x03 - 0x77):
  找到设备: 0x27
共找到 1 个 I2C 设备
```

### LCD1602 使用说明

1. **对比度调节**: 转接板上有蓝色可调电阻，用十字螺丝刀旋转调节（通常需要 10 圈以上）
2. **背光**: 转接板上 LED 亮起表示供电正常
3. **自定义字符**: LCD1602 支持最多 8 个 5x8 像素自定义字符，代码中已内置宠物表情图案
4. **显示内容**:
   - 开机显示: `ESP32-S3 IIC` / `Host Computer`
   - 随后循环展示宠物表情动画

### 如果 LCD 不显示

| 现象 | 原因 | 解决 |
|------|------|------|
| 背光亮，无文字 | 对比度太低 | 旋转蓝色电位器 |
| 全黑方块 | 对比度太高 | 反向旋转电位器 |
| 闪烁后无显示 | I2C 地址错误 | 尝试 0x3F 或运行 I2C 扫描 |
| 无任何反应 | 接线错误 | 检查 SDA/SCL/GND/VCC |

## 依赖库

```ini
lib_deps =
    marcoschwartz/LiquidCrystal_I2C@^1.1.4
```

自动安装，无需手动下载。

## 串口输出示例

```
ESP32-S3-N16R8 环境已启动
芯片: ESP32-S3, 核心数: 2
闪存: 16 MB
PSRAM: 8 MB, 可用: 是

--- I2C 初始化 ---
SDA: GPIO8, SCL: GPIO9, 频率: 100000 Hz
I2C 已启动
扫描 I2C 地址 (0x03 - 0x77):
  找到设备: 0x27
共找到 1 个 I2C 设备

--- LCD1602 初始化 ---
LCD1602 初始化完成
自定义宠物表情已注册
```

## 常见问题

### Q: 编译失败，提示文件被占用
A: 关闭串口监视器和其他终端，然后执行 `platformio run --target clean` 再重新编译。

### Q: 上传时提示端口不存在
A: 检查设备管理器中的 COM 端口号，更新 `platformio.ini` 中的 `upload_port`。

### Q: LCD 显示乱码
A: LCD1602 仅支持 ASCII 字符集，不支持中文。如需中文显示，请换用 SSD1306 OLED 屏幕。

### Q: PSRAM 显示 0 MB
A: 检查 `platformio.ini` 中的 PSRAM 配置是否正确 (`board_build.arduino.memory_type = qio_opi`)。
