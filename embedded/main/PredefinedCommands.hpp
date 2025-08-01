#pragma once


#define CMD_MANUAL_CLOSE_ALL_RELAYS "$Manual;P00;RXX;C#"
#define CMD_MANUAL_CLOSE_RELAY1 "$Manual;P01;R01;C#"
#define CMD_MANUAL_OPEN_ALL_RELAYS "$Manual;P01;RXX;O#"


// clang-format off
#define CMD_ATIME_SINGLE    "$TimSin;P05;R01;S_C07:00->20:00#"           // Close between 7 to 20 hour
#define CMD_ATIME_REPEAT    "$TimRep;P05;R01;R_X06:00->20:00_O01h_C20m#" // 6 to 20 h, open for 1h, close for 20min
#define CMD_SENS_THRESHOLD  "$SenThr;P05;R01;RAIN_C>040.0#"              // Rain -> close if x > 40
#define CMD_SENS_RANGE      "$SenRan;P05;R01;TESU_O>025.0_C<010.5#"      // Temp Sun open if x > 25*C close if x < 10*C

// If Temp Shadow > 30*C && time is  6 to 20 h open for 1 hour, close for 50 sec
#define CMD_SENS_THR_REPEAT "$SeTTRe;P05;R01;TESH_X>030.0_R_X06:00->20:00_O01h_C20m#"   

#define CMD_ATIME_SH_TRIPLE  "$TimSin;P05;R01;S_C00:00->03:00+O06:00->12:00+C23:00->23:59#"   // Close between 7 to 20 hour


#define CMD_DELIMITER "|"
#define CMD_ADD_NEW_SUB_CMD "+"

const String AllCommandExamples = String(CMD_MANUAL_CLOSE_ALL_RELAYS)            \
                                  + CMD_DELIMITER + CMD_MANUAL_CLOSE_RELAY1      \
                                  + CMD_DELIMITER + CMD_MANUAL_OPEN_ALL_RELAYS   \
                                  + CMD_DELIMITER + CMD_ATIME_SINGLE             \
                                  + CMD_DELIMITER + CMD_SENS_RANGE               \
                                  + CMD_DELIMITER + CMD_SENS_THRESHOLD           \
                                  + CMD_DELIMITER + CMD_SENS_THR_REPEAT          \
                                  + CMD_DELIMITER;
const uint8_t AllCommandExamplesCnt = 7;
// clang-format on


constexpr uint8_t CmdTypeStrLen    = 6;
constexpr uint8_t RelayIdStrLen    = 3;
constexpr uint8_t RelayStateStrLen = 6;
constexpr uint8_t PriorityStateLen = 3;

// action
constexpr uint8_t TimeSingleActionLength = 15; // Length of "S_C07:00->20:00"
constexpr uint8_t TimeRepeatActionLength = 25; // Length of "R_X06:00->20:00_O01h_C20m"

constexpr uint8_t SensorFloatThresholdLength = 7;
