#pragma once
#include "..\hw_3\interfaces.h"

class Pkcs7Padding : public IPaddingMode {
public:
    void pad(std::vector<uint8_t>& data, size_t blockSize) const override;

    void unpad(std::vector<uint8_t>& data, size_t blockSize) const override;
};


class Iso10126Padding : public IPaddingMode {
public:
    void pad(std::vector<uint8_t>& data, size_t blockSize) const override;

    void unpad(std::vector<uint8_t>& data, size_t blockSize) const override;
};