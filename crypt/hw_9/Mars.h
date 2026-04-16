#pragma once
#include "..\..\..\crypto\hw_3\interfaces.h"
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <array>


class MARS : public IEncryptionAlgorithm {
public:
    explicit MARS(const std::vector<uint8_t>& key);
    void encryptBlock(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) const override;
    void decryptBlock(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) const override;
    size_t blockSize() const override { return 16; }

private:
    static const uint32_t SBOX[512];
    std::array<uint32_t, 40> K_;

    static uint32_t rol32(uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }
    static uint32_t ror32(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
    static uint32_t toLE(const uint8_t* p) {
        return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
    }
    static void fromLE(uint32_t v, uint8_t* p) {
        p[0] = uint8_t(v); 
        p[1] = uint8_t(v >> 8); 
        p[2] = uint8_t(v >> 16); 
        p[3] = uint8_t(v >> 24);
    }

    void keyExpansion(const uint32_t* key, int n);
};