
#pragma once

/*
* arduinoLoRa Library just only support SX1276/Sx1278,Not support SX1262
* */
// #define LILYGO_TBeam_V0_7
#define LILYGO_TBeam_V1_0
// #define LILYGO_TBeam_V1_1
// #define LILYGO_T3_V1_0
// #define LILYGO_T3_V1_6
// #define LILYGO_T3_V2_0
// #define LILYGO_T95_V1_0

/*
* if you need to change it,
* please open this note and change to the frequency you need to test
* Option: 433E6,470E6,868E6,915E6
* */

#define LoRa_frequency      915E6


#define UNUSE_PIN                   (0)

#if defined(LILYGO_TBeam_V0_7)
#define GPS_RX_PIN                  12
#define GPS_TX_PIN                  15
#define BUTTON_PIN                  39
#define BUTTON_PIN_MASK             GPIO_SEL_39
#define I2C_SDA                     21
#define I2C_SCL                     22

#define RADIO_SCLK_PIN               5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DI0_PIN               26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define BOARD_LED                   14
#define LED_ON                      HIGH
#define LED_OFF                     LOW

#define GPS_BAUD_RATE               9600
#define HAS_GPS
#define HAS_DISPLAY                 //Optional, bring your own board, no OLED !!

#elif defined(LILYGO_TBeam_V1_0) || defined(LILYGO_TBeam_V1_1)

#define TIME_INTERVAL               5000 //Time in miliseconds between samples

#define GPS_RX_PIN                  34
#define GPS_TX_PIN                  12
#define BUTTON_PIN                  38
#define BUTTON_PIN_MASK             GPIO_SEL_38
#define I2C_SDA                     21
#define I2C_SCL                     22
#define I2C_SDA2                    2
#define I2C_SCL2                    13
#define PMU_IRQ                     35

#define RADIO_SCLK_PIN               5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DI0_PIN               26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define BOARD_LED                   4
#define LED_ON                      LOW
#define LED_OFF                     HIGH

#define GPS_BAUD_RATE               9600
#define HAS_GPS
#define HAS_DISPLAY                 //Optional, bring your own board, no OLED !!

#elif defined(LILYGO_T3_V1_0)
#define I2C_SDA                     4
#define I2C_SCL                     15
#define OLED_RST                    16

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DI0_PIN               26
#define RADIO_RST_PIN               14
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define HAS_DISPLAY

#elif defined(LILYGO_T3_V1_6)
#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    UNUSE_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DI0_PIN               26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define SDCARD_MOSI                 15
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13

#define BOARD_LED                   25
#define LED_ON                      HIGH

#define ADC_PIN                     35

#define HAS_SDCARD
#define HAS_DISPLAY

#elif defined(LILYGO_T3_V2_0)
#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    UNUSE_PIN

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DI0_PIN               26
#define RADIO_RST_PIN               14
#define RADIO_DIO1_PIN              UNUSE_PIN
#define RADIO_BUSY_PIN              UNUSE_PIN

#define SDCARD_MOSI                 15
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13

#define BOARD_LED                   0
#define LED_ON                      LOW

#define HAS_DISPLAY
#define HAS_SDCARD

#elif defined(LILYGO_T95_V1_0)

#define I2C_SDA                     21
#define I2C_SCL                     22
#define OLED_RST                    UNUSE_PIN

#define RADIO_SCLK_PIN              18
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              23
#define RADIO_CS_PIN                5
#define RADIO_DI0_PIN               26
#define RADIO_RST_PIN               4
#define RADIO_DIO1_PIN              33
#define RADIO_DIO2_PIN              32
#define RADIO_BUSY_PIN              UNUSE_PIN

#define ADC_PIN                     35
#define HAS_DISPLAY

#else
#error "Please select the version you purchased in utilities.h"
#endif
