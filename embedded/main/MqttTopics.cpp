#pragma once

#include "MqttTopics.hpp"

String MqttTopics::GetShortDeviceId()
{
    uint64_t mac  = ESP.getEfuseMac();
    uint32_t hash = 0;
    for (int i = 0; i < 6; i++)
    {
        hash ^= (mac >> (8 * i)) & 0xFF;
        hash = (hash << 5) | (hash >> 27);
    }

    char id[5];
    for (int i = 0; i < 4; i++)
    {
        id[i] = "0123456789abcdefghijklmnopqrstuvwxyz"[hash % 36];
        hash /= 36;
    }
    id[4] = '\0';
    return String(id); // e.g., "e1sa"
}

String MqttTopics::RootWithId()
{
    return String("rai/") + MqttTopics::GetShortDeviceId() + "/";
}

// Keep the Strings alive in this file scope
static String rootId = MqttTopics::RootWithId();

static String _GET_ALL_INFO          = rootId + "get-all-info";
static String _CMD_ADD               = rootId + "cmd/add";
static String _CMD_REMOVE            = rootId + "cmd/remove";
static String _CMDS_SAVE_ALL         = rootId + "cmd/save_all";
static String _CMDS_LOAD_ALL         = rootId + "cmd/load_all";
static String _CMDS_RESET_TO_DEFAULT = rootId + "cmd/reset_cmd_to_def";
static String _CMD_OVERRIDE          = rootId + "cmd/override";
static String _CMD_IMPORT            = rootId + "cmd/import";
static String _CMD_GET_OPTIONS       = rootId + "cmd/command-options/get";
static String _RELAY_GROUPS_SET      = rootId + "relay-groups/set";
static String _RELAY_GROUPS_LOAD     = rootId + "relay-groups/load";

const char* MqttTopics::Subscription::GET_ALL_INFO          = _GET_ALL_INFO.c_str();
const char* MqttTopics::Subscription::CMD_ADD               = _CMD_ADD.c_str();
const char* MqttTopics::Subscription::CMD_REMOVE            = _CMD_REMOVE.c_str();
const char* MqttTopics::Subscription::CMDS_SAVE_ALL         = _CMDS_SAVE_ALL.c_str();
const char* MqttTopics::Subscription::CMDS_LOAD_ALL         = _CMDS_LOAD_ALL.c_str();
const char* MqttTopics::Subscription::CMDS_RESET_TO_DEFAULT = _CMDS_RESET_TO_DEFAULT.c_str();
const char* MqttTopics::Subscription::CMD_OVERRIDE          = _CMD_OVERRIDE.c_str();
const char* MqttTopics::Subscription::CMD_IMPORT            = _CMD_IMPORT.c_str();
const char* MqttTopics::Subscription::CMD_GET_OPTIONS       = _CMD_GET_OPTIONS.c_str();
const char* MqttTopics::Subscription::RELAY_GROUPS_SET      = _RELAY_GROUPS_SET.c_str();
const char* MqttTopics::Subscription::RELAY_GROUPS_LOAD     = _RELAY_GROUPS_LOAD.c_str();

// Repeat same structure for Publish
static String _SENSORS           = rootId + "sensors";
static String _RELAYS            = rootId + "relays";
static String _LOCAL_TIME        = rootId + "localTime";
static String _CMD_LIST          = rootId + "cmd/list";
static String _CMD_RESPONSE      = rootId + "cmd/response";
static String _CMD_OPTIONS       = rootId + "cmd/command-options";
static String _RELAY_GROUPS_INFO = rootId + "relay-groups/info";

const char* MqttTopics::Publish::SENSORS           = _SENSORS.c_str();
const char* MqttTopics::Publish::RELAYS            = _RELAYS.c_str();
const char* MqttTopics::Publish::LOCAL_TIME        = _LOCAL_TIME.c_str();
const char* MqttTopics::Publish::CMD_LIST          = _CMD_LIST.c_str();
const char* MqttTopics::Publish::CMD_RESPONSE      = _CMD_RESPONSE.c_str();
const char* MqttTopics::Publish::CMD_OPTIONS       = _CMD_OPTIONS.c_str();
const char* MqttTopics::Publish::RELAY_GROUPS_INFO = _RELAY_GROUPS_INFO.c_str();
