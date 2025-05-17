#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Structures.hpp"
#include <WiFi.h>

class LcdLayouts
{
public:
    LcdLayouts();

    void init();

    void selectKeyBoardMode(const String& p_firstLine);
    void defaultL(const String& p_mode,
                  int p_pageId,
                  bool p_wireless,
                  bool p_wirelessError,
                  const String& p_buttonText = "");

    void connectingToSSID(const String& p_ssid, bool p_connected = false, int p_attempts = 0);

    void updateDef(wl_status_t p_wifiState,
                   int8_t p_rssi_dBm,
                   bool p_mqttState,
                   const RelayArrayStates& p_relayArrayState);

private:
    void update(const String& firstLine, const String& secondLine);

    LiquidCrystal_I2C m_lcd;
};