#pragma once
#include"modes_ext.h"

namespace  
{
    void xorBlocks(const std::vector<uint8_t>& a,
        const std::vector<uint8_t>& b,
        std::vector<uint8_t>& dst) 
    {
        dst.resize(a.size());
        for (size_t i = 0; i < a.size(); ++i)
            dst[i] = a[i] ^ b[i];
    }

    // ѕривести IV к нужному размеру блока (обрезать или дополнить нул€ми)
    inline std::vector<uint8_t> normalizeIV(const std::vector<uint8_t>& iv,
        size_t bs) {
        std::vector<uint8_t> result(bs, 0x00);
        size_t len = std::min(iv.size(), bs);
        std::copy(iv.begin(), iv.begin() + len, result.begin());
        return result;
    }

    // от страшего к младшему
    inline void incrementCounter(std::vector<uint8_t>& ctr) {
        for (int i = static_cast<int>(ctr.size()) - 1; i >= 0; --i) {
            if (++ctr[i] != 0) //если не произошло переполнени€ то останавливаемс€
                break;
        }
    }


    inline void addToCounter(std::vector<uint8_t>& ctr, size_t n) {
        for (size_t i = 0; i < n; ++i)
            incrementCounter(ctr);
    }


    inline void addVectors(std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
        uint16_t carry = 0;
        for (int i = static_cast<int>(a.size()) - 1; i >= 0; --i) 
        {
            uint16_t sum = a[i] + b[i] + carry;
            a[i] = static_cast<uint8_t>(sum & 0xFF);
            carry = sum >> 8;
        }
    }

} 



void CFBMode::encrypt(const IEncryptionAlgorithm& algo,
    const std::vector<uint8_t>& input,
    std::vector<uint8_t>& output,
    const std::vector<uint8_t>& iv,
    int /*numThreads*/) const 
{
    const size_t bs = algo.blockSize();
    if (input.size() % bs != 0)
        throw std::invalid_argument("CFB: input size not multiple of block size");

    output.resize(input.size());
    std::vector<uint8_t> feedback = normalizeIV(iv, bs);
    std::vector<uint8_t> encryptedFeedback(bs);

    for (size_t b = 0; b < (input.size() / bs); ++b) 
    {
        algo.encryptBlock(feedback, encryptedFeedback);
        for (size_t i = 0; i < bs; ++i)
            output[b * bs + i] = input[b * bs + i] ^ encryptedFeedback[i];

        std::copy(output.begin() + b * bs,
            output.begin() + b * bs + bs,
            feedback.begin());
    }
}

void CFBMode::decrypt(const IEncryptionAlgorithm& algo,
    const std::vector<uint8_t>& input,
    std::vector<uint8_t>& output,
    const std::vector<uint8_t>& iv,
    int /*numThreads*/) const 
{
    const size_t bs = algo.blockSize();
    if (input.size() % bs != 0)
        throw std::invalid_argument("CFB: input size not multiple of block size");

    output.resize(input.size());
    std::vector<uint8_t> feedback = normalizeIV(iv, bs);
    std::vector<uint8_t> encryptedFeedback(bs);

    for (size_t b = 0; b < (input.size() / bs); ++b)
    {
        algo.encryptBlock(feedback, encryptedFeedback);
        for (size_t i = 0; i < bs; ++i)
            output[b * bs + i] = input[b * bs + i] ^ encryptedFeedback[i];

        std::copy(input.begin() + b * bs,
            input.begin() + b * bs + bs,
            feedback.begin());
    }
}



void OFBMode::process(const IEncryptionAlgorithm& algo,
    const std::vector<uint8_t>& input,
    std::vector<uint8_t>& output,
    const std::vector<uint8_t>& iv) const 
{
    const size_t bs = algo.blockSize();
    if (input.size() % bs != 0)
        throw std::invalid_argument("OFB: input size not multiple of block size");

    output.resize(input.size());
    std::vector<uint8_t> O = normalizeIV(iv, bs);
    std::vector<uint8_t> nextO(bs);

    for (size_t b = 0; b < (input.size() / bs); ++b)
    {
        algo.encryptBlock(O, nextO);
        for (size_t i = 0; i < bs; ++i)
            output[b * bs + i] = input[b * bs + i] ^ nextO[i];
        O = nextO;
    }
}



void CTRMode::process(const IEncryptionAlgorithm& algo,
    const std::vector<uint8_t>& input,
    std::vector<uint8_t>& output,
    const std::vector<uint8_t>& iv,
    int numThreads) const 
{
    const size_t bs = algo.blockSize();
    if (input.size() % bs != 0)
        throw std::invalid_argument("CTR: input size not multiple of block size");

    const size_t numBlocks = input.size() / bs;
    output.resize(input.size());
    numThreads = std::max(1, numThreads);

    std::vector<uint8_t> baseCounter = normalizeIV(iv, bs);


    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    size_t blocksPerThread = numBlocks / numThreads;
    size_t extra = numBlocks % numThreads;
    size_t start = 0;

    for (int t = 0; t < numThreads; ++t) 
    {
        size_t count = blocksPerThread + (t < static_cast<int>(extra) ? 1 : 0);
        if (count == 0) 
            break;

        std::vector<uint8_t> ctr = baseCounter;
        addToCounter(ctr, start);

        threads.emplace_back([&algo, &input, &output, bs, start, count, ctr]() mutable 
        {
            std::vector<uint8_t> encryptedCtr(bs);
            for (size_t b = start; b < start + count; ++b) {
                algo.encryptBlock(ctr, encryptedCtr);
                for (size_t i = 0; i < bs; ++i)
                    output[b * bs + i] = input[b * bs + i] ^ encryptedCtr[i];
                incrementCounter(ctr);
            }
         });
        start += count;
    }
    for (auto& th : threads) th.join();
}





void RandomDeltaMode::process(const IEncryptionAlgorithm& algo,
    const std::vector<uint8_t>& input,
    std::vector<uint8_t>& output,
    const std::vector<uint8_t>& iv,
    int numThreads) const
{
    const size_t bs = algo.blockSize();
    if (input.size() % bs != 0)
        throw std::invalid_argument("RandomDelta: input not multiple of block size");

    if (iv.size() < 2 * bs)
        throw std::invalid_argument("RandomDelta: IV must be at least 2*blockSize");

    // »звлекаем начальное значение (initial) и дельту (delta) из первых двух половин IV
    std::vector<uint8_t> initial(iv.begin(), iv.begin() + bs);
    std::vector<uint8_t> delta(iv.begin() + bs, iv.begin() + 2 * bs);

    const size_t numBlocks = input.size() / bs;
    output.resize(input.size());
    numThreads = std::max(1, numThreads);

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    size_t blocksPerThread = numBlocks / numThreads;
    size_t extra = numBlocks % numThreads;
    size_t start = 0;

    for (int t = 0; t < numThreads; ++t) {
        size_t count = blocksPerThread + (t < static_cast<int>(extra) ? 1 : 0);
        if (count == 0) 
            break;

        // вычисл€ем счЄтчик дл€ первого блока в этом потоке: initial + start * delta
        std::vector<uint8_t> ctr = initial;
        for (size_t k = 0; k < start; ++k) 
            addVectors(ctr, delta);


        threads.emplace_back([&algo, &input, &output, bs, start, count, ctr, delta]() mutable 
        {
            std::vector<uint8_t> encryptedCtr(bs);
            std::vector<uint8_t> currentCtr = ctr;
            for (size_t b = start; b < start + count; ++b) 
            {
                algo.encryptBlock(currentCtr, encryptedCtr);
                for (size_t i = 0; i < bs; ++i)
                    output[b * bs + i] = input[b * bs + i] ^ encryptedCtr[i];

                addVectors(currentCtr, delta);
            }
        });
        start += count;
    }
    for (auto& th : threads) th.join();
}