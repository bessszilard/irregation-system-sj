#include "Utils.hpp"
#include <cstring>

String Utils::GetSubStr(const String& p_rawMsg, int p_startId, int p_endId, bool p_verbose)
{
    if ((p_startId < 0))
    {
        return "";
    }
    int end = p_endId < 0 ? strlen(p_rawMsg.c_str()) + p_endId + 1 : p_endId; // Make inclusive

    String subStr = p_rawMsg.substring(p_startId, end);
#ifdef PC_BUILD
    if (p_verbose)
    {
        std::cout << "Input:" << p_rawMsg << ", StartId:" << p_startId << ", end:" << end << ", substr:" << subStr
                  << std::endl;
    }
#endif
    return subStr;
}

static const uint8_t NUM_SAMPLES = 5;
static int32_t rssiBuffer[NUM_SAMPLES];
static uint8_t rssiBufferId = 0;

int32_t Utils::GetSmoothedRSSI(int32_t newRSSI)
{
    rssiBuffer[rssiBufferId] = newRSSI;
    rssiBufferId             = (rssiBufferId + 1) % NUM_SAMPLES;

    int sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        sum += rssiBuffer[i];
    }
    return sum / NUM_SAMPLES;
}

// TODOsz add proper boundaries
uint16_t Utils::scaleTo99(uint16_t value)
{
    return (uint32_t)value; // * 99 / 65535;
}
