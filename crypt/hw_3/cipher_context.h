#pragma once
#include "interfaces.h"
#include "padding.h"
#include "modes.h"

#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <future>
#include <initializer_list>
#include <any>

class CipherContext
{
public:
    CipherContext(IEncryptionAlgorithm* algorithm,
        EncryptionMode encMode,
        PaddingMode padMode,
        std::vector<uint8_t> iv = {},
        std::initializer_list<std::any> params = {})
        : algo_(algorithm), iv_(std::move(iv)), modeParams_(params)
    {
        if (!algo_)
            throw std::invalid_argument("algorithm must not be null");

        switch (padMode) {
        case PaddingMode::Zeros:
            padding_ = std::make_unique<ZerosPadding>();
            break;
        case PaddingMode::ANSI_X923:
            padding_ = std::make_unique<AnsiX923Padding>();
            break;
        default:
            throw std::invalid_argument("Unknown padding mode");
        }

        switch (encMode) {
        case EncryptionMode::ECB:
            mode_ = std::make_unique<ECBMode>();
            break;
        case EncryptionMode::CBC:
            mode_ = std::make_unique<CBCMode>();
            break;
        case EncryptionMode::PCBC:
            mode_ = std::make_unique<PCBCMode>();
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
                auto raw = readFile(inputPath);
                std::vector<uint8_t> enc;
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
                auto raw = readFile(inputPath);
                std::vector<uint8_t> dec;
                decrypt(raw, dec, numThreads);
                writeFile(outputPath, dec);
            });
    }

    const ModeParams& modeParams() const { return modeParams_; }

private:
    static std::vector<uint8_t> readFile(const std::string& path) 
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate); // переходим в конец для tellg
        if (!file.is_open())
            throw std::runtime_error("Cannot open file for reading: " + path);

        std::streamsize size = file.tellg(); // возвращает текущую позицию (размер)
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(static_cast<size_t>(size));
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
            throw std::runtime_error("Read error: " + path);

        return buffer;
    }

    static void writeFile(const std::string& path, const std::vector<uint8_t>& data)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Cannot open file for writing: " + path);

        file.write(reinterpret_cast<char const*>(data.data()),
            static_cast<std::streamsize>(data.size()));
    }

    IEncryptionAlgorithm* algo_;
    std::vector<uint8_t> iv_;
    ModeParams modeParams_;
    std::unique_ptr<IPaddingMode> padding_;
    std::unique_ptr<IEncryptionMode> mode_;
};