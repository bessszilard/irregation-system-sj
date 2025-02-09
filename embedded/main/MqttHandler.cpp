#include "MqttHandler.hpp"

//---------------------------------------------------------------
MqttHandler::MqttHandler(PubSubClient* p_client)
//---------------------------------------------------------------
{
    m_client = p_client;
    m_client->setBufferSize(2048);
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
    publish(MQTT_RELAYS, relayInfo);
}

//---------------------------------------------------------------
void MqttHandler::publish(const LocalTime& time)
//---------------------------------------------------------------
{
    publish(MQTT_LOCAL_TIME, "{ \"LocalTime\": \"" + time.toString() + "\" }");
}

//---------------------------------------------------------------
void MqttHandler::publish(const SolenoidManager& solm)
//---------------------------------------------------------------
{
    publish(MQTT_CMD_LIST, solm.getCmdListInJson());
}

//---------------------------------------------------------------
void MqttHandler::publish(CommandState cmdState)
//---------------------------------------------------------------
{
    publish(MQTT_CMD_RESPONSE, ToString(cmdState));
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
    success &= m_client->subscribe(MQTT_SUB_ADD_CMD);
    success &= m_client->subscribe(MQTT_SUB_REMOVE_CMD);
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
        Serial.print("Published to ");
        Serial.println(topic);
        Serial.println(message);
    }
    else
    {
        Serial.print("Publishing failed to ");
        Serial.println(topic);
    }
}