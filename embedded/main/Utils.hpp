#pragma once

#ifdef PC_BUILD
#include "../mock/Shifty.h"
#include "../mock/String.h"
#include "../mock/EnumsAndConstants.h"
#include "../mock/Serial.h"
#include <math.h>
#else
#include <Arduino.h>
#endif

namespace Utils
{
String GetSubStr(const String& p_rawMsg, int p_startId, int p_endId, bool p_verbose = false);
int32_t GetSmoothedRSSI(int32_t newRSSI);

uint16_t scaleTo99(uint16_t value);
} // namespace Utils