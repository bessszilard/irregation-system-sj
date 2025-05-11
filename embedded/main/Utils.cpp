#include "Utils.hpp"
#include <cstring>

String Utils::GetSubStr(const String& p_rawMsg, int p_startId, int p_endId, bool p_verbose)
{
    if ((p_startId < 0))
    {
        return "";
    }
    int end = p_endId < 0 ? strlen(p_rawMsg.c_str()) + p_endId : p_endId;

    String subStr = p_rawMsg.substr(p_startId, end - p_startId);
#ifdef PC_BUILD
    if (p_verbose)
        std::cout << subStr << std::endl;
#else
    int end       = p_endId == -1 ? p_rawMsg.length() - 1 : p_endId;
    String subStr = p_rawMsg.substring(p_startId, p_endId);
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