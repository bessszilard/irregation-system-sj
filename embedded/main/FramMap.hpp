
// FRAM MB85RC256V = 32 kByte

// version / date  30 char v20241032-15:36
// WifiPassword    60 char
// wifi ssid name  32 char
// mqtt server usl 60 char

// 012345678901234567890123456789012345678901234567890123456789
// ATime;RXX;Opened;P5;15:00->20:00
// 1  Cmd = max 60 Char / byte
// 50 cmd -> 50*60 = 3000 byte = 3 kByte

#define RELAY_GROUPS_ID_ADDR (100)
#define RELAY_GROUPS_ADDR (102)
#define RELAY_GROUPS_ID (0x1111)

#define CMDS_ID_ADDR (200)
#define CMDS_MEM_LEN_ADDR (202)
#define CMDS_MEM_START_ADDR (204)
// #define CMDS_ID_ADDR 0       // 2 bytes
// #define CMDS_MEM_LEN_ADDR 20  // 2 bytes
// #define CMDS_MEM_START_ADDR 40  // Start writing data here
#define CMDS_ID_NUMBER 0x1234