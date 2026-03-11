#include <gtest/gtest.h>

#include "cipher_context2.h"
#include "../hw_3/xor_cipher.h"

#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>


static std::vector<uint8_t> makeKey(size_t n, uint8_t v = 0xAB) {
    return std::vector<uint8_t>(n, v);
}
static std::vector<uint8_t> makeData(size_t n, uint8_t v = 0x42) {
    return std::vector<uint8_t>(n, v);
}

static std::vector<uint8_t> doEnc(const CipherContext2& ctx,
    const std::vector<uint8_t>& in,
    int t = 1) {
    std::vector<uint8_t> out;
    ctx.encrypt(in, out, t);
    return out;
}
static std::vector<uint8_t> doDec(const CipherContext2& ctx,
    const std::vector<uint8_t>& in,
    int t = 1) {
    std::vector<uint8_t> out;
    ctx.decrypt(in, out, t);
    return out;
}

static std::string writeTmp(const std::string& path,
    const std::vector<uint8_t>& data) {
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(data.data()),
        static_cast<std::streamsize>(data.size()));
    return path;
}
static std::vector<uint8_t> readTmp(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    return { std::istreambuf_iterator<char>(f),
            std::istreambuf_iterator<char>() };
}


class Pkcs7Test : public ::testing::Test {
protected:
    Pkcs7Padding pad;
};

TEST_F(Pkcs7Test, PadsUnalignedData) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x03 };
    pad.pad(data, 8);
    ASSERT_EQ(data.size(), 8u);
    for (size_t i = 3; i < 8; ++i)
        EXPECT_EQ(data[i], 0x05) << "pad byte at " << i;
}

TEST_F(Pkcs7Test, PadsAlignedData_AddsFullBlock) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x03, 0x04 }; 
    pad.pad(data, 4);
    ASSERT_EQ(data.size(), 8u);
    for (size_t i = 4; i < 8; ++i)
        EXPECT_EQ(data[i], 0x04);
}

TEST_F(Pkcs7Test, PadsEmptyData) {
    std::vector<uint8_t> data;
    pad.pad(data, 4);
    ASSERT_EQ(data.size(), 4u);
    for (uint8_t b : data) EXPECT_EQ(b, 0x04);
}

TEST_F(Pkcs7Test, UnpadRestoresOriginalData) {
    std::vector<uint8_t> data = { 0xAA, 0xBB, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06 };
    pad.unpad(data, 8);
    ASSERT_EQ(data.size(), 2u);
    EXPECT_EQ(data[0], 0xAA);
    EXPECT_EQ(data[1], 0xBB);
}

TEST_F(Pkcs7Test, PadUnpadRoundtrip_Unaligned) {
    std::vector<uint8_t> original = { 0x10, 0x20, 0x30, 0x40, 0x50 };
    std::vector<uint8_t> data = original;
    pad.pad(data, 8);
    pad.unpad(data, 8);
    EXPECT_EQ(data, original);
}

TEST_F(Pkcs7Test, PadUnpadRoundtrip_Aligned) {
    std::vector<uint8_t> original = { 0x01, 0x02, 0x03, 0x04 };
    std::vector<uint8_t> data = original;
    pad.pad(data, 4);
    pad.unpad(data, 4);
    EXPECT_EQ(data, original);
}

TEST_F(Pkcs7Test, ThrowsOnZeroBlockSize) {
    std::vector<uint8_t> data = { 0x01 };
    EXPECT_THROW(pad.pad(data, 0), std::invalid_argument);
}

TEST_F(Pkcs7Test, ThrowsOnEmptyDataUnpad) {
    std::vector<uint8_t> data;
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}

TEST_F(Pkcs7Test, ThrowsOnPadLengthExceedsBlockSize) {
    std::vector<uint8_t> data = { 0x01, 0x01, 0x01, 0x09 };
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}

TEST_F(Pkcs7Test, ThrowsOnInconsistentPadBytes) {
    std::vector<uint8_t> data = { 0x01, 0xFF, 0x02 };
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}

TEST_F(Pkcs7Test, ThrowsOnZeroPadLengthByte) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x03, 0x00 };
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}



class Iso10126Test : public ::testing::Test {
protected:
    Iso10126Padding pad;

    void SetUp() override {
        std::srand(0); 
    }
};

TEST_F(Iso10126Test, LastByteEqualspadLength) {
    std::vector<uint8_t> data = { 0xAA, 0xBB, 0xCC };
    pad.pad(data, 8);
    ASSERT_EQ(data.size(), 8u);
    EXPECT_EQ(data.back(), 5u);
}

TEST_F(Iso10126Test, PadsAlignedData_AddsFullBlock) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x03, 0x04 };
    pad.pad(data, 4);
    ASSERT_EQ(data.size(), 8u);
    EXPECT_EQ(data.back(), 4u);
}

TEST_F(Iso10126Test, UnpadRestoresOriginalLength) {
    std::vector<uint8_t> original = { 0xDE, 0xAD, 0xBE };
    std::vector<uint8_t> data = original;
    pad.pad(data, 8);
    pad.unpad(data, 8);
    EXPECT_EQ(data, original);
}

TEST_F(Iso10126Test, PadUnpadRoundtrip_Unaligned) {
    std::vector<uint8_t> original = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    std::vector<uint8_t> data = original;
    pad.pad(data, 8);
    pad.unpad(data, 8);
    EXPECT_EQ(data, original);
}

TEST_F(Iso10126Test, PadUnpadRoundtrip_Aligned) {
    std::vector<uint8_t> original = { 0xAA, 0xBB, 0xCC, 0xDD };
    std::vector<uint8_t> data = original;
    pad.pad(data, 4);
    pad.unpad(data, 4);
    EXPECT_EQ(data, original);
}

TEST_F(Iso10126Test, PadUnpadRoundtrip_EmptyData) {
    std::vector<uint8_t> original;
    std::vector<uint8_t> data = original;
    pad.pad(data, 4);
    pad.unpad(data, 4);
    EXPECT_EQ(data, original);
}

TEST_F(Iso10126Test, ThrowsOnZeroBlockSize) {
    std::vector<uint8_t> data = { 0x01 };
    EXPECT_THROW(pad.pad(data, 0), std::invalid_argument);
}

TEST_F(Iso10126Test, ThrowsOnEmptyDataUnpad) {
    std::vector<uint8_t> data;
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}

TEST_F(Iso10126Test, ThrowsOnPadLengthExceedsBlockSize) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x03, 0x09 };
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}

TEST_F(Iso10126Test, ThrowsOnZeroPadLengthByte) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x03, 0x00 };
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}



class ModeTest : public ::testing::Test {
protected:
    XorCipher algo{ makeKey(8, 0x5A) };
    std::vector<uint8_t> iv{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    std::vector<uint8_t> iv2{ 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88 };

    std::vector<uint8_t> iv16 = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                             0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10 };
    std::vector<uint8_t> iv16_2 = { 0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,
                                   0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00 };
};



TEST_F(ModeTest, CFB_Pkcs7_Roundtrip) {
    CipherContext2 ctx(&algo, EncryptionMode2::CFB, PaddingMode2::PKCS7, iv16);
    auto plain = makeData(24, 0x42);
    EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain);
}

TEST_F(ModeTest, CFB_Roundtrip_UnalignedInput) {
    CipherContext2 ctx(&algo, EncryptionMode2::CFB, PaddingMode2::PKCS7, iv16);
    std::vector<uint8_t> plain = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain);
}

TEST_F(ModeTest, CFB_DifferentIV_DifferentCiphertext) {
    CipherContext2 ctx1(&algo, EncryptionMode2::CFB, PaddingMode2::Zeros, iv16);
    CipherContext2 ctx2(&algo, EncryptionMode2::CFB, PaddingMode2::Zeros, iv16_2);
    auto plain = makeData(16, 0xAB);
    EXPECT_NE(doEnc(ctx1, plain), doEnc(ctx2, plain));
}

TEST_F(ModeTest, CFB_WrongDecryptKey_DoesNotRestorePlaintext) {
    CipherContext2 ctxEnc(&algo, EncryptionMode2::CFB, PaddingMode2::Zeros, iv16);
    CipherContext2 ctxDec(&algo, EncryptionMode2::CFB, PaddingMode2::Zeros, iv16_2);
    auto plain = makeData(16, 0x33);
    EXPECT_NE(doDec(ctxDec, doEnc(ctxEnc, plain)), plain);
}

TEST_F(ModeTest, CFB_BitFlipInBlock_CorruptsCurrentAndNextBlock) {
    CipherContext2 ctx(&algo, EncryptionMode2::CFB, PaddingMode2::Zeros, iv16);
    auto plain = makeData(16, 0xCC);
    auto ciphertext = doEnc(ctx, plain);
    ciphertext[0] ^= 0xFF;
    EXPECT_NE(doDec(ctx, ciphertext), plain);
}



TEST_F(ModeTest, OFB_Pkcs7_Roundtrip) {
    CipherContext2 ctx(&algo, EncryptionMode2::OFB, PaddingMode2::PKCS7, iv16);
    auto plain = makeData(24, 0x55);
    EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain);
}

TEST_F(ModeTest, OFB_Roundtrip_UnalignedInput) {
    CipherContext2 ctx(&algo, EncryptionMode2::OFB, PaddingMode2::PKCS7, iv16);
    std::vector<uint8_t> plain = { 0xAA, 0xBB, 0xCC };
    EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain);
}

TEST_F(ModeTest, OFB_IsSymmetric_EncryptEqualsDecrypt) {
    CipherContext2 ctx(&algo, EncryptionMode2::OFB, PaddingMode2::Zeros, iv16);
    auto plain = makeData(16, 0x77);
    auto enc = doEnc(ctx, plain);
    auto redec = doEnc(ctx, enc);
    EXPECT_EQ(redec, plain);
}

TEST_F(ModeTest, OFB_DifferentIV_DifferentCiphertext) {
    CipherContext2 ctx1(&algo, EncryptionMode2::OFB, PaddingMode2::Zeros, iv16);
    CipherContext2 ctx2(&algo, EncryptionMode2::OFB, PaddingMode2::Zeros, iv16_2);
    auto plain = makeData(8, 0x11);
    EXPECT_NE(doEnc(ctx1, plain), doEnc(ctx2, plain));
}

TEST_F(ModeTest, OFB_BitFlipInCiphertext_OnlyCorruptsCurrentBlock) {
    CipherContext2 ctx(&algo, EncryptionMode2::OFB, PaddingMode2::Zeros, iv16);
    auto plain = makeData(16, 0x22);
    auto ciphertext = doEnc(ctx, plain);
    ciphertext[0] ^= 0xFF; 
    auto decrypted = doDec(ctx, ciphertext);
    std::vector<uint8_t> secondBlock(decrypted.begin() + 8, decrypted.end());
    std::vector<uint8_t> originalSecond(plain.begin() + 8, plain.end());
    EXPECT_EQ(secondBlock, originalSecond);
}



TEST_F(ModeTest, CTR_Pkcs7_Roundtrip) {
    CipherContext2 ctx(&algo, EncryptionMode2::CTR, PaddingMode2::PKCS7, iv16);
    auto plain = makeData(24, 0x99);
    EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain);
}

TEST_F(ModeTest, CTR_Roundtrip_UnalignedInput) {
    CipherContext2 ctx(&algo, EncryptionMode2::CTR, PaddingMode2::PKCS7, iv16);
    std::vector<uint8_t> plain = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain);
}

TEST_F(ModeTest, CTR_IsSymmetric_EncryptEqualsDecrypt) {
    CipherContext2 ctx(&algo, EncryptionMode2::CTR, PaddingMode2::Zeros, iv16);
    auto plain = makeData(16, 0xAA);
    auto enc = doEnc(ctx, plain);
    EXPECT_EQ(doEnc(ctx, enc), plain);
}

TEST_F(ModeTest, CTR_MultiThread_MatchesSingleThread_Encrypt) {
    CipherContext2 ctx(&algo, EncryptionMode2::CTR, PaddingMode2::Zeros, iv16);
    auto plain = makeData(64, 0x55);
    EXPECT_EQ(doEnc(ctx, plain, 1), doEnc(ctx, plain, 4));
}

TEST_F(ModeTest, CTR_MultiThread_MatchesSingleThread_Decrypt) {
    CipherContext2 ctx(&algo, EncryptionMode2::CTR, PaddingMode2::Zeros, iv16);
    auto plain = makeData(64, 0x66);
    auto ciphertext = doEnc(ctx, plain, 1);
    EXPECT_EQ(doDec(ctx, ciphertext, 1), doDec(ctx, ciphertext, 4));
}

TEST_F(ModeTest, CTR_DifferentIV_DifferentCiphertext) {
    CipherContext2 ctx1(&algo, EncryptionMode2::CTR, PaddingMode2::Zeros, iv16);
    CipherContext2 ctx2(&algo, EncryptionMode2::CTR, PaddingMode2::Zeros, iv16_2);
    auto plain = makeData(16, 0x77);
    EXPECT_NE(doEnc(ctx1, plain), doEnc(ctx2, plain));
}

TEST_F(ModeTest, CTR_DifferentBlocks_HaveDifferentKeystreams) {
    CipherContext2 ctx(&algo, EncryptionMode2::CTR, PaddingMode2::Zeros, iv16);
    std::vector<uint8_t> plain(16, 0x00);
    auto enc = doEnc(ctx, plain);

    std::vector<uint8_t> block0(enc.begin(), enc.begin() + 8);
    std::vector<uint8_t> block1(enc.begin() + 8, enc.end());
    EXPECT_NE(block0, block1);
}



TEST_F(ModeTest, RandomDelta_Pkcs7_Roundtrip) {
    CipherContext2 ctx(&algo, EncryptionMode2::RandomDelta, PaddingMode2::PKCS7, iv16);
    auto plain = makeData(24, 0xBB);
    EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain);
}

TEST_F(ModeTest, RandomDelta_Roundtrip_UnalignedInput) {
    CipherContext2 ctx(&algo, EncryptionMode2::RandomDelta, PaddingMode2::PKCS7, iv16);
    std::vector<uint8_t> plain = { 0x11, 0x22, 0x33 };
    EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain);
}

TEST_F(ModeTest, RandomDelta_MultiThread_MatchesSingleThread_Encrypt) {
    CipherContext2 ctx(&algo, EncryptionMode2::RandomDelta, PaddingMode2::Zeros, iv16);
    auto plain = makeData(64, 0xCC);
    EXPECT_EQ(doEnc(ctx, plain, 1), doEnc(ctx, plain, 4));
}

TEST_F(ModeTest, RandomDelta_MultiThread_MatchesSingleThread_Decrypt) {
    CipherContext2 ctx(&algo, EncryptionMode2::RandomDelta, PaddingMode2::Zeros, iv16);
    auto plain = makeData(64, 0xDD);
    auto ciphertext = doEnc(ctx, plain, 1);
    EXPECT_EQ(doDec(ctx, ciphertext, 1), doDec(ctx, ciphertext, 4));
}

TEST_F(ModeTest, RandomDelta_DifferentIV_DifferentCiphertext) {
    CipherContext2 ctx1(&algo, EncryptionMode2::RandomDelta, PaddingMode2::Zeros, iv16);
    CipherContext2 ctx2(&algo, EncryptionMode2::RandomDelta, PaddingMode2::Zeros, iv16_2);
    auto plain = makeData(8, 0xEE);
    EXPECT_NE(doEnc(ctx1, plain), doEnc(ctx2, plain));
}

TEST_F(ModeTest, RandomDelta_And_CTR_DifferentIV_DifferentOutput) {
    CipherContext2 ctxCTR(&algo, EncryptionMode2::CTR, PaddingMode2::Zeros, iv16);
    CipherContext2 ctxRD(&algo, EncryptionMode2::RandomDelta, PaddingMode2::Zeros, iv16);
    auto plain = makeData(16, 0xFF);
    EXPECT_NE(doEnc(ctxCTR, plain), doEnc(ctxRD, plain));
}



class AllNewCombinationsTest : public ::testing::Test {
protected:
    XorCipher algo{ makeKey(8, 0x3C) };
    std::vector<uint8_t> iv{ 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22 };
};

struct ModeAndPad {
    EncryptionMode2 mode;
    PaddingMode2    pad;
    const char* name;
};

static const ModeAndPad kCombinations[] = {
    {EncryptionMode2::CFB, PaddingMode2::PKCS7, "CFB+PKCS7"},
    {EncryptionMode2::CFB, PaddingMode2::ISO10126,"CFB+ISO10126"},
    {EncryptionMode2::OFB, PaddingMode2::PKCS7, "OFB+PKCS7"},
    {EncryptionMode2::OFB, PaddingMode2::ISO10126, "OFB+ISO10126"},
    {EncryptionMode2::CTR, PaddingMode2::PKCS7, "CTR+PKCS7"},
    {EncryptionMode2::CTR, PaddingMode2::ISO10126, "CTR+ISO10126"},
    {EncryptionMode2::RandomDelta, PaddingMode2::PKCS7, "RandomDelta+PKCS7"},
    {EncryptionMode2::RandomDelta, PaddingMode2::ISO10126, "RandomDelta+ISO10126"},
};

TEST_F(AllNewCombinationsTest, Roundtrip_Aligned) {
    for (const auto& c : kCombinations) {
        std::vector<uint8_t> iv16 = { 0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x11,0x22,
                             0x33,0x44,0x55,0x66,0x77,0x88,0x99,0x00 };
        CipherContext2 ctx(&algo, c.mode, c.pad, iv16);
        auto plain = makeData(16, 0x42);
        EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain) << "FAILED: " << c.name;
    }
}

TEST_F(AllNewCombinationsTest, Roundtrip_Unaligned) {
    for (const auto& c : kCombinations) {
        std::vector<uint8_t> iv16 = { 0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x11,0x22,
                             0x33,0x44,0x55,0x66,0x77,0x88,0x99,0x00 };
        CipherContext2 ctx(&algo, c.mode, c.pad, iv16);
        std::vector<uint8_t> plain = { 0x01, 0x02, 0x03, 0x04, 0x05 };
        EXPECT_EQ(doDec(ctx, doEnc(ctx, plain)), plain) << "FAILED: " << c.name;
    }
}



class ConstructionTest : public ::testing::Test {
protected:
    XorCipher algo{ makeKey(8) };
};

TEST_F(ConstructionTest, ThrowsOnNullAlgorithm) {
    EXPECT_THROW(
        (CipherContext2(nullptr, EncryptionMode2::CFB, PaddingMode2::PKCS7)),
        std::invalid_argument
    );
}

TEST_F(ConstructionTest, ConstructsAllNewModes) {
    std::vector<uint8_t> iv(8, 0x00);
    EXPECT_NO_THROW((CipherContext2(&algo, EncryptionMode2::CFB, PaddingMode2::PKCS7, iv)));
    EXPECT_NO_THROW((CipherContext2(&algo, EncryptionMode2::OFB, PaddingMode2::PKCS7, iv)));
    EXPECT_NO_THROW((CipherContext2(&algo, EncryptionMode2::CTR, PaddingMode2::ISO10126, iv)));
    EXPECT_NO_THROW((CipherContext2(&algo, EncryptionMode2::RandomDelta, PaddingMode2::ISO10126, iv)));
}



class FileTest : public ::testing::Test {
protected:
    XorCipher algo{ makeKey(8, 0x7F) };
    std::vector<uint8_t> iv16 = { 0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
                             0x90,0xA0,0xB0,0xC0,0xD0,0xE0,0xF0,0x00 };
    std::vector<uint8_t> iv{ 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80 };

    const std::string pathPlain = "lab3ext_plain.bin";
    const std::string pathEnc = "lab3ext_enc.bin";
    const std::string pathDec = "lab3ext_dec.bin";

    void TearDown() override {
        std::remove(pathPlain.c_str());
        std::remove(pathEnc.c_str());
        std::remove(pathDec.c_str());
    }
};

TEST_F(FileTest, CFB_FileRoundtrip) {
    auto plain = makeData(16, 0x11);
    writeTmp(pathPlain, plain);
    CipherContext2 ctx(&algo, EncryptionMode2::CFB, PaddingMode2::PKCS7, iv16);
    ctx.encrypt(pathPlain, pathEnc).get();
    ctx.decrypt(pathEnc, pathDec).get();
    EXPECT_EQ(readTmp(pathDec), plain);
}

TEST_F(FileTest, OFB_FileRoundtrip) {
    auto plain = makeData(16, 0x22);
    writeTmp(pathPlain, plain);
    CipherContext2 ctx(&algo, EncryptionMode2::OFB, PaddingMode2::PKCS7, iv16);
    ctx.encrypt(pathPlain, pathEnc).get();
    ctx.decrypt(pathEnc, pathDec).get();
    EXPECT_EQ(readTmp(pathDec), plain);
}

TEST_F(FileTest, CTR_FileRoundtrip) {
    auto plain = makeData(32, 0x33);
    writeTmp(pathPlain, plain);
    CipherContext2 ctx(&algo, EncryptionMode2::CTR, PaddingMode2::ISO10126, iv16);
    ctx.encrypt(pathPlain, pathEnc).get();
    ctx.decrypt(pathEnc, pathDec).get();
    EXPECT_EQ(readTmp(pathDec), plain);
}

TEST_F(FileTest, RandomDelta_FileRoundtrip) {
    auto plain = makeData(24, 0x44);
    writeTmp(pathPlain, plain);
    CipherContext2 ctx(&algo, EncryptionMode2::RandomDelta, PaddingMode2::ISO10126, iv16);
    ctx.encrypt(pathPlain, pathEnc).get();
    ctx.decrypt(pathEnc, pathDec).get();
    EXPECT_EQ(readTmp(pathDec), plain);
}

TEST_F(FileTest, CTR_FileRoundtrip_MultiThread) {
    auto plain = makeData(64, 0x55);
    writeTmp(pathPlain, plain);
    CipherContext2 ctx(&algo, EncryptionMode2::CTR, PaddingMode2::PKCS7, iv16);
    ctx.encrypt(pathPlain, pathEnc, /*numThreads=*/4).get();
    ctx.decrypt(pathEnc, pathDec, /*numThreads=*/4).get();
    EXPECT_EQ(readTmp(pathDec), plain);
}

TEST_F(FileTest, FileEncrypt_ThrowsOnMissingFile) {
    CipherContext2 ctx(&algo, EncryptionMode2::CFB, PaddingMode2::PKCS7, iv16);
    EXPECT_THROW(
        ctx.encrypt("/no/such/file.bin", pathEnc).get(),
        std::runtime_error
    );
}

TEST_F(FileTest, FileDecrypt_ThrowsOnMissingFile) {
    CipherContext2 ctx(&algo, EncryptionMode2::OFB, PaddingMode2::PKCS7, iv16);
    EXPECT_THROW(
        ctx.decrypt("/no/such/file.bin", pathDec).get(),
        std::runtime_error
    );
}



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}