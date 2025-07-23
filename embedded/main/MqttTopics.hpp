// publish
#define MQTT_SENSORS "sjirs/sensors"
#define MQTT_RELAYS "sjirs/relays"
#define MQTT_LOCAL_TIME "sjirs/localTime"
#define MQTT_CMD_LIST "sjirs/cmd/list"
#define MQTT_CMD_RESPONSE "sjirs/cmd/response"
#define MQTT_CMD_OPTIONS "sjirs/cmd/command-options"
#define MQTT_RELAY_GROUPS_INFO "sjirs/relay-groups/info"

// subscribe
// TODOsz move general terms to the front -> CMD_ADD, CMD_REMOVE,
// CMD_GET_OPTIONS etc.
#define MQTT_SUB_GET_ALL_INFO "sjirs/get-all-info" // TODOsz move to the front
#define MQTT_SUB_CMD_ADD "sjirs/cmd/add"
#define MQTT_SUB_CMD_REMOVE "sjirs/cmd/remove"
#define MQTT_SUB_CMDS_SAVE_ALL "sjirs/cmd/save_all"
#define MQTT_SUB_CMDS_LOAD_ALL "sjirs/cmd/load_all"
#define MQTT_SUB_CMDS_RESET_TO_DEFAULT "sjirs/cmd/reset_cmd_to_def"
#define MQTT_SUB_CMD_OVERRIDE "sjirs/cmd/override"
#define MQTT_SUB_CMD_IMPORT "sjirs/cmd/import"
#define MQTT_SUB_CMD_GET_OPTIONS "sjirs/cmd/command-options/get"
#define MQTT_SUB_GET_ALL_INFO "sjirs/get-all-info"
#define MQTT_RELAY_GROUPS_SET "sjirs/relay-groups/set"
#define MQTT_RELAY_GROUPS_LOAD "sjirs/relay-groups/load"
