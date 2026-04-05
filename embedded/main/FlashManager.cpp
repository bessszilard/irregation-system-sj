#include "FlashManager.hpp"

// NVS namespace and key name limits: 15 characters each.
// Namespaces used:
//   "wifi"   — WiFi credentials
//   "mqtt"   — MQTT credentials
//   "cmds"   — command list
//   "groups" — relay group bitmasks

FlashManager::FlashManager() {}

bool FlashManager::begin()
{
    Serial.println("FlashManager: initialized (NVS/Preferences)");
    return true;
}

bool FlashManager::saveCommands(const String& cmdList)
{
    if (!m_prefs.begin("cmds", false))
    {
        Serial.println("FlashManager: failed to open 'cmds' namespace");
        return false;
    }
    m_prefs.putString("list", cmdList);
    m_prefs.end();
    Serial.printf("FlashManager: commands saved (%d bytes)\n", cmdList.length());
    return true;
}

bool FlashManager::loadCommands(String& cmdList)
{
    if (!m_prefs.begin("cmds", true))
    {
        Serial.println("FlashManager: failed to open 'cmds' namespace (read-only)");
        return false;
    }
    if (!m_prefs.isKey("list"))
    {
        Serial.println("FlashManager: no commands stored in flash");
        m_prefs.end();
        return false;
    }
    cmdList = m_prefs.getString("list", "");
    m_prefs.end();
    if (cmdList.isEmpty())
    {
        Serial.println("FlashManager: commands key is empty");
        return false;
    }
    Serial.printf("FlashManager: commands loaded (%d bytes)\n", cmdList.length());
    return true;
}

bool FlashManager::saveWifiConfig(const String& ssid, const String& password)
{
    if (!m_prefs.begin("wifi", false))
    {
        Serial.println("FlashManager: failed to open 'wifi' namespace");
        return false;
    }
    m_prefs.putString("ssid", ssid);
    m_prefs.putString("pass", password);
    m_prefs.end();
    Serial.printf("FlashManager: WiFi config saved (ssid=%s)\n", ssid.c_str());
    return true;
}

bool FlashManager::loadWifiConfig(String& ssid, String& password)
{
    if (!m_prefs.begin("wifi", true))
    {
        Serial.println("FlashManager: failed to open 'wifi' namespace (read-only)");
        return false;
    }
    if (!m_prefs.isKey("ssid"))
    {
        Serial.println("FlashManager: no WiFi config in flash");
        m_prefs.end();
        return false;
    }
    ssid     = m_prefs.getString("ssid", "");
    password = m_prefs.getString("pass", "");
    m_prefs.end();
    if (ssid.isEmpty())
    {
        Serial.println("FlashManager: WiFi SSID is empty");
        return false;
    }
    Serial.printf("FlashManager: WiFi config loaded (ssid=%s)\n", ssid.c_str());
    return true;
}

bool FlashManager::saveMqttConfig(const String& server, uint16_t port, const String& password)
{
    if (!m_prefs.begin("mqtt", false))
    {
        Serial.println("FlashManager: failed to open 'mqtt' namespace");
        return false;
    }
    m_prefs.putString("server", server);
    m_prefs.putUShort("port", port);
    m_prefs.putString("pass", password);
    m_prefs.end();
    Serial.printf("FlashManager: MQTT config saved (server=%s port=%d)\n", server.c_str(), port);
    return true;
}

bool FlashManager::loadMqttConfig(String& server, uint16_t& port, String& password)
{
    if (!m_prefs.begin("mqtt", true))
    {
        Serial.println("FlashManager: failed to open 'mqtt' namespace (read-only)");
        return false;
    }
    if (!m_prefs.isKey("server"))
    {
        Serial.println("FlashManager: no MQTT config in flash");
        m_prefs.end();
        return false;
    }
    server   = m_prefs.getString("server", "");
    port     = m_prefs.getUShort("port", 1883);
    password = m_prefs.getString("pass", "");
    m_prefs.end();
    if (server.isEmpty())
    {
        Serial.println("FlashManager: MQTT server is empty");
        return false;
    }
    Serial.printf("FlashManager: MQTT config loaded (server=%s port=%d)\n", server.c_str(), port);
    return true;
}

bool FlashManager::saveRelayGroups(uint16_t* p_data, uint16_t p_length)
{
    if (!m_prefs.begin("groups", false))
    {
        Serial.println("FlashManager: failed to open 'groups' namespace");
        return false;
    }
    size_t written = m_prefs.putBytes("data", p_data, p_length * sizeof(uint16_t));
    m_prefs.end();
    if (written != p_length * sizeof(uint16_t))
    {
        Serial.printf("FlashManager: relay groups write mismatch (%d/%d bytes)\n",
                      written,
                      p_length * sizeof(uint16_t));
        return false;
    }
    Serial.printf("FlashManager: relay groups saved (%d groups)\n", p_length);
    return true;
}

bool FlashManager::loadRelayGroups(uint16_t* p_data, uint16_t p_length)
{
    if (!m_prefs.begin("groups", true))
    {
        Serial.println("FlashManager: failed to open 'groups' namespace (read-only)");
        return false;
    }
    if (!m_prefs.isKey("data"))
    {
        Serial.println("FlashManager: no relay groups in flash");
        m_prefs.end();
        return false;
    }
    size_t bytesRead = m_prefs.getBytes("data", p_data, p_length * sizeof(uint16_t));
    m_prefs.end();
    if (bytesRead != p_length * sizeof(uint16_t))
    {
        Serial.printf("FlashManager: relay groups read mismatch (%d/%d bytes)\n",
                      bytesRead,
                      p_length * sizeof(uint16_t));
        return false;
    }
    Serial.printf("FlashManager: relay groups loaded (%d groups)\n", p_length);
    return true;
}
