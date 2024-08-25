#pragma once

#ifdef PC_BUILD
#include <string>
#include <iostream>
using String = std::string;
#else
#include <Arduino.h>
#endif

namespace Utils
{
String GetSubStr(const String& p_rawMsg, int p_startId, int p_endId);
}