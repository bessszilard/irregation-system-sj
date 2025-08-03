#pragma once
// // publish
#include <Arduino.h>

class MqttTopics
{
public:
    MqttTopics(){};
    static String GetShortDeviceId();

    static String RootWithId();
    struct Subscription
    {
        static const char* GET_ALL_INFO;
        static const char* CMD_ADD;
        static const char* CMD_REMOVE;
        static const char* CMDS_SAVE_ALL;
        static const char* CMDS_LOAD_ALL;
        static const char* CMDS_RESET_TO_DEFAULT;
        static const char* CMD_OVERRIDE;
        static const char* CMD_IMPORT;
        static const char* CMD_GET_OPTIONS;
        static const char* RELAY_GROUPS_SET;
        static const char* RELAY_GROUPS_LOAD;
    };

    struct Publish
    {
        static const char* SENSORS;
        static const char* RELAYS;
        static const char* LOCAL_TIME;
        static const char* CMD_LIST;
        static const char* CMD_RESPONSE;
        static const char* CMD_OPTIONS;
        static const char* RELAY_GROUPS_INFO;
    };

    const Subscription& sub() const
    {
        return m_sub;
    }

    const Publish& pub() const
    {
        return m_publish;
    }

private:
    Subscription m_sub;
    Publish m_publish;
};