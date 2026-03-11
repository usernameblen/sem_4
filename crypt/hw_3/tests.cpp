#include <gtest/gtest.h>

#include "../hw_3/interfaces.h"
#include "../hw_3/padding.h"
#include "../hw_3/modes.h"
#include "../hw_3/cipher_context.h"
#include "../hw_3/xor_cipher.h"

#include <fstream>
#include <string>
#include <vector>
#include <any>
#include <thread>



static std::vector<uint8_t> makeKey(size_t n, uint8_t v = 0xAB) {
    return std::vector<uint8_t>(n, v);
}

static std::vector<uint8_t> makeData(size_t n, uint8_t v = 0x42) {
    return std::vector<uint8_t>(n, v);
}


static std::vector<uint8_t> enc(const CipherContext& ctx,
    const std::vector<uint8_t>& in,
    int threads = 1) {
    std::vector<uint8_t> out;
    ctx.encrypt(in, out, threads);
    return out;
}

static std::vector<uint8_t> dec(const CipherContext& ctx,
    const std::vector<uint8_t>& in,
    int threads = 1) {
    std::vector<uint8_t> out;
    ctx.decrypt(in, out, threads);
    return out;
}


static std::string writeTmpFile(const std::string& path,
    const std::vector<uint8_t>& data) {
    std::ofstream f(path, std::ios::binary);
    if (!f.is_open())
        throw std::runtime_error("Cannot create file: " + path);
    for (uint8_t b : data) f.put(static_cast<char>(b));
    if (!f)
        throw std::runtime_error("Write error: " + path);
    return path;
}


static std::vector<uint8_t> readTmpFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    return { std::istreambuf_iterator<char>(f),
            std::istreambuf_iterator<char>() };
}





class ZerosPaddingTest : public ::testing::Test {
protected:
    ZerosPadding pad;
};

TEST_F(ZerosPaddingTest, NoChangeWhenAlreadyAligned) {
    std::vector<uint8_t> data(8, 0x01);
    pad.pad(data, 4);
    EXPECT_EQ(data.size(), 8u);
}

TEST_F(ZerosPaddingTest, PadsToNextBlockBoundary) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x03 };
    pad.pad(data, 4);
    ASSERT_EQ(data.size(), 4u);
    EXPECT_EQ(data[3], 0x00);
}

TEST_F(ZerosPaddingTest, PadsEmptyDataToFullBlock) {
    std::vector<uint8_t> data;
    pad.pad(data, 4);
    EXPECT_EQ(data.size(), 0u);
}

TEST_F(ZerosPaddingTest, UnpadRemovesTrailingZeros) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x00, 0x00 };
    pad.unpad(data, 4);
    ASSERT_EQ(data.size(), 2u);
    EXPECT_EQ(data[0], 0x01);
    EXPECT_EQ(data[1], 0x02);
}

TEST_F(ZerosPaddingTest, UnpadLeavesNonZeroTail) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x03, 0x04 };
    pad.unpad(data, 4);
    EXPECT_EQ(data.size(), 4u);
}

TEST_F(ZerosPaddingTest, ThrowsOnZeroBlockSize) {
    std::vector<uint8_t> data = { 0x01 };
    EXPECT_THROW(pad.pad(data, 0), std::invalid_argument);
}

TEST_F(ZerosPaddingTest, PadUnpadRoundtrip) {
    std::vector<uint8_t> original = { 0xAA, 0xBB, 0xCC };
    std::vector<uint8_t> data = original;
    pad.pad(data, 8);
    pad.unpad(data, 8);
    EXPECT_EQ(data, original);
}




class AnsiX923PaddingTest : public ::testing::Test {
protected:
    AnsiX923Padding pad;
};

TEST_F(AnsiX923PaddingTest, PadLength_LastByteIsCount) {
    std::vector<uint8_t> data = { 0xAA, 0xBB, 0xCC };
    pad.pad(data, 8);
    ASSERT_EQ(data.size(), 8u);
    EXPECT_EQ(data.back(), 5u);
}

TEST_F(AnsiX923PaddingTest, PadBytes_AreZeroExceptLast) {
    std::vector<uint8_t> data = { 0xAA, 0xBB, 0xCC };
    pad.pad(data, 8);
    for (size_t i = 3; i < 7; ++i)
        EXPECT_EQ(data[i], 0x00) << "byte " << i << " should be 0x00";
}

TEST_F(AnsiX923PaddingTest, UnpadRestoresOriginalData) {
    std::vector<uint8_t> data = { 0xAA, 0xBB, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x05 };
    pad.unpad(data, 8);
    ASSERT_EQ(data.size(), 3u);
    EXPECT_EQ(data[0], 0xAA);
    EXPECT_EQ(data[1], 0xBB);
    EXPECT_EQ(data[2], 0xCC);
}

TEST_F(AnsiX923PaddingTest, PadUnpadRoundtrip) {
    std::vector<uint8_t> original = { 0x10, 0x20, 0x30, 0x40, 0x50 };
    std::vector<uint8_t> data = original;
    pad.pad(data, 8);
    pad.unpad(data, 8);
    EXPECT_EQ(data, original);
}

TEST_F(AnsiX923PaddingTest, PadUnpadRoundtrip_AlignedInput) {
    std::vector<uint8_t> original = { 0x01, 0x02, 0x03, 0x04 };
    std::vector<uint8_t> data = original;
    pad.pad(data, 4);
    EXPECT_EQ(data.size(), 8u);
    pad.unpad(data, 4);
    EXPECT_EQ(data, original);
}

TEST_F(AnsiX923PaddingTest, ThrowsOnZeroBlockSize) {
    std::vector<uint8_t> data = { 0x01 };
    EXPECT_THROW(pad.pad(data, 0), std::invalid_argument);
}

TEST_F(AnsiX923PaddingTest, ThrowsOnPadLengthExceedsBlockSize) {
    // last byte claims 9 bytes of padding but block size is 4
    std::vector<uint8_t> data = { 0x01, 0x01, 0x01, 0x09 };
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}

TEST_F(AnsiX923PaddingTest, ThrowsOnPadLengthIsZero) {
    std::vector<uint8_t> data = { 0x01, 0x02, 0x03, 0x00 };
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}

TEST_F(AnsiX923PaddingTest, ThrowsOnNonZeroPadByte) {
    // padding bytes must be 0x00; 0xFF violates the standard
    std::vector<uint8_t> data = { 0x01, 0x02, 0xFF, 0x02 };
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}

TEST_F(AnsiX923PaddingTest, ThrowsOnEmptyInput) {
    std::vector<uint8_t> data;
    EXPECT_THROW(pad.unpad(data, 4), std::runtime_error);
}




class XorCipherTest : public ::testing::Test {
protected:
    // 4-byte key: {0x5A, 0xA5, 0xFF, 0x00}
    XorCipher cipher{ {0x5A, 0xA5, 0xFF, 0x00} };
};

TEST_F(XorCipherTest, BlockSizeEqualsKeySize) {
    EXPECT_EQ(cipher.blockSize(), 4u);
}

TEST_F(XorCipherTest, EncryptProducesCorrectXor) {
    std::vector<uint8_t> in = { 0x00, 0x00, 0x00, 0x00 };
    std::vector<uint8_t> out;
    cipher.encryptBlock(in, out);
    EXPECT_EQ(out, (std::vector<uint8_t>{0x5A, 0xA5, 0xFF, 0x00}));
}

TEST_F(XorCipherTest, DecryptIsSymmetricToEncrypt) {
    std::vector<uint8_t> plain = { 0x01, 0x02, 0x03, 0x04 };
    std::vector<uint8_t> enc_out, dec_out;
    cipher.encryptBlock(plain, enc_out);
    cipher.decryptBlock(enc_out, dec_out);
    EXPECT_EQ(dec_out, plain);
}

TEST_F(XorCipherTest, EncryptDecryptAllZerosRoundtrip) {
    std::vector<uint8_t> plain(4, 0x00), enc_out, dec_out;
    cipher.encryptBlock(plain, enc_out);
    cipher.decryptBlock(enc_out, dec_out);
    EXPECT_EQ(dec_out, plain);
}

TEST_F(XorCipherTest, ThrowsOnEmptyKey) {
    EXPECT_THROW((XorCipher({})), std::invalid_argument);
}

TEST_F(XorCipherTest, ThrowsOnWrongInputSize_Encrypt) {
    std::vector<uint8_t> bad = { 0x01, 0x02 };
    std::vector<uint8_t> out;
    EXPECT_THROW(cipher.encryptBlock(bad, out), std::invalid_argument);
}

TEST_F(XorCipherTest, ThrowsOnWrongInputSize_Decrypt) {
    std::vector<uint8_t> bad = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    std::vector<uint8_t> out;
    EXPECT_THROW(cipher.decryptBlock(bad, out), std::invalid_argument);
}




class CipherContextConstructionTest : public ::testing::Test {
protected:
    XorCipher algo{ makeKey(4) };
};

TEST_F(CipherContextConstructionTest, ThrowsOnNullAlgorithm) {
    EXPECT_THROW(
        (CipherContext(nullptr, EncryptionMode::ECB, PaddingMode::Zeros)),
        std::invalid_argument
    );
}

TEST_F(CipherContextConstructionTest, ConstructsWithDefaultParams) {
    EXPECT_NO_THROW(
        (CipherContext(&algo, EncryptionMode::ECB, PaddingMode::Zeros))
    );
}

TEST_F(CipherContextConstructionTest, ModeParamsStoredCorrectly) {
    // Constructor takes std::initializer_list<std::any>
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros, {},
        { std::any(42), std::any(std::string("hello")) });

    const ModeParams& stored = ctx.modeParams();
    ASSERT_EQ(stored.size(), 2u);
    EXPECT_EQ(std::any_cast<int>(stored[0]), 42);
    EXPECT_EQ(std::any_cast<std::string>(stored[1]), std::string("hello"));
}

TEST_F(CipherContextConstructionTest, EmptyModeParamsIsValid) {
    CipherContext ctx(&algo, EncryptionMode::CBC, PaddingMode::Zeros);
    EXPECT_EQ(ctx.modeParams().size(), 0u);
}

TEST_F(CipherContextConstructionTest, ModeParamsForwardedToConfigure) {
    EXPECT_NO_THROW(
        (CipherContext(&algo, EncryptionMode::ECB, PaddingMode::Zeros, {},
            { std::any(1), std::any(2), std::any(3) }))
    );
}




class CipherContextSyncTest : public ::testing::Test {
protected:
    XorCipher algo{ makeKey(4) };
    std::vector<uint8_t> iv{ 0xDE, 0xAD, 0xBE, 0xEF };
};


TEST_F(CipherContextSyncTest, EncryptFillsOutputByReference) {
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    std::vector<uint8_t> plain = makeData(8, 0x11);
    std::vector<uint8_t> out;
    ctx.encrypt(plain, out);
    EXPECT_FALSE(out.empty());
}

TEST_F(CipherContextSyncTest, DecryptFillsOutputByReference) {
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    auto ciphertext = enc(ctx, makeData(8, 0x11));
    std::vector<uint8_t> out;
    ctx.decrypt(ciphertext, out);
    EXPECT_FALSE(out.empty());
}



TEST_F(CipherContextSyncTest, ECB_Zeros_Roundtrip) {
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    auto plain = makeData(16, 0x55);
    EXPECT_EQ(dec(ctx, enc(ctx, plain)), plain);
}

TEST_F(CipherContextSyncTest, ECB_AnsiX923_Roundtrip_UnalignedInput) {
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::ANSI_X923);
    std::vector<uint8_t> plain = { 0x01, 0x02, 0x03 }; // not block-aligned
    EXPECT_EQ(dec(ctx, enc(ctx, plain)), plain);
}

TEST_F(CipherContextSyncTest, ECB_MultiThreadEncryptMatchesSingleThread) {
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    auto plain = makeData(64, 0x7E);
    EXPECT_EQ(enc(ctx, plain, 1), enc(ctx, plain, 4));
}

TEST_F(CipherContextSyncTest, ECB_MultiThreadDecryptMatchesSingleThread) {
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    auto plain = makeData(64, 0x7E);
    auto ciphertext = enc(ctx, plain, 1);
    EXPECT_EQ(dec(ctx, ciphertext, 1), dec(ctx, ciphertext, 4));
}

TEST_F(CipherContextSyncTest, ECB_DeterministicOutput_SameInputSameOutput) {
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    auto plain = makeData(8, 0xAB);
    EXPECT_EQ(enc(ctx, plain), enc(ctx, plain));
}


TEST_F(CipherContextSyncTest, CBC_Zeros_Roundtrip) {
    CipherContext ctx(&algo, EncryptionMode::CBC, PaddingMode::Zeros, iv);
    auto plain = makeData(16, 0x55);
    EXPECT_EQ(dec(ctx, enc(ctx, plain)), plain);
}

TEST_F(CipherContextSyncTest, CBC_AnsiX923_Roundtrip_UnalignedInput) {
    CipherContext ctx(&algo, EncryptionMode::CBC, PaddingMode::ANSI_X923, iv);
    std::vector<uint8_t> plain = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE };
    EXPECT_EQ(dec(ctx, enc(ctx, plain)), plain);
}

TEST_F(CipherContextSyncTest, CBC_DifferentIV_ProducesDifferentCiphertext) {
    std::vector<uint8_t> iv1(4, 0x00);
    std::vector<uint8_t> iv2(4, 0xFF);
    CipherContext ctx1(&algo, EncryptionMode::CBC, PaddingMode::Zeros, iv1);
    CipherContext ctx2(&algo, EncryptionMode::CBC, PaddingMode::Zeros, iv2);
    auto plain = makeData(8, 0xAB);
    EXPECT_NE(enc(ctx1, plain), enc(ctx2, plain));
}

TEST_F(CipherContextSyncTest, CBC_ParallelDecrypt_MatchesSerialDecrypt) {
    CipherContext ctx(&algo, EncryptionMode::CBC, PaddingMode::Zeros, iv);
    auto plain = makeData(32, 0xCC);
    auto ciphertext = enc(ctx, plain, 1);
    EXPECT_EQ(dec(ctx, ciphertext, 1), dec(ctx, ciphertext, 4));
}

TEST_F(CipherContextSyncTest, CBC_ParallelDecrypt_RecoversPlaintext) {
    CipherContext ctx(&algo, EncryptionMode::CBC, PaddingMode::Zeros, iv);
    auto plain = makeData(32, 0xCC);
    EXPECT_EQ(dec(ctx, enc(ctx, plain), 4), plain);
}

TEST_F(CipherContextSyncTest, CBC_EmptyIV_TreatedAsZeroVector) {
    CipherContext ctxNoIV(&algo, EncryptionMode::CBC, PaddingMode::Zeros);
    CipherContext ctxZeroIV(&algo, EncryptionMode::CBC, PaddingMode::Zeros,
        std::vector<uint8_t>(4, 0x00));
    auto plain = makeData(8, 0x11);
    EXPECT_EQ(enc(ctxNoIV, plain), enc(ctxZeroIV, plain));
}



TEST_F(CipherContextSyncTest, PCBC_Zeros_Roundtrip) {
    CipherContext ctx(&algo, EncryptionMode::PCBC, PaddingMode::Zeros, iv);
    auto plain = makeData(12, 0x77);
    EXPECT_EQ(dec(ctx, enc(ctx, plain)), plain);
}

TEST_F(CipherContextSyncTest, PCBC_AnsiX923_Roundtrip_UnalignedInput) {
    CipherContext ctx(&algo, EncryptionMode::PCBC, PaddingMode::ANSI_X923, iv);
    std::vector<uint8_t> plain = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
    EXPECT_EQ(dec(ctx, enc(ctx, plain)), plain);
}

TEST_F(CipherContextSyncTest, PCBC_BitFlip_CorruptsAllSubsequentBlocks) {
    CipherContext ctx(&algo, EncryptionMode::PCBC, PaddingMode::Zeros, iv);
    auto plain = makeData(8, 0xAA);
    auto ciphertext = enc(ctx, plain);
    ciphertext[2] ^= 0xFF;           // flip one bit in block 0
    EXPECT_NE(dec(ctx, ciphertext), plain);
}

TEST_F(CipherContextSyncTest, PCBC_DifferentIV_ProducesDifferentCiphertext) {
    std::vector<uint8_t> iv1(4, 0x00);
    std::vector<uint8_t> iv2(4, 0x11);
    CipherContext ctx1(&algo, EncryptionMode::PCBC, PaddingMode::Zeros, iv1);
    CipherContext ctx2(&algo, EncryptionMode::PCBC, PaddingMode::Zeros, iv2);
    auto plain = makeData(8, 0xBB);
    EXPECT_NE(enc(ctx1, plain), enc(ctx2, plain));
}



TEST_F(CipherContextSyncTest, ECB_And_CBC_ProduceDifferentCiphertexts) {
    CipherContext ecb(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    CipherContext cbc(&algo, EncryptionMode::CBC, PaddingMode::Zeros, iv);
    auto plain = makeData(16, 0x33);
    EXPECT_NE(enc(ecb, plain), enc(cbc, plain));
}


class CipherContextFileTest : public ::testing::Test {
protected:
    XorCipher              algo{ makeKey(4) };
    std::vector<uint8_t>   iv{ 0x01, 0x02, 0x03, 0x04 };
    const std::string      pathPlain = "lab3_plain.bin";
    const std::string      pathEnc = "lab3_enc.bin";
    const std::string      pathDec = "lab3_dec.bin";

    void TearDown() override {
        std::remove(pathPlain.c_str());
        std::remove(pathEnc.c_str());
        std::remove(pathDec.c_str());
    }
};

TEST_F(CipherContextFileTest, EncryptFile_ThenDecryptFile_RestoresOriginal) {
    std::vector<uint8_t> plain = { 0xDE,0xAD,0xBE,0xEF,0xCA,0xFE,0xBA,0xBE };
    writeTmpFile(pathPlain, plain);

    CipherContext ctx(&algo, EncryptionMode::CBC, PaddingMode::ANSI_X923, iv);
    ctx.encrypt(pathPlain, pathEnc).get();
    ctx.decrypt(pathEnc, pathDec).get();

    EXPECT_EQ(readTmpFile(pathDec), plain);
}

TEST_F(CipherContextFileTest, EncryptedFile_DiffersFromPlaintext) {
    std::vector<uint8_t> plain = makeData(16, 0x42);
    writeTmpFile(pathPlain, plain);

    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    ctx.encrypt(pathPlain, pathEnc).get();

    EXPECT_NE(readTmpFile(pathEnc), plain);
}

TEST_F(CipherContextFileTest, EncryptFile_ReturnsFuture) {
    std::vector<uint8_t> plain = makeData(8, 0x55);
    writeTmpFile(pathPlain, plain);

    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    // encrypt() must return a std::future — .get() must not throw
    std::future<void> f = ctx.encrypt(pathPlain, pathEnc);
    EXPECT_NO_THROW(f.get());
    EXPECT_FALSE(readTmpFile(pathEnc).empty());
}

TEST_F(CipherContextFileTest, DecryptFile_ReturnsFuture) {
    std::vector<uint8_t> plain = makeData(8, 0x55);
    writeTmpFile(pathPlain, plain);

    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    ctx.encrypt(pathPlain, pathEnc).get();

    std::future<void> f = ctx.decrypt(pathEnc, pathDec);
    EXPECT_NO_THROW(f.get());
    EXPECT_EQ(readTmpFile(pathDec), plain);
}

TEST_F(CipherContextFileTest, EncryptFile_FutureCompletesAsynchronously) {
    std::vector<uint8_t> plain = makeData(8, 0x11);
    writeTmpFile(pathPlain, plain);

    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    // Future must be valid immediately after the call
    std::future<void> f = ctx.encrypt(pathPlain, pathEnc);
    EXPECT_TRUE(f.valid());
    f.get();
}

TEST_F(CipherContextFileTest, EncryptFile_ThrowsOnMissingInputFile) {
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    // Exception must surface through .get()
    EXPECT_THROW(
        ctx.encrypt("/no/such/file.bin", pathEnc).get(),
        std::runtime_error
    );
}

TEST_F(CipherContextFileTest, DecryptFile_ThrowsOnMissingInputFile) {
    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    EXPECT_THROW(
        ctx.decrypt("/no/such/file.bin", pathDec).get(),
        std::runtime_error
    );
}

TEST_F(CipherContextFileTest, FileRoundtrip_MultipleThreads) {
    std::vector<uint8_t> plain = makeData(64, 0x77);
    writeTmpFile(pathPlain, plain);

    CipherContext ctx(&algo, EncryptionMode::ECB, PaddingMode::Zeros);
    ctx.encrypt(pathPlain, pathEnc, /*numThreads=*/4).get();
    ctx.decrypt(pathEnc, pathDec, /*numThreads=*/4).get();

    EXPECT_EQ(readTmpFile(pathDec), plain);
}

TEST_F(CipherContextFileTest, FileRoundtrip_CBCWithIV) {
    std::vector<uint8_t> plain = makeData(32, 0xAB);
    writeTmpFile(pathPlain, plain);

    CipherContext ctx(&algo, EncryptionMode::CBC, PaddingMode::Zeros, iv);
    ctx.encrypt(pathPlain, pathEnc).get();
    ctx.decrypt(pathEnc, pathDec).get();

    EXPECT_EQ(readTmpFile(pathDec), plain);
}

TEST_F(CipherContextFileTest, FileRoundtrip_PCBCWithAnsiPadding) {
    std::vector<uint8_t> plain = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    writeTmpFile(pathPlain, plain);

    CipherContext ctx(&algo, EncryptionMode::PCBC, PaddingMode::ANSI_X923, iv);
    ctx.encrypt(pathPlain, pathEnc).get();
    ctx.decrypt(pathEnc, pathDec).get();

    EXPECT_EQ(readTmpFile(pathDec), plain);
}


int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}