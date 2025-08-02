#include "MqttHandler.hpp"

//---------------------------------------------------------------
MqttHandler::MqttHandler(PubSubClient* p_client)
//---------------------------------------------------------------
{
    m_client = p_client;
    m_client->setBufferSize(4096);
}

//---------------------------------------------------------------
bool MqttHandler::init(const char* p_domain, uint16_t p_port, MQTT_CALLBACK_SIGNATURE)
//---------------------------------------------------------------
{
    if (m_client == nullptr)
    {
        Serial.println("Invalid mqtt client");
        return false;
    }
    char clientID[20];

    m_client->setServer(p_domain, p_port);
    if (m_client->connect("esp32_irrigator"))
    {
        Serial.print("Connection has been established with ");
        Serial.println(p_domain);
    }
    else
    {
        Serial.println("The MQTT server connection failed...");
        return false;
    }

    Serial.println("connected");
    if (false == subscribeTopics())
    {
        Serial.println("Failed to subscribe topics");
    }

    // comes through the macro
    m_client->setCallback(callback);

    return true;
}

//---------------------------------------------------------------
void MqttHandler::publish(const SensorData& sensors)
//---------------------------------------------------------------
{
    publish(MQTT_SENSORS, sensors.toJSON());
}

//---------------------------------------------------------------
void MqttHandler::publish(const RelayArrayStates& states)
//---------------------------------------------------------------
{
    publish(MQTT_RELAYS, states.toString());
}

//---------------------------------------------------------------
void MqttHandler::publishRelayInfo(const String& relayInfo)
//---------------------------------------------------------------
{
    // publish(MQTT_RELAYS, "123456");
    publish(MQTT_RELAYS, relayInfo);
    // publish(MQTT_RELAYS, "7890");
}

//---------------------------------------------------------------
void MqttHandler::publish(const RelayGroupManager& p_relayGroups)
//---------------------------------------------------------------
{
    publish(MQTT_RELAY_GROUPS_INFO, p_relayGroups.toJson());
}

//---------------------------------------------------------------
void MqttHandler::publish(const LocalTime& p_time, uint32_t p_uptime)
//---------------------------------------------------------------
{
    publish(MQTT_LOCAL_TIME, "{ \"LocalTime\": \"" + p_time.toString() + "\", \"UpTime\": " + String(p_uptime) + " }");
}

//---------------------------------------------------------------
void MqttHandler::publish(const SolenoidManager& p_solm)
//---------------------------------------------------------------
{
    publish(MQTT_CMD_LIST, p_solm.getCmdListInJson());
}

//---------------------------------------------------------------
void MqttHandler::publish(CommandState p_cmdState)
//---------------------------------------------------------------
{
    publish(MQTT_CMD_RESPONSE, "{ \"Result\": \"" + ToString(p_cmdState) + "\" }");
}

//---------------------------------------------------------------
void MqttHandler::publishCmdOptions(const String& cmdOptions)
//---------------------------------------------------------------
{
    publish(MQTT_CMD_OPTIONS, cmdOptions);
}

//---------------------------------------------------------------
bool MqttHandler::loop()
//---------------------------------------------------------------
{
    if (m_client == nullptr)
    {
        Serial.println("Invalid mqtt client");
        return false;
    }
    if (false == connected())
    {
        reconnectMqtt();
    }
    return m_client->loop();
}

//---------------------------------------------------------------
bool MqttHandler::connected()
//---------------------------------------------------------------
{
    return m_client->connected();
}

//---------------------------------------------------------------
bool MqttHandler::subscribeTopics()
//---------------------------------------------------------------
{
    if (m_client == nullptr)
    {
        Serial.println("Invalid mqtt client");
        return false;
    }

    bool success = true;
    success &= m_client->subscribe(MQTT_SUB_CMD_ADD);
    success &= m_client->subscribe(MQTT_SUB_CMD_REMOVE);
    success &= m_client->subscribe(MQTT_SUB_CMD_OVERRIDE);
    success &= m_client->subscribe(MQTT_SUB_CMD_IMPORT);
    success &= m_client->subscribe(MQTT_SUB_CMD_GET_OPTIONS);
    success &= m_client->subscribe(MQTT_SUB_CMDS_SAVE_ALL);
    success &= m_client->subscribe(MQTT_SUB_CMDS_LOAD_ALL);
    success &= m_client->subscribe(MQTT_SUB_CMDS_RESET_TO_DEFAULT);
    success &= m_client->subscribe(MQTT_RELAY_GROUPS_SET);
    success &= m_client->subscribe(MQTT_RELAY_GROUPS_LOAD);
    success &= m_client->subscribe(MQTT_SUB_GET_ALL_INFO);
    return success;
}

//---------------------------------------------------------------
void MqttHandler::reconnectMqtt()
//---------------------------------------------------------------
{
    // Loop until we're reconnected
    while (!m_client->connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (m_client->connect("espClient"))
        {
            Serial.println("connected");
            if (false == subscribeTopics())
            {
                Serial.println("Failed to subscribe topics");
            }
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(m_client->state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

//---------------------------------------------------------------
void MqttHandler::publish(const char* topic, const String& message)
//---------------------------------------------------------------
{
    if (m_client == nullptr)
    {
        Serial.println("Invalid mqtt client");
        return;
    }

    if (m_client->publish(topic, message.c_str()))
    {
        Serial.printf("Published to %s %s\n", topic, message.c_str());
    }
    else
    {
        Serial.printf("Publishing failed to %s", topic);
    }
}