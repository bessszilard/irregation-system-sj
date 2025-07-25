#pragma once

#define CMD_MANUAL_CLOSE_ALL_RELAYS "$Manual;P00;RXX;C#"
#define CMD_MANUAL_CLOSE_RELAY1 "$Manual;P01;R01;C#"
#define CMD_MANUAL_OPEN_ALL_RELAYS "$Manual;P01;RXX;O#"

// Close between 7 to 20 hour
#define CMD_ATIME_SINGLE "$TimSin;P05;R01;S_C07:00->20:00#"

// Between 6 to 20 hour, open for 1h, close for 20min
#define CMD_ATIME_REPEAT "$TimRep;P05;R01;R_X06:00->20:00_O01h_C20m#"

#define CMD_DELIMITER "|"

constexpr uint8_t CmdTypeStrLen    = 6;
constexpr uint8_t RelayIdStrLen    = 3;
constexpr uint8_t RelayStateStrLen = 6;
constexpr uint8_t PriorityStateLen = 3;

// action
constexpr uint8_t TimeSingleActionLength = 15; // Length of "S_C07:00->20:00"
constexpr uint8_t TimeRepeatActionLength = 25; // Length of "R_X06:00->20:00_O01h_C20m"

const String AllCommandExamples = String(CMD_MANUAL_CLOSE_ALL_RELAYS) + CMD_DELIMITER + CMD_MANUAL_CLOSE_RELAY1 +
                                  CMD_DELIMITER + CMD_MANUAL_OPEN_ALL_RELAYS + CMD_DELIMITER + CMD_ATIME_SINGLE +
                                  CMD_DELIMITER + CMD_ATIME_REPEAT + CMD_DELIMITER;
const uint8_t AllCommandExamplesCnt = 5;