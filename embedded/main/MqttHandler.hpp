#include <Arduino.h>
#include <PubSubClient.h>
#include "Structures.hpp"
#include "MqttTopics.hpp"
#include "SolenoidManager.hpp"

class MqttHandler
{
public:
    MqttHandler(PubSubClient* p_client);

    bool init(const char* p_domain, uint16_t p_port, MQTT_CALLBACK_SIGNATURE);

    void publish(const SensorData& sensors);
    void publish(const RelayArrayStates& sensors);
    void publishRelayInfo(const String& relayInfo);
    void publish(const RelayGroupManager& p_relayGroups);
    void publish(const LocalTime& time);
    void publish(const SolenoidManager& solm);
    void publish(CommandState cmdState);

    void publishCmdOptions(const String& cmdOptions);

    bool loop();
    bool connected();
    bool subscribeTopics();

    void reconnectMqtt();

private:
    PubSubClient* m_client;

    void publish(const char* topic, const String& message);
};