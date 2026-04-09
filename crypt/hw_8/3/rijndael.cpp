#include "rijndael.h"
#include "../2/sbox.hpp"
#include"../hw_8/poly.hpp"

RijndaelAlgorithm::RijndaelAlgorithm(const std::vector<uint8_t>& key, size_t blockSizeBytes)
{
    if (blockSizeBytes != 16 && blockSizeBytes != 24 && blockSizeBytes != 32)
        throw std::invalid_argument("Rijndael: unsupported block size (must be 16, 24, or 32)");

    size_t ks = key.size();
    if (ks != 16 && ks != 24 && ks != 32)
        throw std::invalid_argument("Rijndael: unsupported key size (must be 16, 24, or 32 bytes)");

    blockSizeBytes_ = blockSizeBytes;
    keySizeBytes_ = ks;
    Nb_ = static_cast<int>(blockSizeBytes / 4);
    Nk_ = static_cast<int>(ks / 4);
    Nr_ = std::max(Nk_, Nb_) + 6;

    sbox_ = build_sbox(0x11B);
    inv_sbox_ = build_inv_sbox(0x11B);

    keyExpansion(key);
}

const size_t RijndaelAlgorithm::blockSize() const
{
    return blockSizeBytes_;
}

int RijndaelAlgorithm::shiftOffset(int row) const
{
    if (Nb_ == 4)
    {
        static const int offsets[4] = { 0, 1, 2, 3 };
        return offsets[row];
    }
    else if (Nb_ == 6)
    {
        static const int offsets[4] = { 0, 1, 2, 3 };
        return offsets[row];
    }
    else
    {
        static const int offsets[4] = { 0, 1, 3, 4 };
        return offsets[row];
    }
}

void RijndaelAlgorithm::subBytes(std::vector<uint8_t>& state) const
{
    for (auto& b : state)
        b = sbox_[b];
}

void RijndaelAlgorithm::invSubBytes(std::vector<uint8_t>& state) const
{
    for (auto& b : state)
        b = inv_sbox_[b];
}

void RijndaelAlgorithm::shiftRows(std::vector<uint8_t>& state) const
{
    std::vector<uint8_t> tmp(state.size());
    for (int r = 0; r < 4; ++r)
    {
        int shift = shiftOffset(r);
        for (int c = 0; c < Nb_; ++c)
            tmp[r + 4 * c] = state[r + 4 * ((c + shift) % Nb_)];
    }
    state = tmp;
}

void RijndaelAlgorithm::invShiftRows(std::vector<uint8_t>& state) const
{
    std::vector<uint8_t> tmp(state.size());
    for (int r = 0; r < 4; ++r)
    {
        int shift = shiftOffset(r);
        for (int c = 0; c < Nb_; ++c)
            tmp[r + 4 * ((c + shift) % Nb_)] = state[r + 4 * c];
    }
    state = tmp;
}


uint8_t RijndaelAlgorithm::gf_mul(uint8_t a, uint8_t b) const {
    return static_cast<uint8_t>(::gf_mul(a, b, 0x11B, 8));
}

void RijndaelAlgorithm::mixColumns(std::vector<uint8_t>& state) const {
    for (int c = 0; c < Nb_; ++c) {
        uint8_t s0 = state[0 + 4 * c];
        uint8_t s1 = state[1 + 4 * c];
        uint8_t s2 = state[2 + 4 * c];
        uint8_t s3 = state[3 + 4 * c];

        state[0 + 4 * c] = gf_mul(0x02, s0) ^ gf_mul(0x03, s1) ^ s2 ^ s3;
        state[1 + 4 * c] = s0 ^ gf_mul(0x02, s1) ^ gf_mul(0x03, s2) ^ s3;
        state[2 + 4 * c] = s0 ^ s1 ^ gf_mul(0x02, s2) ^ gf_mul(0x03, s3);
        state[3 + 4 * c] = gf_mul(0x03, s0) ^ s1 ^ s2 ^ gf_mul(0x02, s3);
    }
}

void RijndaelAlgorithm::invMixColumns(std::vector<uint8_t>& state) const {
    for (int c = 0; c < Nb_; ++c) {
        uint8_t s0 = state[0 + 4 * c];
        uint8_t s1 = state[1 + 4 * c];
        uint8_t s2 = state[2 + 4 * c];
        uint8_t s3 = state[3 + 4 * c];

        state[0 + 4 * c] = gf_mul(0x0e, s0) ^ gf_mul(0x0b, s1) ^ gf_mul(0x0d, s2) ^ gf_mul(0x09, s3);
        state[1 + 4 * c] = gf_mul(0x09, s0) ^ gf_mul(0x0e, s1) ^ gf_mul(0x0b, s2) ^ gf_mul(0x0d, s3);
        state[2 + 4 * c] = gf_mul(0x0d, s0) ^ gf_mul(0x09, s1) ^ gf_mul(0x0e, s2) ^ gf_mul(0x0b, s3);
        state[3 + 4 * c] = gf_mul(0x0b, s0) ^ gf_mul(0x0d, s1) ^ gf_mul(0x09, s2) ^ gf_mul(0x0e, s3);
    }
}

void RijndaelAlgorithm::addRoundKey(std::vector<uint8_t>& state, int round) const
{
    const auto& rk = roundKeys_[round];
    for (size_t i = 0; i < state.size(); ++i)
        state[i] ^= rk[i];
}

std::vector<uint8_t> RijndaelAlgorithm::rotWord(const std::vector<uint8_t>& w) const
{
    return { w[1], w[2], w[3], w[0] };
}

std::vector<uint8_t> RijndaelAlgorithm::subWord(const std::vector<uint8_t>& w) const
{
    return { sbox_[w[0]], sbox_[w[1]], sbox_[w[2]], sbox_[w[3]] };
}

uint8_t RijndaelAlgorithm::rconByte(int i) const
{
    static const uint8_t rconTable[] = {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36,
        0x6C, 0xD8, 0xAB, 0x4D, 0x9A, 0x2F, 0x5E, 0xBC, 0x63, 0xC6,
        0x97, 0x35, 0x6A, 0xD4, 0xB3, 0x7D, 0xFA, 0xEF, 0xC5, 0x91
    };
    if (i <= 0) 
        return 0x00;
    if (i <= static_cast<int>(sizeof(rconTable) / sizeof(rconTable[0])))
        return rconTable[i - 1];
    return 0x00;
}

void RijndaelAlgorithm::keyExpansion(const std::vector<uint8_t>& key)
{
    int totalWords = Nb_ * (Nr_ + 1);
    std::vector<std::vector<uint8_t>> w(totalWords, std::vector<uint8_t>(4, 0));

    for (int i = 0; i < Nk_; ++i)
        w[i] = { key[4 * i], key[4 * i + 1], key[4 * i + 2], key[4 * i + 3] };

    for (int i = Nk_; i < totalWords; ++i)
    {
        std::vector<uint8_t> temp = w[i - 1];

        if (i % Nk_ == 0)
        {
            temp = subWord(rotWord(temp));
            uint8_t rc = rconByte(i / Nk_);
            temp[0] ^= rc;
        }
        else if (Nk_ > 6 && i % Nk_ == 4)
        {
            temp = subWord(temp);
        }

        for (int j = 0; j < 4; ++j)
            w[i][j] = w[i - Nk_][j] ^ temp[j];
    }

    roundKeys_.resize(Nr_ + 1, std::vector<uint8_t>(blockSizeBytes_, 0));
    for (int round = 0; round <= Nr_; ++round)
        for (int c = 0; c < Nb_; ++c)
            for (int r = 0; r < 4; ++r)
                roundKeys_[round][r + 4 * c] = w[round * Nb_ + c][r];
}

void RijndaelAlgorithm::encryptBlock(const std::vector<uint8_t>& input,
    std::vector<uint8_t>& output) const
{
    if (input.size() != blockSizeBytes_)
        throw std::invalid_argument("Rijndael::encryptBlock: wrong input size");

    std::vector<uint8_t> state(blockSizeBytes_);
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < Nb_; ++c)
            state[r + 4 * c] = input[r + 4 * c];

    addRoundKey(state, 0);

    for (int round = 1; round < Nr_; ++round)
    {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, round);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, Nr_);

    output.resize(blockSizeBytes_);
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < Nb_; ++c)
            output[r + 4 * c] = state[r + 4 * c];
}

void RijndaelAlgorithm::decryptBlock(const std::vector<uint8_t>& input,
    std::vector<uint8_t>& output) const
{
    if (input.size() != blockSizeBytes_)
        throw std::invalid_argument("Rijndael::decryptBlock: wrong input size");

    std::vector<uint8_t> state(blockSizeBytes_);
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < Nb_; ++c)
            state[r + 4 * c] = input[r + 4 * c];

    addRoundKey(state, Nr_);

    for (int round = Nr_ - 1; round >= 1; --round)
    {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, round);
        invMixColumns(state);
    }

    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, 0);

    output.resize(blockSizeBytes_);
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < Nb_; ++c)
            output[r + 4 * c] = state[r + 4 * c];
}