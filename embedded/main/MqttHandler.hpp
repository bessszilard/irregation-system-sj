#include <Arduino.h>
#include <PubSubClient.h>

class MqttHandler
{
public:
    MqttHandler(PubSubClient* p_client)
    {
        m_client = p_client;
    }

    bool init(const char* p_domain, uint16_t p_port)
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
    }

    void updateSensorData()
    {
        if (m_client == nullptr)
        {
            Serial.println("Invalid mqtt client");
            return;
        }

        if (m_client->publish("sjirs/humidity", "0.11")) // String(sensorData.humidity).c_str());
        {
            Serial.println("Published to sjirs/humidity");
        }
        else
        {
            Serial.print("Failed to publish");
        }
    }

private:
    PubSubClient* m_client;
};