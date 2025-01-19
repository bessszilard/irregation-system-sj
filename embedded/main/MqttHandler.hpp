#include <Arduino.h>
#include <PubSubClient.h>
#include "Structures.hpp"
class MqttHandler
{
public:
    MqttHandler(PubSubClient* p_client);

    bool init(const char* p_domain, uint16_t p_port);

    void publish(const SensorData& sensors);
    void publish(const RelayArrayStates& sensors);

    static void callback(char* topic, byte* message, unsigned int length);

    bool loop();

    void reconnectMqtt();

private:
    PubSubClient* m_client;
};