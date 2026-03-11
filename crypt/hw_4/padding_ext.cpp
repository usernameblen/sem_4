#pragma once
#include"padding_ext.h"
#include <stdexcept>
#include <cstdlib>

void Pkcs7Padding::pad(std::vector<uint8_t>& data, size_t blockSize) const 
{
    if (blockSize == 0 || blockSize > 255)
        throw std::invalid_argument("PKCS7: blockSize must be 1..255");

    size_t rem = data.size() % blockSize;
    size_t padLen = (rem == 0) ? blockSize : (blockSize - rem);
    data.insert(data.end(), padLen, static_cast<uint8_t>(padLen)); 
}

void Pkcs7Padding::unpad(std::vector<uint8_t>& data, size_t blockSize) const
{
    if (data.empty())
        throw std::runtime_error("PKCS7: empty data");
    if (blockSize == 0 || blockSize > 255)
        throw std::invalid_argument("PKCS7: blockSize must be 1..255");

    uint8_t padLen = data.back();
    if (padLen == 0 || padLen > static_cast<uint8_t>(blockSize))
        throw std::runtime_error("PKCS7: invalid padding length");

    // проверяем, что все байты набивки равны padLen
    for (size_t i = data.size() - padLen; i < data.size(); ++i) {
        if (data[i] != padLen)
            throw std::runtime_error("PKCS7: invalid padding byte");
    }
    data.resize(data.size() - padLen);
}



void Iso10126Padding::pad(std::vector<uint8_t>& data, size_t blockSize) const
{
    if (blockSize == 0 || blockSize > 255)
        throw std::invalid_argument("ISO10126: blockSize must be 1..255");

    size_t rem = data.size() % blockSize;
    size_t padLen = (rem == 0) ? blockSize : (blockSize - rem);

    for (size_t i = 0; i < padLen - 1; ++i)
        data.push_back(static_cast<uint8_t>(std::rand() & 0xFF));

    data.push_back(static_cast<uint8_t>(padLen));
}

void Iso10126Padding::unpad(std::vector<uint8_t>& data, size_t blockSize) const
{
    if (data.empty())
        throw std::runtime_error("ISO10126: empty data");
    if (blockSize == 0 || blockSize > 255)
        throw std::invalid_argument("ISO10126: blockSize must be 1..255");

    uint8_t padLen = data.back();
    if (padLen == 0 || padLen > static_cast<uint8_t>(blockSize))
        throw std::runtime_error("ISO10126: invalid padding length");

    data.resize(data.size() - padLen);
}