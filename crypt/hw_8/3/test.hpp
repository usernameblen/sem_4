#pragma once
#include "rijndael.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

namespace RijndaelTests
{

    static std::vector<uint8_t> hexToBytes(const std::string& hex)
    {
        std::vector<uint8_t> out;
        for (size_t i = 0; i + 1 < hex.size(); i += 2)
        {
            uint8_t hi = hex[i] >= 'a' ? hex[i] - 'a' + 10 : hex[i] - '0';
            uint8_t lo = hex[i + 1] >= 'a' ? hex[i + 1] - 'a' + 10 : hex[i + 1] - '0';
            out.push_back((hi << 4) | lo);
        }
        return out;
    }

    static void checkEqual(const std::vector<uint8_t>& a,
        const std::vector<uint8_t>& b,
        const std::string& label)
    {
        if (a != b)
        {
            std::cerr << "FAIL: " << label << "\n  got:      ";
            for (auto x : a) std::cerr << std::hex << (int)x << " ";
            std::cerr << "\n  expected: ";
            for (auto x : b) std::cerr << std::hex << (int)x << " ";
            std::cerr << std::dec << "\n";
            throw std::runtime_error("Test failed: " + label);
        }
        std::cout << "PASS: " << label << "\n";
    }

    static void testSBox()
    {
        std::vector<uint8_t> key(16, 0);
        RijndaelAlgorithm algo(key, 16);

        std::vector<uint8_t> state(16, 0);
        std::vector<uint8_t> enc(16), dec(16);
        algo.encryptBlock(state, enc);

        std::vector<uint8_t> expected = hexToBytes("66e94bd4ef8a2c3b884cfa59ca342b2e");
        checkEqual(enc, expected, "SBox/AES-128 all-zero encrypt");

        algo.decryptBlock(enc, dec);
        checkEqual(dec, state, "SBox/InvSBox roundtrip");
    }

    static void testShiftRows()
    {
        std::vector<uint8_t> key(16, 0x00);
        RijndaelAlgorithm algo16(key, 16);
        std::vector<uint8_t> plaintext(16, 0x00);
        std::vector<uint8_t> enc(16), dec(16);
        algo16.encryptBlock(plaintext, enc);
        algo16.decryptBlock(enc, dec);
        checkEqual(dec, plaintext, "ShiftRows Nb=4 roundtrip");

        std::vector<uint8_t> key24(24, 0x00);
        RijndaelAlgorithm algo24(key24, 24);
        std::vector<uint8_t> pt24(24, 0x00);
        std::vector<uint8_t> enc24(24), dec24(24);
        algo24.encryptBlock(pt24, enc24);
        algo24.decryptBlock(enc24, dec24);
        checkEqual(dec24, pt24, "ShiftRows Nb=6 roundtrip");

        std::vector<uint8_t> key32(32, 0x00);
        RijndaelAlgorithm algo32(key32, 32);
        std::vector<uint8_t> pt32(32, 0x00);
        std::vector<uint8_t> enc32(32), dec32(32);
        algo32.encryptBlock(pt32, enc32);
        algo32.decryptBlock(enc32, dec32);
        checkEqual(dec32, pt32, "ShiftRows Nb=8 roundtrip");
    }

    static void testMixColumns()
    {
        std::vector<uint8_t> key(16, 0x00);
        RijndaelAlgorithm algo(key, 16);

        std::vector<uint8_t> pt(16);
        for (int i = 0; i < 16; ++i) pt[i] = static_cast<uint8_t>(i);
        std::vector<uint8_t> enc(16), dec(16);
        algo.encryptBlock(pt, enc);
        algo.decryptBlock(enc, dec);
        checkEqual(dec, pt, "MixColumns/InvMixColumns roundtrip (sequential block)");
    }

    static void testKeyExpansion128()
    {
        std::vector<uint8_t> key = hexToBytes("000102030405060708090a0b0c0d0e0f");
        std::vector<uint8_t> pt = hexToBytes("00112233445566778899aabbccddeeff");
        std::vector<uint8_t> expected_ct = hexToBytes("69c4e0d86a7b0430d8cdb78070b4c55a");

        RijndaelAlgorithm algo(key, 16);
        std::vector<uint8_t> ct(16);
        algo.encryptBlock(pt, ct);
        checkEqual(ct, expected_ct, "KeyExpansion AES-128 NIST vector");

        std::vector<uint8_t> dec(16);
        algo.decryptBlock(ct, dec);
        checkEqual(dec, pt, "KeyExpansion AES-128 decrypt NIST vector");
    }

    static void testKeyExpansion192()
    {
        std::vector<uint8_t> key = hexToBytes("000102030405060708090a0b0c0d0e0f1011121314151617");
        std::vector<uint8_t> pt = hexToBytes("00112233445566778899aabbccddeeff");
        std::vector<uint8_t> expected_ct = hexToBytes("dda97ca4864cdfe06eaf70a0ec0d7191");

        RijndaelAlgorithm algo(key, 16);
        std::vector<uint8_t> ct(16);
        algo.encryptBlock(pt, ct);
        checkEqual(ct, expected_ct, "KeyExpansion AES-192 NIST vector");

        std::vector<uint8_t> dec(16);
        algo.decryptBlock(ct, dec);
        checkEqual(dec, pt, "KeyExpansion AES-192 decrypt NIST vector");
    }

    static void testKeyExpansion256()
    {
        std::vector<uint8_t> key = hexToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
        std::vector<uint8_t> pt = hexToBytes("00112233445566778899aabbccddeeff");
        std::vector<uint8_t> expected_ct = hexToBytes("8ea2b7ca516745bfeafc49904b496089");

        RijndaelAlgorithm algo(key, 16);
        std::vector<uint8_t> ct(16);
        algo.encryptBlock(pt, ct);
        checkEqual(ct, expected_ct, "KeyExpansion AES-256 NIST vector");

        std::vector<uint8_t> dec(16);
        algo.decryptBlock(ct, dec);
        checkEqual(dec, pt, "KeyExpansion AES-256 decrypt NIST vector");
    }

    static void testAllBlockKeyCombinations()
    {
        std::vector<size_t> keySizes = { 16, 24, 32 };
        std::vector<size_t> blockSizes = { 16, 24, 32 };

        for (size_t ks : keySizes)
        {
            for (size_t bs : blockSizes)
            {
                std::vector<uint8_t> key(ks, 0xAB);
                std::vector<uint8_t> pt(bs);
                for (size_t i = 0; i < bs; ++i) pt[i] = static_cast<uint8_t>(i * 3 + 7);

                RijndaelAlgorithm algo(key, bs);
                std::vector<uint8_t> ct(bs), dec(bs);
                algo.encryptBlock(pt, ct);
                algo.decryptBlock(ct, dec);

                std::string label = "Block=" + std::to_string(bs * 8) +
                    " Key=" + std::to_string(ks * 8) + " roundtrip";
                checkEqual(dec, pt, label);
            }
        }
    }

    static void testInvalidInputs()
    {
        try
        {
            std::vector<uint8_t> key(15, 0);
            RijndaelAlgorithm algo(key, 16);
            throw std::runtime_error("Should have thrown on bad key size");
        }
        catch (const std::invalid_argument&)
        {
            std::cout << "PASS: invalid key size throws\n";
        }

        try
        {
            std::vector<uint8_t> key(16, 0);
            RijndaelAlgorithm algo(key, 17);
            throw std::runtime_error("Should have thrown on bad block size");
        }
        catch (const std::invalid_argument&)
        {
            std::cout << "PASS: invalid block size throws\n";
        }
    }

    static void runAll()
    {
        std::cout << "\n=== Rijndael Unit Tests ===\n";
        testSBox();
        testShiftRows();
        testMixColumns();
        testKeyExpansion128();
        testKeyExpansion192();
        testKeyExpansion256();
        testAllBlockKeyCombinations();
        testInvalidInputs();
        std::cout << "=== All tests passed ===\n\n";
    }

}