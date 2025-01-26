#include <Arduino.h>
#include <PubSubClient.h>
#include "Structures.hpp"
#include "MqttTopics.hpp"

class MqttHandler
{
public:
    MqttHandler(PubSubClient* p_client);

    bool init(const char* p_domain, uint16_t p_port, MQTT_CALLBACK_SIGNATURE);

    void publish(const SensorData& sensors);
    void publish(const RelayArrayStates& sensors);
    void publish(const LocalTime& time);

    bool loop();

    void reconnectMqtt();

private:
    PubSubClient* m_client;

    void publish(const char* topic, const String& message);
};