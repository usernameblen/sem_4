#include "rijndael.h"
#include "test.hpp"

#include "../hw_4/cipher_context2.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <stdexcept>

static void printHex(const std::string& label, const std::vector<uint8_t>& data)
{
    std::cout << label << ": ";
    for (auto b : data) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    std::cout << std::dec << "\n";
}

static std::vector<uint8_t> makeIV(size_t len)
{
    std::vector<uint8_t> iv(len);
    for (size_t i = 0; i < len; ++i) 
        iv[i] = static_cast<uint8_t>(i + 1);
    return iv;
}

static std::vector<uint8_t> makeRandomDeltaIV(size_t blockSize)
{
    std::vector<uint8_t> iv(2 * blockSize);
    for (size_t i = 0; i < blockSize; ++i)        iv[i] = static_cast<uint8_t>(i + 1);
    for (size_t i = blockSize; i < 2 * blockSize; ++i) iv[i] = static_cast<uint8_t>(i - blockSize + 5);
    return iv;
}

struct ModeEntry { EncryptionMode2 mode; const char* name; bool needsIV; bool isRandomDelta; };
struct PadEntry { PaddingMode2 pad;   const char* name; };

static void demoSyncAllCombinations()
{
    std::cout << "\n=== Sync encrypt/decrypt: all modes x paddings x thread counts ===\n";

    std::vector<uint8_t> key16(16, 0x2B);
    RijndaelAlgorithm algo(key16, 16);

    std::vector<uint8_t> plaintext(48);
    for (size_t i = 0; i < plaintext.size(); ++i) 
        plaintext[i] = static_cast<uint8_t>(i);

    ModeEntry modes[] = {
        {EncryptionMode2::ECB,         "ECB",         false, false},
        {EncryptionMode2::CBC,         "CBC",         true,  false},
        {EncryptionMode2::PCBC,        "PCBC",        true,  false},
        {EncryptionMode2::CFB,         "CFB",         true,  false},
        {EncryptionMode2::OFB,         "OFB",         true,  false},
        {EncryptionMode2::CTR,         "CTR",         true,  false},
        {EncryptionMode2::RandomDelta, "RandomDelta", true,  true },
    };

    PadEntry pads[] = {
        {PaddingMode2::Zeros,     "Zeros"    },
        {PaddingMode2::ANSI_X923,"ANSI_X923"},
        {PaddingMode2::PKCS7,    "PKCS7"    },
        {PaddingMode2::ISO10126, "ISO10126" },
    };

    int threads[] = { 1, 2, 4 };

    for (auto& m : modes)
        for (auto& p : pads)
        {
            std::vector<uint8_t> iv = m.isRandomDelta ? makeRandomDeltaIV(16)
                : m.needsIV ? makeIV(16)
                : std::vector<uint8_t>{};

            std::string label = std::string(m.name) + "+" + p.name;

            std::vector<uint8_t> ref_dec;
            bool first = true;

            for (int nt : threads)
            {
                try
                {
                    CipherContext2 ctx(&algo, m.mode, p.pad, iv);
                    std::vector<uint8_t> enc, dec;
                    ctx.encrypt(plaintext, enc, nt);
                    ctx.decrypt(enc, dec, nt);

                    if (dec != plaintext)
                    {
                        std::cout << "FAIL " << label << " threads=" << nt << " (roundtrip mismatch)\n";
                        continue;
                    }

                    if (first) 
                    { 
                        ref_dec = dec; 
                        first = false; 
                    }
                    else if (dec != ref_dec)
                    {
                        std::cout << "FAIL " << label << " threads=" << nt << " (thread result differs)\n";
                        continue;
                    }

                    std::cout << "PASS " << label << " threads=" << nt << "\n";
                }
                catch (const std::exception& e)
                {
                    std::cout << "SKIP " << label << " threads=" << nt << " (" << e.what() << ")\n";
                }
            }
        }
}

static void demoAllBlockKeySizes()
{
    std::cout << "\n=== Sync demo: all block x key size combinations ===\n";

    size_t keySizes[] = { 16, 24, 32 };
    size_t blockSizes[] = { 16, 24, 32 };

    for (size_t ks : keySizes)
        for (size_t bs : blockSizes)
        {
            std::vector<uint8_t> key(ks, 0xAB);
            RijndaelAlgorithm algo(key, bs);

            std::vector<uint8_t> pt(bs * 2);
            for (size_t i = 0; i < pt.size(); ++i) pt[i] = static_cast<uint8_t>(i);

            std::vector<uint8_t> iv = makeIV(bs);
            CipherContext2 ctx(&algo, EncryptionMode2::CBC, PaddingMode2::PKCS7, iv);

            std::vector<uint8_t> enc, dec;
            ctx.encrypt(pt, enc);
            ctx.decrypt(enc, dec);

            bool ok = (dec == pt);
            std::cout << (ok ? "PASS" : "FAIL")
                << " block=" << bs * 8 << " key=" << ks * 8 << " CBC+PKCS7\n";
        }
}

static void writeTestFile(const std::string& path, const std::vector<uint8_t>& data)
{
    std::ofstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot write: " + path);
    f.write(reinterpret_cast<const char*>(data.data()),
        static_cast<std::streamsize>(data.size()));
}

static std::vector<uint8_t> readTestFile(const std::string& path)
{
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) throw std::runtime_error("Cannot read: " + path);
    std::streamsize sz = f.tellg();
    f.seekg(0);
    std::vector<uint8_t> buf(static_cast<size_t>(sz));
    f.read(reinterpret_cast<char*>(buf.data()), sz);
    return buf;
}

static void demoAsyncFile()
{
    std::cout << "\n=== Async file encrypt/decrypt ===\n";

    std::vector<uint8_t> original(256);
    for (size_t i = 0; i < original.size(); ++i) original[i] = static_cast<uint8_t>(i);

    const std::string plain_path = "rijndael_plain.bin";
    const std::string enc_path = "rijndael_enc.bin";
    const std::string dec_path = "rijndael_dec.bin";

    writeTestFile(plain_path, original);

    std::vector<uint8_t> key(16, 0x3A);
    RijndaelAlgorithm algo(key, 16);
    std::vector<uint8_t> iv = makeIV(16);

    CipherContext2 ctx(&algo, EncryptionMode2::CBC, PaddingMode2::PKCS7, iv);

    auto fenc = ctx.encrypt(plain_path, enc_path, 2);
    fenc.get();
    std::cout << "Async encrypt done\n";

    auto fdec = ctx.decrypt(enc_path, dec_path, 2);
    fdec.get();
    std::cout << "Async decrypt done\n";

    std::vector<uint8_t> recovered = readTestFile(dec_path);
    if (recovered == original)
        std::cout << "PASS async file roundtrip\n";
    else
        std::cout << "FAIL async file roundtrip\n";
}

int main()
{
    try
    {
        RijndaelTests::runAll();
        demoSyncAllCombinations();
        demoAllBlockKeySizes();
        demoAsyncFile();


        std::cout << "\nfile encrypt/decrypt (input.txt -> output.txt -> input2.txt)\n";

        std::vector<uint8_t> key(16, 0x2B);
        RijndaelAlgorithm algo(key, 16);
        std::vector<uint8_t> iv = makeIV(16);

        CipherContext2 ctx(&algo, EncryptionMode2::CBC, PaddingMode2::PKCS7, iv);

        auto fenc = ctx.encrypt("input.txt", "output.txt", 2);
        fenc.get();
        std::cout << "Encrypted input.txt -> output.txt\n";

        auto fdec = ctx.decrypt("output.txt", "input2.txt", 2);
        fdec.get();
        std::cout << "Decrypted output.txt -> input2.txt\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
    return 0;
}