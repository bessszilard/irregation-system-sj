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

    // Arduino-style indexOf
    int indexOf(const std::string& substr, size_t fromIndex = 0) const
    {
        size_t pos = this->find(substr, fromIndex);
        return (pos != std::string::npos) ? static_cast<int>(pos) : -1;
    }

    // Arduino-style substring
    String substring(size_t start, size_t end) const
    {
        if (start >= this->length() || end <= start)
            return "";
        return this->substr(start, end - start);
    }

    String substring(size_t start) const
    {
        if (start >= this->length())
            return "";
        return this->substr(start);
    }

    // Arduino-style trim (removes leading and trailing whitespace)
    void trim()
    {
        auto start = this->begin();
        while (start != this->end() && std::isspace(*start))
            ++start;

        auto end = this->end();
        do
        {
            --end;
        } while (end >= start && std::isspace(*end));

        if (start <= end)
            *this = this->substr(start - this->begin(), end - start + 1);
        else
            this->clear();
    }
};