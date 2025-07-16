// publish
#define MQTT_SENSORS "sjirs/sensors"
#define MQTT_RELAYS "sjirs/relays"
#define MQTT_LOCAL_TIME "sjirs/localTime"
#define MQTT_CMD_LIST "sjirs/cmd/list"
#define MQTT_CMD_RESPONSE "sjirs/cmd/response"
#define MQTT_CMD_OPTIONS "sjirs/cmd/command-options"

// subscribe
// TODOsz move general terms to the front -> CMD_ADD, CMD_REMOVE,
// CMD_GET_OPTIONS etc.
#define MQTT_SUB_ADD_CMD "sjirs/cmd/add"
#define MQTT_SUB_REMOVE_CMD "sjirs/cmd/remove"
#define MQTT_SUB_SAVE_ALL_CMDS "sjirs/cmd/save_all"
#define MQTT_SUB_LOAD_ALL_CMDS "sjirs/cmd/load_all"
#define MQTT_SUB_RESET_CMDS_TO_DEFAULT "sjirs/cmd/reset_cmd_to_def"
#define MQTT_SUB_OVERRIDE_CMD "sjirs/cmd/override"
#define MQTT_SUB_GET_COMMAND_OPTIONS "sjirs/cmd/command-options/get"
#define MQTT_SUB_RELAY_GROUP_ID_ADD "sjirs/relaygroup/add"
#define MQTT_SUB_RELAY_GROUP_ID_REMOVE "sjirs/relaygroup/remove"
#define MQTT_SUB_GET_ALL_INFO "sjirs/get-all-info" // TODOsz move to the front
