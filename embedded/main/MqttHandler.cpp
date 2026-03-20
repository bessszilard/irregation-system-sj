#include "MqttHandler.hpp"

static const unsigned long RECONNECT_INTERVAL_MS = 5000;

//---------------------------------------------------------------
MqttHandler::MqttHandler(PubSubClient* p_client)
    : m_client(p_client)
    , m_btHandler(nullptr)
    , m_lastReconnectAttempt_ms(0)
//---------------------------------------------------------------
{
    if (m_client != nullptr)
    {
        m_client->setBufferSize(4096);
    }
}

//---------------------------------------------------------------
void MqttHandler::setBtHandler(BtHandler* p_btHandler)
//---------------------------------------------------------------
{
    m_btHandler = p_btHandler;
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
    m_client->setSocketTimeout(0);

    return true;
}

//---------------------------------------------------------------
void MqttHandler::publish(const SensorData& sensors)
//---------------------------------------------------------------
{
    publish(m_topics.pub().SENSORS, sensors.toJSON());
}

//---------------------------------------------------------------
void MqttHandler::publish(const RelayArrayStates& states)
//---------------------------------------------------------------
{
    publish(m_topics.pub().RELAYS, states.toString());
}

//---------------------------------------------------------------
void MqttHandler::publishRelayInfo(const String& relayInfo)
//---------------------------------------------------------------
{
    // publish(m_topics.pub().RELAYS, "123456");
    publish(m_topics.pub().RELAYS, relayInfo);
    // publish(m_topics.pub().RELAYS, "7890");
}

//---------------------------------------------------------------
void MqttHandler::publish(const RelayGroupManager& p_relayGroups)
//---------------------------------------------------------------
{
    publish(m_topics.pub().RELAY_GROUPS_INFO, p_relayGroups.toJson());
}

//---------------------------------------------------------------
void MqttHandler::publish(const LocalTime& p_time, uint32_t p_uptime)
//---------------------------------------------------------------
{
    publish(m_topics.pub().LOCAL_TIME,
            "{ \"LocalTime\": \"" + p_time.toString() + "\", \"UpTime\": " + String(p_uptime) + " }");
}

//---------------------------------------------------------------
void MqttHandler::publish(const SolenoidManager& p_solm)
//---------------------------------------------------------------
{
    publish(m_topics.pub().CMD_LIST, p_solm.getCmdListInJson());
}

//---------------------------------------------------------------
void MqttHandler::publish(CommandState p_cmdState)
//---------------------------------------------------------------
{
    publish(m_topics.pub().CMD_RESPONSE, "{ \"Result\": \"" + ToString(p_cmdState) + "\" }");
}

//---------------------------------------------------------------
void MqttHandler::publishCmdOptions(const String& cmdOptions)
//---------------------------------------------------------------
{
    publish(m_topics.pub().CMD_OPTIONS, cmdOptions);
}

//---------------------------------------------------------------
void MqttHandler::publishConfigInfo(const String& wifiSsid,
                                    const String& mqttServer,
                                    uint16_t      mqttPort,
                                    bool          saved)
//---------------------------------------------------------------
{
    String json = "{ \"WifiSsid\": \"" + wifiSsid + "\", \"MqttServer\": \"" + mqttServer +
                  "\", \"MqttPort\": " + String(mqttPort) + ", \"Saved\": " + (saved ? "true" : "false") + " }";
    publish(m_topics.pub().CONFIG_INFO, json);
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
    success &= m_client->subscribe(m_topics.sub().CMD_ADD);
    success &= m_client->subscribe(m_topics.sub().CMD_REMOVE);
    success &= m_client->subscribe(m_topics.sub().CMD_OVERRIDE);
    success &= m_client->subscribe(m_topics.sub().CMD_IMPORT);
    success &= m_client->subscribe(m_topics.sub().CMD_GET_OPTIONS);
    success &= m_client->subscribe(m_topics.sub().CMDS_SAVE_ALL);
    success &= m_client->subscribe(m_topics.sub().CMDS_LOAD_ALL);
    success &= m_client->subscribe(m_topics.sub().CMDS_RESET_TO_DEFAULT);
    success &= m_client->subscribe(m_topics.sub().RELAY_GROUPS_SET);
    success &= m_client->subscribe(m_topics.sub().RELAY_GROUPS_LOAD);
    success &= m_client->subscribe(m_topics.sub().GET_ALL_INFO);
    success &= m_client->subscribe(m_topics.sub().CONFIG_WIFI_SET);
    success &= m_client->subscribe(m_topics.sub().CONFIG_WIFI_GET);
    success &= m_client->subscribe(m_topics.sub().CONFIG_MQTT_SET);
    success &= m_client->subscribe(m_topics.sub().CONFIG_MQTT_GET);
    return success;
}

//---------------------------------------------------------------
void MqttHandler::reconnectMqtt()
//---------------------------------------------------------------
{
    unsigned long now = millis();
    if (now - m_lastReconnectAttempt_ms < RECONNECT_INTERVAL_MS)
    {
        return;
    }
    m_lastReconnectAttempt_ms = now;

    Serial.print("Attempting MQTT connection...");
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
        Serial.println(" will retry in 5 seconds");
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

    if (false == m_client->connected())
    {
        Serial.printf("Warning, Mqtt client isn't connected. topic %s Message %s\n", topic, message.c_str());
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

    if (m_btHandler != nullptr)
    {
        m_btHandler->publish(topic, message);
    }
}