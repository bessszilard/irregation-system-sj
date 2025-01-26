#include "MqttHandler.hpp"

//---------------------------------------------------------------
MqttHandler::MqttHandler(PubSubClient* p_client)
//---------------------------------------------------------------
{
    m_client = p_client;
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
    m_client->subscribe("esp32/output");
    m_client->subscribe(MQTT_SUB_ADD_CMD);
    m_client->subscribe(MQTT_SUB_REMOVE_CMD);

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
void MqttHandler::publish(const LocalTime& time)
//---------------------------------------------------------------
{
    publish(MQTT_LOCAL_TIME, time.toString());
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
    return m_client->loop();
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
            // Subscribe
            m_client->subscribe("esp32/output");
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
        Serial.print("Published to");
        Serial.println(topic);
    }
    else
    {
        Serial.print("Failed to");
        Serial.println(topic);
    }
}