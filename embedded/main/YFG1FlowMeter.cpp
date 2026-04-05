#include "YFG1FlowMeter.hpp"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

static volatile uint16_t s_pulseCount = 0;
static portMUX_TYPE s_pulseMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR pulseCounter()
{
    portENTER_CRITICAL_ISR(&s_pulseMux);
    s_pulseCount++;
    portEXIT_CRITICAL_ISR(&s_pulseMux);
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
    portENTER_CRITICAL(&s_pulseMux);
    s_pulseCount = 0;
    portEXIT_CRITICAL(&s_pulseMux);
    m_oldTime_ms = 0;
    m_data       = Data(0.0, 0);
}

void YFG1FlowMeter::updateFlowData(bool verbose)
{
    if ((millis() - m_oldTime_ms) > 1000) // Only process counters once per second
    {
        portENTER_CRITICAL(&s_pulseMux);
        uint16_t pulseCount = s_pulseCount;
        s_pulseCount        = 0;
        portEXIT_CRITICAL(&s_pulseMux);

        float flowRate_mLitPerSec = ((1000.0 / (millis() - m_oldTime_ms)) * pulseCount) / m_calibrationFactor;
        m_oldTime_ms = millis();

        m_data.flowRate_LitMin = flowRate_mLitPerSec * 60 / 1000;
        m_data.totalFlow_litres += m_data.flowRate_LitMin;

        // Print the flow rate for this second in litres / minute
        if (verbose)
        {
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
}
