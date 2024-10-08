#ifndef __IO_H__
#define __IO_H__
// TinyPico IO Definitions

// UART Communication
#define PIN_SERIAL_1_TX (0u)
#define PIN_SERIAL_1_RX (1u)
#undef  PIN_SERIAL_2_TX 
#undef  PIN_SERIAL_2_RX
#define PIN_SERIAL_2_TX (8u)
#define PIN_SERIAL_2_RX (9u)
#define SerialKbd       Serial1
#define SerialRfm       Serial2
#define SerialClock     Serial2

// I2C PIN Definitions
#define PIN_I2C_SCL     (5u)  
#define PIN_I2C_SDA     (4u)
#define PIN_I2C1_SCL    (7u)  
#define PIN_I2C1_SDA    (6u)

// SPI0 / Micro SD-card
#define SPIO0_MISO      (16u)
#define SPIO0_CS        (17u)
#define SPIO0_SCK       (18u)
#define SPIO0_MOSI      (19u)

// Power Control
#define PIN_EDOG_CLR        (12u)
#define PIN_VEXT_EN         (13u)
#define PIN_V33_EN          (14u)
#define PIN_EXT_CLR         (15u)
//#define PIN_EXT_PWR_OFF (13u)

// RGB Neopixel Output
#define PIN_NEOPIXEL_1  (10u)
#define PIN_NEOPIXEL_2  (11u)

// Audio / PWM
#define PIN_PWM2A           (20u)
#define PIN_PWM2B           (21u)

// Application Specific Definitions

#define PIN_PIR         (22u)
#define PIN_LDR         (26u)


void io_enable_vext(bool enable);

void io_enable_v33(bool enable);

void io_ext_clr(bool do_clear);

void io_feed_watchdog(void);

void io_initialize_tiny_pico(void);


#endif