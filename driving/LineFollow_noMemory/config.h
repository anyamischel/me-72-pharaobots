#pragma once
#include <Arduino.h>

// -------------------- Serial rates --------------------
constexpr uint32_t DEBUG_BAUD   = 115200; //for print statements
constexpr uint32_t IBUS_BAUD    = 115200; // for ibus
constexpr uint32_t ROBOCLAW_BAUD = 38400;

// -------------------- RoboClaw --------------------
constexpr uint8_t  ROBOCLAW_ADDRESS = 0x80;

// -------------------- iBus channel mapping --------------------
// NOTE: IBusBM uses 0-based channel indices in your code.
constexpr uint8_t CH_STEER      = 0;  // rcCH1
constexpr uint8_t CH_THROTTLE   = 1;  // rcCH2
constexpr uint8_t CH_ACCEL      = 2;  // rcCH3
constexpr uint8_t CH_AUTO_SW    = 4;  // your AUTO_SWITCH_CH
constexpr uint8_t CH_DRIVE_MODE = 7;  // rcCH8 (drive vs climb)
constexpr uint8_t CH_INTAKE     = 8;  // rcCH9

// -------------------- Intake / TReX servo --------------------
constexpr uint8_t TREC_CMD_PIN = 5;

constexpr int INTAKE_STOP_US   = 1500;
constexpr int INTAKE_MIN_US    = 1000;
constexpr int INTAKE_MAX_US    = 2000;

// -------------------- IR Sensors --------------------
constexpr uint8_t IR_PIN_L = 2;
constexpr uint8_t IR_PIN_C = 3;
constexpr uint8_t IR_PIN_R = 4;
constexpr uint8_t IR_LED_PIN = 13;

// -------------------- Drive behavior --------------------
constexpr int    DRIVE_DEADBAND = 3;

// -------------------- Autonomous behavior --------------------
constexpr unsigned long AUTO_TIMEOUT_MS = 60000; 
constexpr bool AUTO_SWITCH_DEFAULT = false;
