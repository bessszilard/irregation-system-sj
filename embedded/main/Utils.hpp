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
}