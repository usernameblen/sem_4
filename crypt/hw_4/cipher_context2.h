#pragma once
#include "..\hw_3\interfaces.h"
#include"..\hw_3\modes.h"
#include"..\hw_3\padding.h"

#include "padding_ext.h"
#include "modes_ext.h"

#include <string>
#include <fstream>
#include <future>
#include <memory>
#include <stdexcept>
#include <initializer_list>
#include <any>



enum class EncryptionMode2 
{
    //  из первой работы 
    ECB,
    CBC,
    PCBC,
    //  новые 
    CFB,
    OFB,
    CTR,
    RandomDelta
};

enum class PaddingMode2 
{
    //  из первой работы 
    Zeros,
    ANSI_X923,
    //  новые 
    PKCS7,
    ISO10126
};


class CipherContext2 
{
public:
     CipherContext2(IEncryptionAlgorithm* algorithm,
        EncryptionMode2                 encMode,
        PaddingMode2                    padMode,
        std::vector<uint8_t>            iv = {},
        std::initializer_list<std::any> params = {})
        : algo_(algorithm)
        , iv_(std::move(iv))
        , modeParams_(params)
    {
        if (!algo_) 
            throw std::invalid_argument("algorithm must not be null");

        // Padding
        switch (padMode) {
        case PaddingMode2::Zeros:      
            padding_ = std::make_unique<ZerosPadding>();
            break;

        case PaddingMode2::ANSI_X923:  
            padding_ = std::make_unique<AnsiX923Padding>(); 
            break;

        case PaddingMode2::PKCS7:
            padding_ = std::make_unique<Pkcs7Padding>();
            break;

        case PaddingMode2::ISO10126:
            padding_ = std::make_unique<Iso10126Padding>();
            break;

        default:
            throw std::invalid_argument("Unknown padding mode");
        }

        //Encryption mode
        switch (encMode) {
        case EncryptionMode2::ECB:
            mode_ = std::make_unique<ECBMode>();
            break;

        case EncryptionMode2::CBC:
            mode_ = std::make_unique<CBCMode>();
            break;

        case EncryptionMode2::PCBC:
            mode_ = std::make_unique<PCBCMode>();
            break;

        case EncryptionMode2::CFB:
            mode_ = std::make_unique<CFBMode>();
            break;

        case EncryptionMode2::OFB:
            mode_ = std::make_unique<OFBMode>();
            break;

        case EncryptionMode2::CTR:
            mode_ = std::make_unique<CTRMode>();
            break;

        case EncryptionMode2::RandomDelta:
            mode_ = std::make_unique<RandomDeltaMode>();
            break;

        default: 
            throw std::invalid_argument("Unknown encryption mode");
        }

        mode_->configure(modeParams_);
    }


    void encrypt(const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        int numThreads = 1) const 
    {
        std::vector<uint8_t> buf(input);
        padding_->pad(buf, algo_->blockSize());
        mode_->encrypt(*algo_, buf, output, iv_, numThreads);
    }

    void decrypt(const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        int numThreads = 1) const 
    {
        mode_->decrypt(*algo_, input, output, iv_, numThreads);
        padding_->unpad(output, algo_->blockSize());
    }

 

    std::future<void> encrypt(const std::string& inputPath,
        const std::string& outputPath,
        int numThreads = 1) const 
    {
        return std::async(std::launch::async,
            [this, inputPath, outputPath, numThreads]() {
                std::vector<uint8_t> raw, enc;
                readFile(inputPath, raw);
                encrypt(raw, enc, numThreads);
                writeFile(outputPath, enc);
            });
    }

    std::future<void> decrypt(const std::string& inputPath,
        const std::string& outputPath,
        int numThreads = 1) const 
    {
        return std::async(std::launch::async,
            [this, inputPath, outputPath, numThreads]() {
                std::vector<uint8_t> raw, dec;
                readFile(inputPath, raw);
                decrypt(raw, dec, numThreads);
                writeFile(outputPath, dec);
            });
    }

    const ModeParams& modeParams() const { return modeParams_; }

private:
    static void readFile(const std::string& path, std::vector<uint8_t>& out) 
    {
        std::ifstream f(path, std::ios::binary | std::ios::ate); // переходим в конец для tellg
        if (!f.is_open())
            throw std::runtime_error("Cannot open for reading: " + path);

        std::streamsize size = f.tellg();// возвращает текущую позицию (размер)
        f.seekg(0, std::ios::beg);

        out.resize(static_cast<size_t>(size));
        if (!f.read(reinterpret_cast<char*>(out.data()), size))
            throw std::runtime_error("Read error: " + path);
    }

    static void writeFile(const std::string& path, const std::vector<uint8_t>& data) 
    {
        std::ofstream f(path, std::ios::binary);
        if (!f.is_open())
            throw std::runtime_error("Cannot open for writing: " + path);

        f.write(reinterpret_cast<const char*>(data.data()),
            static_cast<std::streamsize>(data.size()));
        if (!f) 
            throw std::runtime_error("Write error: " + path);
    }

    IEncryptionAlgorithm* algo_;
    std::vector<uint8_t> iv_;
    ModeParams modeParams_;
    std::unique_ptr<IPaddingMode> padding_;
    std::unique_ptr<IEncryptionMode> mode_;
};