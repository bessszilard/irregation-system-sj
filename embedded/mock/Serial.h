#pragma once

#include <iostream>
#include "String.h"

class SerialClass
{
public:
    SerialClass(){};
    static void println(const String& input)
    {
        std::cout << input;
    }

    static void println(const std::string& input)
    {
        std::cout << input;
    }
};

inline SerialClass& SerialInstance()
{
    static SerialClass instance;
    return instance;
}

#define Serial SerialInstance()