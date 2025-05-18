#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

constexpr uint8_t DEC = 10;
constexpr uint8_t HEX = 16;
constexpr uint8_t OCT = 8;
constexpr uint8_t BIN = 2;

class String : public std::string
{
public:
    String(){};
    String(int p_i) : std::string(std::to_string(p_i)) {}
    String(uint8_t p_i) : std::string(std::to_string(p_i)) {}
    explicit String(uint8_t value, uint8_t style)
    {
        std::ostringstream oss;
        switch (style)
        {
            case HEX:
                oss << std::hex << std::uppercase << static_cast<int>(value);
                break;
            case OCT:
                oss << std::oct << static_cast<int>(value);
                break;
            case BIN:
                for (int i = 7; i >= 0; --i)
                    oss << ((value >> i) & 1);
                break;
            case DEC:
            default:
                oss << static_cast<int>(value);
                break;
        }
        this->assign(oss.str());
    }

    String(std::string p_str) : std::string(p_str) {}
    String(const char* p_charPtr) : std::string(p_charPtr) {}
    template <typename T>
    String(T value, int p_digits, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(p_digits) << value;
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

    // Check if it ends with a character
    bool endsWith(const std::string& suffix) const
    {
        if (suffix.size() > this->size())
            return false;
        return this->compare(this->size() - suffix.size(), suffix.size(), suffix) == 0;
    }
};