#define CATCH_CONFIG_MAIN
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <cstring>
#include <random>
#include <fstream>
#include <future>
#include <thread>
#include <sstream>
#include <iomanip>

#include "..\..\..\crypto\hw_3\interfaces.h"
#include "..\..\..\crypto\hw_3\modes.h"
#include "..\..\..\crypto\hw_3\padding.h"
#include "..\..\..\crypto\hw_4\padding_ext.h"
#include "..\..\..\crypto\hw_4\modes_ext.h"
#include "..\..\..\crypto\hw_4\cipher_context2.h"
#include "twofish.h"
#include "mars.h"

static int g_pass = 0, g_fail = 0;

#define CHECK(expr, msg) do { \
    if (!(expr)) { std::cout << "FAIL: " << (msg) << "\n"; g_fail++; } \
    else { std::cout << "PASS: " << (msg) << "\n"; g_pass++; } \
} while(0)

static std::vector<uint8_t> hexToBytes(const std::string& s) {
    std::vector<uint8_t> result;
    for (size_t i = 0; i + 1 < s.size(); i += 2) {
        uint8_t b = (uint8_t)std::stoi(s.substr(i, 2), nullptr, 16);
        result.push_back(b);
    }
    return result;
}

static std::string bytesToHex(const std::vector<uint8_t>& v) {
    std::ostringstream oss;
    for (auto b : v) oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    return oss.str();
}

static void testTwofishVectors() {
    {
        auto key = hexToBytes("00000000000000000000000000000000");
        auto pt = hexToBytes("00000000000000000000000000000000");
        auto expected = hexToBytes("9F589F5CF6122C32B6BFEC2F2AE8C35A");
        Twofish tf(key);
        std::vector<uint8_t> ct;
        tf.encryptBlock(pt, ct);
        CHECK(bytesToHex(ct) == "9f589f5cf6122c32b6bfec2f2ae8c35a", "Twofish 128-bit KAT encrypt");
        std::vector<uint8_t> dec;
        tf.decryptBlock(ct, dec);
        CHECK(dec == pt, "Twofish 128-bit KAT decrypt");
    }
    {
        auto key = hexToBytes("000000000000000000000000000000000000000000000000");
        auto pt = hexToBytes("00000000000000000000000000000000");
        auto expected = hexToBytes("EFA71F788965BD4453F860178FC19101");
        Twofish tf(key);
        std::vector<uint8_t> ct;
        tf.encryptBlock(pt, ct);
        CHECK(bytesToHex(ct) == "efa71f788965bd4453f860178fc19101", "Twofish 192-bit KAT encrypt");
        std::vector<uint8_t> dec;
        tf.decryptBlock(ct, dec);
        CHECK(dec == pt, "Twofish 192-bit KAT decrypt");
    }
    {
        auto key = hexToBytes("0000000000000000000000000000000000000000000000000000000000000000");
        auto pt = hexToBytes("00000000000000000000000000000000");
        Twofish tf(key);
        std::vector<uint8_t> ct;
        tf.encryptBlock(pt, ct);
        CHECK(bytesToHex(ct) == "57ff739d4dc92c1bd7fc01700cc8216f", "Twofish 256-bit KAT encrypt");
        std::vector<uint8_t> dec;
        tf.decryptBlock(ct, dec);
        CHECK(dec == pt, "Twofish 256-bit KAT decrypt");
    }
}

static void testTwofishInvertible() {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 255);
    std::vector<size_t> keySizes = { 16, 24, 32 };
    for (size_t ks : keySizes) {
        std::vector<uint8_t> key(ks);
        for (auto& b : key) b = (uint8_t)dist(rng);
        Twofish tf(key);
        for (int iter = 0; iter < 100; iter++) {
            std::vector<uint8_t> plain(16);
            for (auto& b : plain) b = (uint8_t)dist(rng);
            std::vector<uint8_t> ct, dec;
            tf.encryptBlock(plain, ct);
            tf.decryptBlock(ct, dec);
            CHECK(dec == plain, "Twofish invertible keysize=" + std::to_string(ks * 8) + " iter=" + std::to_string(iter));
        }
    }
}

static void testMARSInvertible() {
    std::mt19937 rng(123);
    std::uniform_int_distribution<int> dist(0, 255);
    std::vector<size_t> keySizes = { 16, 24, 32 };
    for (size_t ks : keySizes) {
        std::vector<uint8_t> key(ks);
        for (auto& b : key) b = (uint8_t)dist(rng);
        MARS mars(key);
        for (int iter = 0; iter < 100; iter++) {
            std::vector<uint8_t> plain(16);
            for (auto& b : plain) b = (uint8_t)dist(rng);
            std::vector<uint8_t> ct, dec;
            mars.encryptBlock(plain, ct);
            mars.decryptBlock(ct, dec);
            CHECK(dec == plain, "MARS invertible keysize=" + std::to_string(ks * 8) + " iter=" + std::to_string(iter));
        }
    }
}

static void testInvalidKey() {
    bool threw = false;
    try {
        std::vector<uint8_t> badKey(10, 0);
        Twofish tf(badKey);
    }
    catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw, "Twofish invalid key throws");

    threw = false;
    try {
        std::vector<uint8_t> badKey(10, 0);
        MARS m(badKey);
    }
    catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw, "MARS invalid key throws");
}

static void testCipherContext2() {
    auto key = hexToBytes("000102030405060708090a0b0c0d0e0f");
    std::vector<uint8_t> iv(16, 0x55);
    Twofish* tf = new Twofish(key);
    std::vector<uint8_t> plain = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                                   0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,
                                   0x11,0x12,0x13,0x14 };

    struct ModeTest { EncryptionMode2 mode; std::string name; };
    std::vector<ModeTest> modes = {
        {EncryptionMode2::ECB, "ECB"},
        {EncryptionMode2::CBC, "CBC"},
        {EncryptionMode2::CTR, "CTR"},
    };
    struct PadTest { PaddingMode2 pad; std::string name; };
    std::vector<PadTest> pads = {
        {PaddingMode2::PKCS7, "PKCS7"},
        {PaddingMode2::ISO10126, "ISO10126"},
    };

    for (auto& mt : modes) {
        for (auto& pt : pads) {
            CipherContext2 ctx(tf, mt.mode, pt.pad, iv);
            std::vector<uint8_t> ct, dec;
            ctx.encrypt(plain, ct, 1);
            ctx.decrypt(ct, dec, 1);
            CHECK(dec == plain, "CipherContext2 Twofish " + mt.name + " " + pt.name);
        }
    }

    {
        std::vector<uint8_t> iv2(32, 0xAB);
        CipherContext2 ctx(tf, EncryptionMode2::RandomDelta, PaddingMode2::PKCS7, iv2);
        std::vector<uint8_t> ct, dec;
        ctx.encrypt(plain, ct, 1);
        ctx.decrypt(ct, dec, 1);
        CHECK(dec == plain, "CipherContext2 Twofish RandomDelta PKCS7");
    }

    delete tf;
}

static void testMultiThread() {
    auto key = hexToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
    std::vector<uint8_t> iv(16, 0);
    Twofish* tf = new Twofish(key);
    std::vector<uint8_t> plain(64, 0xAB);

    std::vector<uint8_t> ct1, ct2, ct4;
    {
        CipherContext2 ctx(tf, EncryptionMode2::CTR, PaddingMode2::PKCS7, iv);
        ctx.encrypt(plain, ct1, 1);
    }
    {
        CipherContext2 ctx(tf, EncryptionMode2::CTR, PaddingMode2::PKCS7, iv);
        ctx.encrypt(plain, ct2, 2);
    }
    {
        CipherContext2 ctx(tf, EncryptionMode2::CTR, PaddingMode2::PKCS7, iv);
        ctx.encrypt(plain, ct4, 4);
    }
    CHECK(ct1 == ct2, "Twofish CTR same result 1 vs 2 threads");
    CHECK(ct1 == ct4, "Twofish CTR same result 1 vs 4 threads");
    delete tf;
}

static void testAsyncFile() {
    try {
        auto key = hexToBytes("000102030405060708090a0b0c0d0e0f");
        std::vector<uint8_t> iv(16, 0);
        MARS* m = new MARS(key);
        std::vector<uint8_t> plain = { 0xDE,0xAD,0xBE,0xEF,0xCA,0xFE,0xBA,0xBE,
                                       0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF };

        // Запись входного файла
        {
            std::ofstream f("mars_test_input.bin", std::ios::binary);
            if (!f.is_open()) throw std::runtime_error("Cannot create input file");
            f.write((const char*)plain.data(), plain.size());
            if (!f) throw std::runtime_error("Write error");
        }

        {
            CipherContext2 ctx(m, EncryptionMode2::CBC, PaddingMode2::Zeros, iv);
            auto fut = ctx.encrypt("mars_test_input.bin", "mars_test_enc.bin", 2);
            fut.get(); // здесь может вылететь исключение
            auto fut2 = ctx.decrypt("mars_test_enc.bin", "mars_test_dec.bin", 2);
            fut2.get();
        }

        std::vector<uint8_t> dec;
        {
            std::ifstream f("mars_test_dec.bin", std::ios::binary);
            if (!f.is_open()) throw std::runtime_error("Cannot open decrypted file");
            dec.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
        }

        std::vector<uint8_t> expected = plain;
        while (!expected.empty() && expected.back() == 0) expected.pop_back();
        while (!dec.empty() && dec.back() == 0) dec.pop_back();
        CHECK(dec == expected, "MARS async file encrypt/decrypt CBC Zeros 2 threads");
        delete m;
    }
    catch (const std::exception& e) {
        std::cout << "Exception in testAsyncFile: " << e.what() << "\n";
        throw; // чтобы тест провалился
    }
}

static void testEdgeCases() {
    auto key = hexToBytes("000102030405060708090a0b0c0d0e0f");
    std::vector<uint8_t> iv(16, 0);
    Twofish* tf = new Twofish(key);

    std::vector<uint8_t> empty = {};
    {
        CipherContext2 ctx(tf, EncryptionMode2::ECB, PaddingMode2::PKCS7, iv);
        std::vector<uint8_t> ct, dec;
        ctx.encrypt(empty, ct, 1);
        ctx.decrypt(ct, dec, 1);
        CHECK(dec == empty, "Twofish ECB empty data");
    }

    std::vector<uint8_t> single(1, 0x42);
    {
        CipherContext2 ctx(tf, EncryptionMode2::CBC, PaddingMode2::PKCS7, iv);
        std::vector<uint8_t> ct, dec;
        ctx.encrypt(single, ct, 1);
        ctx.decrypt(ct, dec, 1);
        CHECK(dec == single, "Twofish CBC single byte");
    }

    std::vector<uint8_t> exact(16, 0x55);
    {
        CipherContext2 ctx(tf, EncryptionMode2::CTR, PaddingMode2::PKCS7, iv);
        std::vector<uint8_t> ct, dec;
        ctx.encrypt(exact, ct, 1);
        ctx.decrypt(ct, dec, 1);
        CHECK(dec == exact, "Twofish CTR exact block PKCS7");
    }

    delete tf;
}

void demo() {
    std::cout << "\n DEMO \n";

    {
        std::cout << "\n[1] Twofish CTR + PKCS7 sync:\n";
        auto key = hexToBytes("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
        std::vector<uint8_t> iv(16, 0x01);
        Twofish tf(key);
        std::vector<uint8_t> plain = { 0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64 };
        CipherContext2 ctx(&tf, EncryptionMode2::CTR, PaddingMode2::PKCS7, iv);
        std::vector<uint8_t> ct, dec;
        ctx.encrypt(plain, ct, 1);
        ctx.decrypt(ct, dec, 1);
        std::cout << "  Plain:  " << bytesToHex(plain) << "\n";
        std::cout << "  CT:     " << bytesToHex(ct) << "\n";
        std::cout << "  Decrypted matches: " << (dec == plain ? "YES" : "NO") << "\n";
    }

    {
        std::cout << "\n[2] MARS CBC + Zeros async file, 2 threads:\n";
        auto key = hexToBytes("000102030405060708090a0b0c0d0e0f");
        std::vector<uint8_t> iv(16, 0);
        MARS mars(key);
        std::vector<uint8_t> plain(48, 0xAB);
        {
            std::ofstream f("demo_input.bin", std::ios::binary);
            f.write((const char*)plain.data(), plain.size());
        }
        CipherContext2 ctx(&mars, EncryptionMode2::CBC, PaddingMode2::Zeros, iv);
        ctx.encrypt("demo_input.bin", "demo_enc.bin", 2).get();
        ctx.decrypt("demo_enc.bin", "demo_dec.bin", 2).get();
        std::vector<uint8_t> dec;
        {
            std::ifstream f("demo_dec.bin", std::ios::binary);
            dec.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
        }
        while (!dec.empty() && dec.back() == 0) dec.pop_back();
        std::cout << "  Async file MARS CBC Zeros 2 threads OK: " << (dec == plain ? "YES" : "NO") << "\n";
    }

    {
        std::cout << "\n[3] Twofish RandomDelta (IV >= 2*blockSize):\n";
        auto key = hexToBytes("000102030405060708090a0b0c0d0e0f");
        std::vector<uint8_t> iv(32);
        for (int i = 0; i < 32; i++) iv[i] = (uint8_t)i;
        Twofish tf(key);
        std::vector<uint8_t> plain(48, 0x77);
        CipherContext2 ctx(&tf, EncryptionMode2::RandomDelta, PaddingMode2::PKCS7, iv);
        std::vector<uint8_t> ct, dec;
        ctx.encrypt(plain, ct, 1);
        ctx.decrypt(ct, dec, 1);
        std::cout << "  RandomDelta roundtrip OK: " << (dec == plain ? "YES" : "NO") << "\n";
    }

    std::cout << "\n END DEMO \n";
}

int main() {
    std::cout << " Running Tests \n\n";

    testTwofishVectors();
    testTwofishInvertible();
    testMARSInvertible();
    testInvalidKey();
    testCipherContext2();
    testMultiThread();
    testAsyncFile();
    testEdgeCases();

    demo();

    std::cout << "\n Results: " << g_pass << " passed, " << g_fail << " failed \n";
    return g_fail == 0 ? 0 : 1;
}