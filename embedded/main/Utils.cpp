#include "Utils.hpp"

String Utils::GetSubStr(const String& p_rawMsg, int p_startId, int p_endId, bool p_verbose)
{
    if ((p_startId < 0))
    {
        return "";
    }
#ifdef PC_BUILD
    int end       = p_endId == -1 ? p_rawMsg.size() - 1 : p_endId;
    String subStr = p_rawMsg.substr(p_startId, p_endId - p_startId);

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