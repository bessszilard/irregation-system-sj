#pragma once
#include <Preferences.h>

// FlashManager — stores the same data as FramManager but uses ESP32 NVS flash
// (via the Preferences library). Both backends can be active simultaneously.
class FlashManager {
public:
    FlashManager();
    bool begin();

    bool saveCommands(const String& cmdList);
    bool loadCommands(String& cmdList);

    bool saveWifiConfig(const String& ssid, const String& password);
    bool loadWifiConfig(String& ssid, String& password);

    bool saveMqttConfig(const String& server, uint16_t port, const String& password);
    bool loadMqttConfig(String& server, uint16_t& port, String& password);

    bool saveRelayGroups(uint16_t* p_data, uint16_t p_length);
    bool loadRelayGroups(uint16_t* p_data, uint16_t p_length);

private:
    Preferences m_prefs;
};
