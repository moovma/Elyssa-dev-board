#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

//
// Naming rule: header pins follow the PCB silkscreen (IO0..IO6 = GPIO1..GPIO7).
//              SDA/SCL keep their GPIO number (A8/A9, T8/T9 = GPIO8/GPIO9).
//

//
// USB identity - used ONLY in "USB-OTG (TinyUSB)" mode.
// In "Hardware CDC and JTAG" mode the identity is fixed in silicon (303A:1001).
// USB_PID 0x81FF is TEMPORARY: replace with the PID assigned by Espressif
// and keep it identical to upload_port.0.pid in boards.txt
// 
#define USB_VID          0x303A
#define USB_PID          0x81FF
#define USB_MANUFACTURER "Moovma"
#define USB_PRODUCT      "Elyssa"

//
// Elyssa board functions (implemented in variant.cpp)
//
#ifdef __cplusplus
bool hello_elyssa(uint32_t wait_ms = 3000);
bool    elyssa_imu_begin();              // start the IMU (call first)
bool    elyssa_imu_ready();              // true when a new sample is ready

// IMU reading: acceleration in g, rotation in degrees per second, temperature in C
float elyssa_imu_accel_x();
float elyssa_imu_accel_y();
float elyssa_imu_accel_z();
float elyssa_imu_gyro_x();
float elyssa_imu_gyro_y();
float elyssa_imu_gyro_z();
float elyssa_imu_temperature();
bool  elyssa_imu_read_accel(float &x, float &y, float &z);   // 3 axes, same sample
bool  elyssa_imu_read_gyro(float &x, float &y, float &z);    // 3 axes, same sample

// Low-level, and older names kept for compatibility (same values as above)
uint8_t elyssa_imu_whoami();             // 0x6A
uint8_t elyssa_imu_read_reg(uint8_t reg);
float gyro_return_ax();                  // = elyssa_imu_gyro_x()
float gyro_return_ay();
float gyro_return_az();
float accel_return_ax();                 // = elyssa_imu_accel_x()
float accel_return_ay();
float accel_return_az();
bool  elyssa_imu_read(float gyro[3], float accel[3], float *temp_c);  // gyro FIRST

// IMU settings
bool     elyssa_imu_set_accel_range(uint8_t g);    // 2, 4, 8, 16
uint8_t  elyssa_imu_get_accel_range();
bool     elyssa_imu_set_gyro_range(uint16_t dps);  // 125, 250, 500, 1000, 2000
uint16_t elyssa_imu_get_gyro_range();
bool     elyssa_imu_set_rate(float hz);            // 12.5 .. 6660 Hz, both sensors
float    elyssa_imu_get_rate();
bool     elyssa_imu_low_power(bool on);

// IMU angles and orientation (from gravity)
enum ElyssaOrientation : uint8_t {
  ELYSSA_ORIENTATION_UNKNOWN,   // between two sides
  ELYSSA_FACE_UP,               // +Z up (board flat, components up)
  ELYSSA_FACE_DOWN,
  ELYSSA_X_UP,
  ELYSSA_X_DOWN,
  ELYSSA_Y_UP,
  ELYSSA_Y_DOWN
};
float elyssa_imu_pitch();                          // degrees
float elyssa_imu_roll();                           // degrees
ElyssaOrientation elyssa_imu_orientation();

// IMU calibration (board still)
bool elyssa_imu_calibrate_gyro(uint16_t ms = 1000);

// IMU events: each check returns true ONCE per event
// Tap can't be combined with free-fall / motion in the same sketch
// (one chip setting: tap needs events not latched, the others latched).
bool elyssa_imu_enable_tap();                      // sets 416 Hz if lower; check often
bool elyssa_imu_tapped();
bool elyssa_imu_double_tapped();
bool elyssa_imu_enable_freefall();
bool elyssa_imu_fell();
bool elyssa_imu_enable_motion(uint8_t sensitivity = 2);  // 1 = most sensitive .. 63
bool elyssa_imu_moved();

// IMU step counter (sets 26 Hz if lower)
bool     elyssa_imu_start_steps();
uint16_t elyssa_imu_steps();
bool     elyssa_imu_reset_steps();

// IMU wake-up from deep sleep: call just before esp_deep_sleep_start()
bool elyssa_imu_wake_on_motion(uint8_t sensitivity = 2);

// RGB LED colours as a bit mask: bit0 = red, bit1 = green, bit2 = blue
enum ElyssaColor : uint8_t {
  ELYSSA_OFF     = 0,
  ELYSSA_RED     = 1,
  ELYSSA_GREEN   = 2,
  ELYSSA_YELLOW  = 3,   // red + green
  ELYSSA_BLUE    = 4,
  ELYSSA_MAGENTA = 5,   // red + blue
  ELYSSA_CYAN    = 6,   // green + blue
  ELYSSA_WHITE   = 7    // red + green + blue
};

void setLedColor(ElyssaColor color);              // named colours (on/off)
void setLedRGB(uint8_t r, uint8_t g, uint8_t b);  // any mix, 0..255 each (PWM)
#endif

//
// IOs (header J12, names = silkscreen)
//
static const uint8_t IO0 = 1;
static const uint8_t IO1 = 2;
static const uint8_t IO2 = 3;
static const uint8_t IO3 = 4;
static const uint8_t IO4 = 5;
static const uint8_t IO5 = 6;
static const uint8_t IO6 = 7;   // shared with BAT_SENSE when JP3 is bridged

//
// Board functions
//
static const uint8_t BOOT_BUTTON = 0;      // BOOT button, LOW when pressed
#define BUTTON_BUILTIN BOOT_BUTTON         // previous name, kept for compatibility

// Battery voltage (VBAT/2). Works ONLY if solder jumper JP3 is bridged
// (open by default). When JP3 is bridged, do NOT use IO6 for anything else.
static const uint8_t BAT_SENSE      = 7;

// Battery charger BQ24092: no GPIO connection (CHG drives the red charge LED only)

//
// UART (header J12 pins 2/3)
//
static const uint8_t TX = 43;
static const uint8_t RX = 44;

//
// IMU (LSM6DS3TR-C, dedicated I2C bus, SA0 = GND -> address 0x6A, WHO_AM_I = 0x6A)
//
static const uint8_t SDA_gyro = 17;
static const uint8_t SCL_gyro = 18;
static const uint8_t INT_gyro = 21;
#define GYRO_ADDR 0x6A

// Second I2C bus (Wire1) = IMU bus: Wire1.begin() uses these pins by default
static const uint8_t SDA1 = SDA_gyro;
static const uint8_t SCL1 = SCL_gyro;
#define WIRE1_PIN_DEFINED 1

//
// I2C (header J12 + Qwiic J11, 4.7k pull-ups on board)
//
static const uint8_t SDA = 8;
static const uint8_t SCL = 9;

//
// SPI (default bus = header J12)
// Internal flash pins are NOT exposed
//
static const uint8_t SS   = 34;
static const uint8_t MOSI = 35;
static const uint8_t MISO = 37;
static const uint8_t SCK  = 36;
static const uint8_t CS   = SS;    // name printed on the board
static const uint8_t CLK  = SCK;   // name printed on the board

//
// SPI (microSD, separate bus)
//
static const uint8_t SD_CS   = 10;
static const uint8_t SD_MOSI = 11;
static const uint8_t SD_SCK  = 12;
static const uint8_t SD_MISO = 13;

//
// Analog (A0..A6 = header IO0..IO6 = GPIO1..GPIO7, A8/A9 = SDA/SCL = GPIO8/GPIO9)
// There is no A7.
// All on ADC1: usable while Wi-Fi is on
//
static const uint8_t A0 = 1;
static const uint8_t A1 = 2;
static const uint8_t A2 = 3;
static const uint8_t A3 = 4;
static const uint8_t A4 = 5;
static const uint8_t A5 = 6;
static const uint8_t A6 = 7;
#define A8 SDA
#define A9 SCL

//
// Touch (T0..T6 = header IO0..IO6 = GPIO1..GPIO7, T8/T9 = SDA/SCL = GPIO8/GPIO9,
//        T10 = PWM1 = GPIO14)
// There is no T7. Avoid T8/T9 for touch when I2C is used (4.7k pull-ups).
// T10 and PWM1 are the same pin: use it for touch OR for PWM.
//
static const uint8_t T0 = 1;
static const uint8_t T1 = 2;
static const uint8_t T2 = 3;
static const uint8_t T3 = 4;
static const uint8_t T4 = 5;
static const uint8_t T5 = 6;
static const uint8_t T6 = 7;
static const uint8_t T8 = 8;
static const uint8_t T9 = 9;
static const uint8_t T10 = 14;

//
// PWM (names = silkscreen; nets PWM2/PWM3 are crossed on J12)
//
static const uint8_t PWM1 = 14;
static const uint8_t PWM2 = 47;   // header pin 16, printed "PWM2"
static const uint8_t PWM3 = 48;   // header pin 17, printed "PWM3"

//
// LEDs (active HIGH)
// RGB LED (Elyssa v5): 3 separate channels, red = 39, green = 38, blue = 33.
// Colour helpers setLedColor() / setLedRGB() are declared above (variant.cpp).
// LED_BUILTIN stays on 38 = green, so Blink keeps working.
//
static const uint8_t LED_RED   = 39;
static const uint8_t LED_GREEN = 38;
static const uint8_t LED_BLUE  = 33;

static const uint8_t LED_BUILTIN = 38;
#define LED_BUILTIN LED_BUILTIN    // allow testing #ifdef LED_BUILTIN

#define BUILTIN_LED LED_BUILTIN    // backward compatibility
#define STATUS_LED  LED_BUILTIN
#define USER_LED    LED_BUILTIN
#define AMBOUBA     LED_BUILTIN
#define ONBOARD_LED LED_BUILTIN

//
// Serial aliases
//
#define SERIAL0_RX RX
#define SERIAL0_TX TX

//
// Wire aliases
//
#define PIN_WIRE_SDA SDA
#define PIN_WIRE_SCL SCL

//
// SPI aliases
//
#define PIN_SPI_SS   SS
#define PIN_SPI_MOSI MOSI
#define PIN_SPI_MISO MISO
#define PIN_SPI_SCK  SCK

#endif /* Pins_Arduino_h */