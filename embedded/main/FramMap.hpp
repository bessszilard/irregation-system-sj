// FRAM MB85RC256V Memory Map - 32 kByte (32768 bytes)
// ============================================================================

// SYSTEM CONFIGURATION SECTION (0-499)
// ----------------------------------------------------------------------------
#define VERSION_DATE_ADDR 0      // 30 bytes - "v20241032-15:36"
#define WIFI_SSID_ADDR 30        // 32 bytes - WiFi network name
#define WIFI_PASSWORD_ADDR 62    // 60 bytes - WiFi password
#define MQTT_SERVER_URL_ADDR 122 // 60 bytes - MQTT broker URL
#define MQTT_PORT_ADDR 182       // 2 bytes  - MQTT port (uint16_t)
#define MQTT_PASSWORD_ADDR 184   // 60 bytes - MQTT broker password
#define WIFI_CONFIG_ID_ADDR 244  // 2 bytes  - WiFi config validation ID
#define WIFI_CONFIG_ID 0x4444    // Validation value for WiFi config
#define MQTT_CONFIG_ID_ADDR 246  // 2 bytes  - MQTT config validation ID
#define MQTT_CONFIG_ID 0x5555    // Validation value for MQTT config

// RELAY NAMES SECTION (500-883)
// ----------------------------------------------------------------------------
#define RELAY_NAMES_ID_ADDR 500    // 2 bytes  - Validation ID
#define RELAY_NAMES_START_ADDR 502 // 384 bytes - 16 relays × 24 chars
#define RELAY_NAMES_ID 0x2222      // Validation value
// Relay names: 16 relays × 24 bytes = 384 bytes

// GROUP NAMES SECTION (900-1093)
// ----------------------------------------------------------------------------
#define GROUP_NAMES_ID_ADDR 900    // 2 bytes  - Validation ID
#define GROUP_NAMES_START_ADDR 902 // 192 bytes - 8 groups × 24 chars
#define GROUP_NAMES_ID 0x3333      // Validation value
// Group names: 8 groups × 24 bytes = 192 bytes

// RELAY GROUPS CONFIGURATION (1100-1105)
// ----------------------------------------------------------------------------
#define RELAY_GROUPS_ID_ADDR 1100 // 2 bytes  - Validation ID
#define RELAY_GROUPS_ADDR 1102    // Variable - Group assignments
#define RELAY_GROUPS_ID 0x1111    // Validation value

// COMMANDS SECTION (1200-4199)
// ----------------------------------------------------------------------------
#define CMDS_ID_ADDR 1200        // 2 bytes  - Validation ID
#define CMDS_MEM_LEN_ADDR 1202   // 2 bytes  - Used memory length
#define CMDS_MEM_START_ADDR 1204 // 3000 bytes - Command storage
#define CMDS_ID_NUMBER                                                                                                                               \
    0x1234 // Validation value
           // Commands: 50 commands × 60 bytes = 3000 bytes
           // Format: "ATime;RXX;Opened;P5;15:00->20:00"

// FREE SPACE (4200-32767)
// ----------------------------------------------------------------------------
// Available: ~28.5 kByte for future expansion

// ============================================================================
// MEMORY USAGE SUMMARY
// ============================================================================
// System Config:        184 bytes  (0-183)
// Relay Names:          386 bytes  (500-885)
// Group Names:          194 bytes  (900-1093)
// Relay Groups:         ~6 bytes   (1100-1105)
// Commands:            3004 bytes  (1200-4203)
// ----------------------------------------
// Total Used:         ~3774 bytes
// Free Space:        ~28994 bytes
// ============================================================================