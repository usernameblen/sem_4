#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <any>

using ModeParams = std::vector<std::any>;


class IEncryptionAlgorithm 
{
public:
    virtual ~IEncryptionAlgorithm() = default;

    virtual void encryptBlock(const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output) const = 0;

    virtual void decryptBlock(const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output) const = 0;

    virtual size_t blockSize() const = 0;
};


enum class PaddingMode { Zeros, ANSI_X923 };

class IPaddingMode {
public:
    virtual ~IPaddingMode() = default;
    virtual void pad(std::vector<uint8_t>& data, size_t blockSize) const = 0;
    virtual void unpad(std::vector<uint8_t>& data, size_t blockSize) const = 0;
};

enum class EncryptionMode { ECB, CBC, PCBC };

class IEncryptionMode {
public:
    virtual ~IEncryptionMode() = default;

    virtual void configure(const ModeParams& params) {} 

    virtual void encrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int numThreads) const = 0;

    virtual void decrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int numThreads) const = 0;
};