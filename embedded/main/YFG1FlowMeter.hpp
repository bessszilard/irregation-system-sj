#pragma once
#include <Arduino.h>

// TODOsz Calibrate!!!!
// Warning: this class uses a static variable, so it's a singleton.
class YFG1FlowMeter
{
public:
    YFG1FlowMeter(int pin, float calibrationFactor = 4.8);
    void resetValues();

    struct Data
    {
        float flowRate_LitMin;
        uint32_t totalFlow_litres;
    };

    void updateFlowData();

    inline const Data& getData() const
    {
        return m_data;
    }

private:
    float m_calibrationFactor;
    int m_inputPin;
    Data m_data;
    unsigned long m_oldTime_ms;
};