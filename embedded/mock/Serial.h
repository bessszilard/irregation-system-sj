#pragma once

#include <iostream>
#include "String.h"
#include <cstdarg> // For va_list
#include <cstdio>  // For vsnprintf

class SerialClass
{
public:
    SerialClass(){};
    static void println(const String& input)
    {
        std::cout << input << std::endl;
    }

    static void print(const String& input)
    {
        std::cout << input;
    }

    static void println(const char* input)
    {
        std::cout << input << std::endl;
    }

    static void print(const char* input)
    {
        std::cout << input;
    }

    static void println(const std::string& input)
    {
        std::cout << input << std::endl;
    }

    static void print(const std::string& input)
    {
        std::cout << input;
    }

    static void printf(const char* format, ...)
    {
        constexpr size_t BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];

        va_list args;
        va_start(args, format);
        vsnprintf(buffer, BUFFER_SIZE, format, args);
        va_end(args);

        std::cout << buffer;
    }
};

inline SerialClass& SerialInstance()
{
    static SerialClass instance;
    return instance;
}

#define Serial SerialInstance()