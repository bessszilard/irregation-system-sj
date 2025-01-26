#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

class String : public std::string
{
public:
    String(){};
    String(int p_i) : std::string(std::to_string(p_i)) {}
    String(uint8_t p_i) : std::string(std::to_string(p_i)) {}
    String(std::string p_str) : std::string(p_str) {}
    String(const char* p_charPtr) : std::string(p_charPtr) {}
    String(float p_float, int p_digits)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(p_digits) << p_float;
        this->assign(oss.str());
    }
};