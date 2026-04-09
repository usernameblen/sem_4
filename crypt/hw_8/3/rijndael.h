#pragma once
#include "../hw_3/interfaces.h"
#include <vector>
#include <array>
#include <cstdint>
#include <stdexcept>

class RijndaelAlgorithm : public IEncryptionAlgorithm
{
public:
    RijndaelAlgorithm(const std::vector<uint8_t>& key, size_t blockSizeBytes);

    void encryptBlock(const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output) const override;

    void decryptBlock(const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output) const override;

    const size_t blockSize() const override;

private:
    size_t blockSizeBytes_;
    size_t keySizeBytes_;
    int    Nb_;
    int    Nk_;
    int    Nr_; //колво раундов

    std::array<uint8_t, 256> sbox_;
    std::array<uint8_t, 256> inv_sbox_;

    std::vector<std::vector<uint8_t>> roundKeys_;

    void subBytes(std::vector<uint8_t>& state) const;
    void invSubBytes(std::vector<uint8_t>& state) const;

    void shiftRows(std::vector<uint8_t>& state) const;
    void invShiftRows(std::vector<uint8_t>& state) const;

    void mixColumns(std::vector<uint8_t>& state) const;
    void invMixColumns(std::vector<uint8_t>& state) const;

    void addRoundKey(std::vector<uint8_t>& state, int round) const;

    void keyExpansion(const std::vector<uint8_t>& key);

    std::vector<uint8_t> subWord(const std::vector<uint8_t>& w) const;
    std::vector<uint8_t> rotWord(const std::vector<uint8_t>& w) const;
    uint8_t              rconByte(int i) const;

    int shiftOffset(int row) const;
    uint8_t gf_mul(uint8_t a, uint8_t b) const;
};