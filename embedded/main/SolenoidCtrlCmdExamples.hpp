#pragma once

#define CMD_MANUAL_CLOSE_ALL_RELAYS "$Manua;PLW;RXX;C#"
#define CMD_MANUAL_CLOSE_RELAY1 "$Manua;PLW;R01;C#"
#define CMD_MANUAL_OPEN_ALL_RELAYS "$Manua;PLW;RXX;O#"

#define CMD_ATIME_SINGLE "$ATime;P05;R01;S_C07:00->20:00#" // Close between 7 to 20 hour
#define CMD_ATIME_REPEAT "$ATime;P05;R01;R_X06:00->20:00_O01h_C20m#" // Between 6 to 20 hour, open for 1h, close for 20min

