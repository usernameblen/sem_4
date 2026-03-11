#pragma once
#include "interfaces.h"

#include <stdexcept>

class XorCipher : public IEncryptionAlgorithm 
{
public:
    explicit XorCipher(std::vector<uint8_t> key) : key_(std::move(key)) 
    {
        if (key_.empty())
            throw std::invalid_argument("XorCipher: key must not be empty");
    }

    void encryptBlock(const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output) const override 
    {
        if (input.size() != key_.size())
            throw std::invalid_argument("XorCipher: block size mismatch");
        output.resize(key_.size());
        for (size_t i = 0; i < key_.size(); ++i)
            output[i] = input[i] ^ key_[i];
    }

    void decryptBlock(const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output) const override 
    {
        encryptBlock(input, output);
    }

    size_t blockSize() const override { return key_.size(); }

private:
    std::vector<uint8_t> key_;
};