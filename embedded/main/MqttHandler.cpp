#include "MqttHandler.hpp"
#include "MqttTopics.hpp"

//---------------------------------------------------------------
MqttHandler::MqttHandler(PubSubClient* p_client)
//---------------------------------------------------------------
{
    m_client = p_client;
}

//---------------------------------------------------------------
bool MqttHandler::init(const char* p_domain, uint16_t p_port)
//---------------------------------------------------------------
{
    if (m_client == nullptr)
    {
        Serial.println("Invalid mqtt client");
        return false;
    }
    m_client->setCallback(callback);
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
    return true;
}

//---------------------------------------------------------------
void MqttHandler::publish(const SensorData& sensors)
//---------------------------------------------------------------
{
    if (m_client == nullptr)
    {
        Serial.println("Invalid mqtt client");
        return;
    }
    if (m_client->publish(MQTT_SENSORS, sensors.toJSON().c_str())) // String(sensorData.humidity).c_str());
    {
        Serial.println("Published to sjirs/humidity");
    }
    else
    {
        Serial.print("Failed to publish");
    }
}

//---------------------------------------------------------------
void MqttHandler::publish(const RelayArrayStates& states)
//---------------------------------------------------------------
{
    if (m_client == nullptr)
    {
        Serial.println("Invalid mqtt client");
        return;
    }

    if (m_client->publish(MQTT_RELAYS, states.toString().c_str()))
    {
        Serial.println("Published to sjirs/humidity");
    }
    else
    {
        Serial.print("Failed to publish relay states");
    }
}

//---------------------------------------------------------------
void MqttHandler::callback(char* topic, byte* message, unsigned int length)
//---------------------------------------------------------------
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    // Feel free to add more if statements to control more GPIOs with MQTT

    // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
    // Changes the output state according to the message
    if (String(topic) == "esp32/output")
    {
        Serial.print("Changing output to ");
        // if (messageTemp == "on")
        // {
        //     Serial.println("on");
        //     digitalWrite(LED_PIN, HIGH);
        // }
        // else if (messageTemp == "off")
        // {
        //     Serial.println("off");
        //     digitalWrite(LED_PIN, LOW);
        // }
    }
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
