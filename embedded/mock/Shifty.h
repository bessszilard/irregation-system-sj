#pragma once
#include <stdint.h>

using byte = uint8_t;

class Shifty
{
public:
    Shifty();

    void setBitCount(int bitCount){};
    void setPins(int dataPin, int clockPin, int latchPin, int readPin){};
    void setPins(int dataPin, int clockPin, int latchPin){};
    void setBitMode(int bitnum, bool mode){};
    bool getBitMode(int bitnum)
    {
        return false;
    };
    void batchWriteBegin(){};
    void batchWriteEnd(){};
    void batchReadBegin(){};
    void batchReadEnd(){};
    void writeBit(int bitnum, bool value){};
    bool readBit(int bitnum)
    {
        return false;
    };

private:
    int dataPin;
    int clockPin;
    int readPin;
    int latchPin;

    int bitCount;
    int byteCount;
    byte writeBuffer[16];
    byte dataModes[16];
    byte readBuffer[16];
    bool batchWriteMode;
    bool batchReadMode;

    void writeAllBits(){};
    void readAllBits(){};
    void writeBitHard(int bitnum, bool value){};
    void writeBitSoft(int bitnum, bool value){};
    bool readBitHard(int bitnum)
    {
        return false;
    };
    bool readBitSoft(int bitnum)
    {
        return false;
    };
};