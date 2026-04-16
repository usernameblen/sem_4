#include "twofish.h"
#include <cstring>

static const uint8_t q0_t0[16] = { 0x8, 0x1, 0x7, 0xD, 0x6, 0xF, 0x3, 0x2, 0x0, 0xB, 0x5, 0x9, 0xE, 0xC, 0xA, 0x4 };
static const uint8_t q0_t1[16] = { 0xE, 0xC, 0xB, 0x8, 0x1, 0x2, 0x3, 0x5, 0xF, 0x4, 0xA, 0x6, 0x7, 0x0, 0x9, 0xD };
static const uint8_t q0_t2[16] = { 0xB, 0xA, 0x5, 0xE, 0x6, 0xD, 0x9, 0x0, 0xC, 0x8, 0xF, 0x3, 0x2, 0x4, 0x7, 0x1 };
static const uint8_t q0_t3[16] = { 0xD, 0x7, 0xF, 0x4, 0x1, 0x2, 0x6, 0xE, 0x9, 0xB, 0x3, 0x0, 0x8, 0x5, 0xC, 0xA };

static const uint8_t q1_t0[16] = { 0x2, 0x8, 0xB, 0xD, 0xF, 0x7, 0x6, 0xE, 0x3, 0x1, 0x9, 0x4, 0x0, 0xA, 0xC, 0x5 };
static const uint8_t q1_t1[16] = { 0x1, 0xE, 0x2, 0xB, 0x4, 0xC, 0x3, 0x7, 0x6, 0xD, 0xA, 0x5, 0xF, 0x9, 0x0, 0x8 };
static const uint8_t q1_t2[16] = { 0x4, 0xC, 0x7, 0x5, 0x1, 0x6, 0x9, 0xA, 0x0, 0xE, 0xD, 0x8, 0x2, 0xB, 0x3, 0xF };
static const uint8_t q1_t3[16] = { 0xB, 0x9, 0x5, 0x1, 0xC, 0x3, 0xD, 0xE, 0x6, 0x4, 0x7, 0xF, 0x2, 0x0, 0x8, 0xA };

const uint8_t Twofish::RS[4][8] = {
    {0x01,0xa4,0x55,0x87,0x5a,0x58,0xdb,0x9e},
    {0xa4,0x56,0x82,0xf3,0x1e,0xc6,0x68,0xe5},
    {0x02,0xa1,0xfc,0xc1,0x47,0xae,0x3d,0x19},
    {0xa4,0x55,0x87,0x5a,0x58,0xdb,0x9e,0x03}
};

const uint8_t Twofish::MDS[4][4] = {
    {0x01,0xef,0x5b,0x5b},
    {0x5b,0xef,0xef,0x01},
    {0xef,0x5b,0x01,0xef},
    {0xef,0x01,0xef,0x5b}
};

static inline uint8_t ror4(uint8_t x) {
    return ((x >> 1) | ((x & 1) << 3)) & 0x0F;
}

uint8_t Twofish::q0(uint8_t x) {
    uint8_t a0 = x >> 4;
    uint8_t b0 = x & 0x0F;
    uint8_t a1 = a0 ^ b0;
    uint8_t b1 = a0 ^ ror4(b0) ^ ((a0 << 3) & 0x0F);
    uint8_t a2 = q0_t0[a1];
    uint8_t b2 = q0_t1[b1];
    uint8_t a3 = a2 ^ b2;
    uint8_t b3 = a2 ^ ror4(b2) ^ ((a2 << 3) & 0x0F);
    uint8_t a4 = q0_t2[a3];
    uint8_t b4 = q0_t3[b3];
    return (a4 << 4) | b4;
}

uint8_t Twofish::q1(uint8_t x) {
    uint8_t a0 = x >> 4;
    uint8_t b0 = x & 0x0F;
    uint8_t a1 = a0 ^ b0;
    uint8_t b1 = a0 ^ ror4(b0) ^ ((a0 << 3) & 0x0F);
    uint8_t a2 = q1_t0[a1];
    uint8_t b2 = q1_t1[b1];
    uint8_t a3 = a2 ^ b2;
    uint8_t b3 = a2 ^ ror4(b2) ^ ((a2 << 3) & 0x0F);
    uint8_t a4 = q1_t2[a3];   // q1_t2
    uint8_t b4 = q1_t3[b3];   // q1_t3
    return (a4 << 4) | b4;
}

//умн в столбик
uint8_t Twofish::gfMul(uint8_t a, uint8_t b, uint16_t mod) {
    uint8_t result = 0;

    for (int i = 0; i < 8; i++) {
        if (b & 1) 
            result ^= a;
        b >>= 1;
        bool hi = (a & 0x80) != 0; //старший бит
        a <<= 1;
        if (hi) 
            a ^= mod;
    }
    return result;
}

uint32_t Twofish::mdsMultiply(uint32_t x) {
    uint8_t b[4];
    b[0] = uint8_t(x); 
    b[1] = uint8_t(x >> 8); 
    b[2] = uint8_t(x >> 16); 
    b[3] = uint8_t(x >> 24);
    uint8_t z[4] = { 0,0,0,0 };
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            z[i] ^= gfMul(b[j], MDS[i][j], 0x69);
    return uint32_t(z[0]) | (uint32_t(z[1]) << 8) | (uint32_t(z[2]) << 16) | (uint32_t(z[3]) << 24);
}

uint32_t Twofish::rsMultiply(uint8_t b, uint8_t p) {
    return gfMul(b, p, 0x4d);//0100 1101
}

uint32_t Twofish::h(uint32_t X, const uint32_t* L, int k) {
    uint8_t x[4];
    x[0] = uint8_t(X); 
    x[1] = uint8_t(X >> 8); 
    x[2] = uint8_t(X >> 16); 
    x[3] = uint8_t(X >> 24);

    uint8_t l[4][4];
    for (int i = 0; i < k; i++) {
        l[i][0] = uint8_t(L[i]);
        l[i][1] = uint8_t(L[i] >> 8);
        l[i][2] = uint8_t(L[i] >> 16);
        l[i][3] = uint8_t(L[i] >> 24);
    }

    if (k == 4) {
        x[0] = q1(x[0]) ^ l[3][0];
        x[1] = q0(x[1]) ^ l[3][1];
        x[2] = q0(x[2]) ^ l[3][2];
        x[3] = q1(x[3]) ^ l[3][3];
    }
    if (k >= 3) {
        x[0] = q1(x[0]) ^ l[2][0];
        x[1] = q1(x[1]) ^ l[2][1];
        x[2] = q0(x[2]) ^ l[2][2];
        x[3] = q0(x[3]) ^ l[2][3];
    }
    x[0] = q1(q0(q0(x[0]) ^ l[1][0]) ^ l[0][0]);
    x[1] = q0(q0(q1(x[1]) ^ l[1][1]) ^ l[0][1]);
    x[2] = q1(q1(q0(x[2]) ^ l[1][2]) ^ l[0][2]);
    x[3] = q0(q1(q1(x[3]) ^ l[1][3]) ^ l[0][3]);

    uint32_t v = uint32_t(x[0]) | (uint32_t(x[1]) << 8) | (uint32_t(x[2]) << 16) | (uint32_t(x[3]) << 24);
    return mdsMultiply(v);
}

uint32_t Twofish::g(uint32_t X, const uint32_t* skey, int k) {
    return h(X, skey, k);
}

void Twofish::keySchedule(const std::vector<uint8_t>& key) {
    int N = (int)key.size() * 8;
    k_ = N / 64;

    std::vector<uint32_t> Me(k_), Mo(k_);
    for (int i = 0; i < k_; i++) {
        Me[i] = toLE(key.data() + 8 * i);
        Mo[i] = toLE(key.data() + 8 * i + 4);
    }

    for (int i = 0; i < k_; i++) {
        uint32_t Si = 0;
        for (int j = 0; j < 4; j++) {
            uint8_t s = 0;
            for (int m = 0; m < 8; m++) {
                uint8_t kb = key[8 * i + m];
                s ^= rsMultiply(kb, RS[j][m]);
            }
            Si ^= (uint32_t(s) << (8 * j));
        }
        sboxKey_[k_ - 1 - i] = Si;
    }

    //раундовые ключи
    const uint32_t p = 0x01010101u;
    for (int i = 0; i < 20; i++) { //4+4 - нач+фин забеливания, 32 - для раундов
        uint32_t A = h(2 * i * p, Me.data(), k_);
        uint32_t B = rol32(h((2 * i + 1) * p, Mo.data(), k_), 8);
        subkeys_[2 * i] = (A + B) & ((1ULL << 32) - 1);
        subkeys_[2 * i + 1] = rol32((A + 2 * B) & ((1ULL << 32) - 1), 9);
    }
}

Twofish::Twofish(const std::vector<uint8_t>& key) {
    size_t bits = key.size() * 8;
    if (bits != 128 && bits != 192 && bits != 256)
        throw std::invalid_argument("Twofish: key must be 128, 192, or 256 bits");
    keySchedule(key);
}

void Twofish::encryptBlock(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) const {
    if (input.size() != 16) 
        throw std::invalid_argument("Twofish: block must be 16 bytes");

    output.resize(16);

    uint32_t R0 = toLE(input.data() + 0) ^ subkeys_[0];
    uint32_t R1 = toLE(input.data() + 4) ^ subkeys_[1];
    uint32_t R2 = toLE(input.data() + 8) ^ subkeys_[2];
    uint32_t R3 = toLE(input.data() + 12) ^ subkeys_[3];

    for (int r = 0; r < 16; r++) {
        uint32_t T0 = g(R0, sboxKey_.data(), k_);
        uint32_t T1 = g(rol32(R1, 8), sboxKey_.data(), k_);
        uint32_t F0 = (T0 + T1 + subkeys_[2 * r + 8]) & 0xFFFFFFFFu;
        uint32_t F1 = (T0 + 2 * T1 + subkeys_[2 * r + 9]) & 0xFFFFFFFFu;

        R2 = ror32(R2 ^ F0, 1);
        R3 = rol32(R3, 1) ^ F1;

        //(R0,R1,R2,R3) -> (R2,R3,R0,R1)
        uint32_t tmp0 = R0;
        uint32_t tmp1 = R1;
        R0 = R2; 
        R1 = R3; 
        R2 = tmp0; 
        R3 = tmp1;
    }

    fromLE(R2 ^ subkeys_[4], output.data() + 0);
    fromLE(R3 ^ subkeys_[5], output.data() + 4);
    fromLE(R0 ^ subkeys_[6], output.data() + 8);
    fromLE(R1 ^ subkeys_[7], output.data() + 12);
}

void Twofish::decryptBlock(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) const {
    if (input.size() != 16) 
        throw std::invalid_argument("Twofish: block must be 16 bytes");

    output.resize(16);

    uint32_t R0 = toLE(input.data() + 0) ^ subkeys_[4];
    uint32_t R1 = toLE(input.data() + 4) ^ subkeys_[5];
    uint32_t R2 = toLE(input.data() + 8) ^ subkeys_[6];
    uint32_t R3 = toLE(input.data() + 12) ^ subkeys_[7];

    for (int r = 15; r >= 0; r--) {
        uint32_t T0 = g(R0, sboxKey_.data(), k_);
        uint32_t T1 = g(rol32(R1, 8), sboxKey_.data(), k_);
        uint32_t F0 = (T0 + T1 + subkeys_[2 * r + 8]) & 0xFFFFFFFFu;
        uint32_t F1 = (T0 + 2 * T1 + subkeys_[2 * r + 9]) & 0xFFFFFFFFu;

        R2 = rol32(R2, 1) ^ F0;
        R3 = ror32(R3 ^ F1, 1);

        uint32_t tmp0 = R0;
        uint32_t tmp1 = R1;

        R0 = R2;
        R1 = R3;
        R2 = tmp0;
        R3 = tmp1;
    }

    fromLE(R2 ^ subkeys_[0], output.data() + 0);
    fromLE(R3 ^ subkeys_[1], output.data() + 4);
    fromLE(R0 ^ subkeys_[2], output.data() + 8);
    fromLE(R1 ^ subkeys_[3], output.data() + 12);
}