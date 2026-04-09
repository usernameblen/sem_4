#pragma once

#include <array>
#include <cstdint>
#include "../hw_8/poly.hpp"

static uint8_t affine(uint8_t x) {
    uint8_t result = 0;
    for (int i = 0; i < 8; ++i) {
        int bit = ((x >> i) & 1)
            ^ ((x >> ((i + 4) & 7)) & 1)
            ^ ((x >> ((i + 5) & 7)) & 1)
            ^ ((x >> ((i + 6) & 7)) & 1)
            ^ ((x >> ((i + 7) & 7)) & 1)
            ^ ((0x63 >> i) & 1);
        result |= static_cast<uint8_t>(bit << i);
    }
    return result;
}

std::array<uint8_t, 256> build_sbox(gf_elem irreducible_poly) {
    std::array<uint8_t, 256> sbox;
    for (int i = 0; i < 256; ++i) {
        gf_elem b_inv = (i == 0) ? 0 : gf_inv(static_cast<gf_elem>(i), irreducible_poly, 8);
        sbox[i] = affine(static_cast<uint8_t>(b_inv));
    }
    return sbox;
}

std::array<uint8_t, 256> build_inv_sbox(gf_elem irreducible_poly) {
    auto sbox = build_sbox(irreducible_poly);
    std::array<uint8_t, 256> inv_sbox;
    for (int i = 0; i < 256; ++i)
        inv_sbox[sbox[i]] = static_cast<uint8_t>(i);
    return inv_sbox;
}