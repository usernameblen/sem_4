#pragma once
#include "..\..\..\crypto\hw_3\interfaces.h"
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <array>

class Twofish : public IEncryptionAlgorithm {
public:
    explicit Twofish(const std::vector<uint8_t>& key);
    void encryptBlock(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) const override;
    void decryptBlock(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) const override;
    size_t blockSize() const override { return 16; }

private:
    static const uint8_t RS[4][8];
    static const uint8_t MDS[4][4];

    int k_;
    std::array<uint32_t, 40> subkeys_;
    std::array<uint32_t, 4> sboxKey_;

    static uint8_t q0(uint8_t x);
    static uint8_t q1(uint8_t x);
    static uint8_t gfMul(uint8_t a, uint8_t b, uint16_t mod);
    static uint32_t mdsMultiply(uint32_t x);
    static uint32_t rsMultiply(uint8_t b, uint8_t p);
    static uint32_t h(uint32_t X, const uint32_t* L, int k);
    static uint32_t g(uint32_t X, const uint32_t* skey, int k);
    static uint32_t rol32(uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }//циклические сдвиги
    static uint32_t ror32(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
    static uint32_t toLE(const uint8_t* p) { //4 байта -> число
        return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
    }
    static void fromLE(uint32_t v, uint8_t* p) {//число -> 4 байта
        p[0] = uint8_t(v); 
        p[1] = uint8_t(v >> 8); 
        p[2] = uint8_t(v >> 16); 
        p[3] = uint8_t(v >> 24);
    }

    void keySchedule(const std::vector<uint8_t>& key);
};