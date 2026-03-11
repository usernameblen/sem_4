#pragma once
#include "interfaces.h"

#include <stdexcept>

class ZerosPadding : public IPaddingMode 
{
public:
    void pad(std::vector<uint8_t>& data, size_t blockSize) const override
    {
        if (blockSize == 0)
            throw std::invalid_argument("blockSize must be > 0");

        size_t rem = data.size() % blockSize;
        if (rem != 0)
            data.resize(data.size() + (blockSize - rem), 0x00);
    }

    void unpad(std::vector<uint8_t>& data, size_t ) const override
    {
        while (!data.empty() && data.back() == 0x00)
            data.pop_back();
    }
};


class AnsiX923Padding : public IPaddingMode 
{
public:
    void pad(std::vector<uint8_t>& data, size_t blockSize) const override
    {
        if (blockSize == 0)
            throw std::invalid_argument("blockSize must be > 0");

        size_t rem = data.size() % blockSize;
        size_t padLen = (rem == 0) ? blockSize : (blockSize - rem);

        data.insert(data.end(), padLen - 1, 0x00);
        data.push_back(static_cast<uint8_t>(padLen));
    }

    void unpad(std::vector<uint8_t>& data, size_t blockSize) const override
    {
        if (data.empty())
            throw std::runtime_error("Empty data during unpad");

        uint8_t padLen = data.back();
        if (padLen == 0 || padLen > blockSize)
            throw std::runtime_error("Invalid ANSI X.923 padding length");


        for (size_t i = data.size() - padLen; i < data.size() - 1; ++i)
        {
            if (data[i] != 0x00)
                throw std::runtime_error("Invalid ANSI X.923 padding bytes");
        }
        data.resize(data.size() - padLen);
    }
};