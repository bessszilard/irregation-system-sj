#include "YFG1FlowMeter.hpp"
#include "driver/gpio.h"

static uint16_t s_pulseCount = 0;

void IRAM_ATTR pulseCounter()
{
    s_pulseCount++;
}

YFG1FlowMeter::YFG1FlowMeter(int p_pin, float p_calibrationFactor)
        : m_calibrationFactor{p_calibrationFactor}
        , m_inputPin(p_pin)
        // , m_pulseCoun (0) // static
        , m_oldTime_ms(0)
        , m_data{0.0, 0}
{
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    pinMode(m_inputPin, INPUT_PULLUP);
    attachInterrupt(m_inputPin, pulseCounter, FALLING);
}
void YFG1FlowMeter::resetValues()
{
    s_pulseCount = 0;
    m_oldTime_ms = 0;
    m_data       = Data(0.0, 0);
}

void YFG1FlowMeter::updateFlowData()
{
    if ((millis() - m_oldTime_ms) > 1000) // Only process counters once per second
    {
        detachInterrupt(m_inputPin);
        float flowRate_mLitPerSec = ((1000.0 / (millis() - m_oldTime_ms)) * s_pulseCount) / m_calibrationFactor;
        s_pulseCount              = 0;
        attachInterrupt(m_inputPin, pulseCounter, FALLING);
        m_oldTime_ms = millis();

        m_data.flowRate_LitMin = flowRate_mLitPerSec * 60 / 1000;
        m_data.totalFlow_litres += m_data.flowRate_LitMin;

        // Print the flow rate for this second in litres / minute
        Serial.print("Flow rate [lit/sec]: ");
        Serial.print(flowRate_mLitPerSec); // Print the integer part of the variable
        Serial.print("\t");                // Print the decimal point
        // Determine the fractional part. The 10 multiplier gives us 1 decimal place.

        Serial.print("[L/min]:");
        Serial.print(m_data.flowRate_LitMin); // Print the fractional part of the variable
        // Print the number of litres flowed in this second

        // Print the cumulative total of litres flowed since starting
        Serial.print("  Output Liquid Quantity: "); // Output separator
        Serial.print(m_data.totalFlow_litres);
        Serial.println("L");
    }
}
