#pragma once

// ============================================================
// Board selection — uncomment exactly one:
// ============================================================
// #define BOARD_ESP32_WROOM   // Classic ESP32 (38-pin WROOM/WROVER)
#define BOARD_ESP32_S3       // ESP32-S3
// ============================================================

#if !defined(BOARD_ESP32_WROOM) && !defined(BOARD_ESP32_S3)
#error "No board defined. Uncomment one of BOARD_ESP32_WROOM or BOARD_ESP32_S3 in Pinout.hpp."
#endif

#if defined(BOARD_ESP32_WROOM) && defined(BOARD_ESP32_S3)
#error "Multiple boards defined. Uncomment exactly one of BOARD_ESP32_WROOM or BOARD_ESP32_S3."
#endif

// ============================================================
// Pinout LUT: ESP32_WROOM  →  ESP32_S3
// ============================================================
//
//  Function              WROOM pin   S3 pin
//  ──────────────────────────────────────────────────────────
//  LED                   GPIO2       BUILTIN
//  RELAY_ARRAY_DATA          32          18
//  RELAY_ARRAY_CLOCK         33           3
//  RELAY_ARRAY_LATCH         25          46
//  TEMPERATURE_DATA           4          47
//  FLOW_METER                23          41
//  I2C SDA                   21           8
//  I2C SCL                   22           9
//  HC12_TXD (ESP TX)         17          20
//  HC12_RXD (ESP RX)         16          19
// ============================================================

#if defined(LED_BUILTIN)
    #define LED_PIN LED_BUILTIN
#elif defined(BOARD_ESP32_WROOM)
    #define LED_PIN (2)
#else  // BOARD_ESP32_S3
    #define LED_PIN (48)
#endif

#ifdef BOARD_ESP32_WROOM
    #define RELAY_ARRAY_DATA  (32)
    #define RELAY_ARRAY_CLOCK (33)
    #define RELAY_ARRAY_LATCH (25)
#else  // BOARD_ESP32_S3
    #define RELAY_ARRAY_DATA  (18)
    #define RELAY_ARRAY_CLOCK (3)
    #define RELAY_ARRAY_LATCH (46)
#endif

#ifdef BOARD_ESP32_WROOM
    #define TEMPERATURE_DATA_PIN (4)
#else  // BOARD_ESP32_S3
    #define TEMPERATURE_DATA_PIN (47)
#endif

#define HUMIDITY_DATA_PIN (5)

#ifdef BOARD_ESP32_WROOM
    #define BAROMETRIC_SDA (21)
    #define BAROMETRIC_SCL (22)
#else  // BOARD_ESP32_S3
    #define BAROMETRIC_SDA (8)
    #define BAROMETRIC_SCL (9)
#endif

#ifdef BOARD_ESP32_WROOM
    #define FLOW_METER_PIN (23)
#else  // BOARD_ESP32_S3
    #define FLOW_METER_PIN (41)
#endif

#ifdef BOARD_ESP32_WROOM
    #define HC12_RXD (16)
    #define HC12_TXD (17)
#else  // BOARD_ESP32_S3
    #define HC12_RXD (19)
    #define HC12_TXD (20)
#endif

// FRAM I2C
#define SDA_2    19
#define SCL_2    18
#define I2C_FREQ 400000
