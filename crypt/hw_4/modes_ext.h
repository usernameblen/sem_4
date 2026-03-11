#pragma once
#include "..\hw_3\interfaces.h"
#include <algorithm>
#include <stdexcept>
#include <thread>
#include <vector>
#include <cstdint>


class CFBMode : public IEncryptionMode 
{
public:
    void encrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int /*numThreads*/) const override;

    void decrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int /*numThreads*/) const override;
};



class OFBMode : public IEncryptionMode 
{
public:
    void encrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int /*numThreads*/) const override {
        process(algo, input, output, iv);
    }

    void decrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int /*numThreads*/) const override {
        process(algo, input, output, iv);
    }

private:
    void process(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv) const;
};



class CTRMode : public IEncryptionMode 
{
public:
    void encrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int numThreads) const override {
        process(algo, input, output, iv, numThreads);
    }

    void decrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int numThreads) const override {
        process(algo, input, output, iv, numThreads);
    }

private:
    void process(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int numThreads) const;
};




//похож на CTR Mode
// Идея: счётчик для блока i вычисляется как Initial + i * Delta,
// где Initial и Delta — случайные числа из IV. Это усложняет
// предсказание последующих счётчиков атакующим.
// Поддерживает многопоточность, так как блоки независимы.

class RandomDeltaMode : public IEncryptionMode 
{
public:

    void encrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int numThreads) const override {
        process(algo, input, output, iv, numThreads);
    }

    void decrypt(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int numThreads) const override {
        process(algo, input, output, iv, numThreads);
    }

private:
    void process(const IEncryptionAlgorithm& algo,
        const std::vector<uint8_t>& input,
        std::vector<uint8_t>& output,
        const std::vector<uint8_t>& iv,
        int numThreads) const;

};