#pragma once
#include "interfaces.h"

#include <stdexcept>
#include <thread>
#include <vector>

void xorBlocks(std::vector<uint8_t> const& a,
    std::vector<uint8_t> const& b,
    std::vector<uint8_t>& dst)
{
    dst.resize(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        dst[i] = a[i] ^ b[i];
}

class ECBMode : public IEncryptionMode 
{
public:
    void encrypt(IEncryptionAlgorithm const& algo,
        std::vector<uint8_t>const& input,
        std::vector<uint8_t>& output,
        std::vector<uint8_t> const& /*iv*/,
        int numThreads) const override
    {
        processBlocks(algo, input, output, numThreads, true);
    }

    void decrypt(IEncryptionAlgorithm const& algo,
        std::vector<uint8_t> const& input,
        std::vector<uint8_t>& output,
        std::vector<uint8_t> const& /*iv*/,
        int numThreads) const override 
    {
        processBlocks(algo, input, output, numThreads, false);
    }

private:
    void processBlocks(IEncryptionAlgorithm const& algo,
        std::vector<uint8_t> const& input,
        std::vector<uint8_t>& output,
        int numThreads,
        bool enc) const
    {
        size_t const bs = algo.blockSize();
        if (input.size() % bs != 0)
            throw std::invalid_argument("ECB: input size not multiple of block size");

        size_t const numBlocks = input.size() / bs;
        output.resize(input.size());

        numThreads = std::max(1, numThreads);
        std::vector<std::thread> threads;
        threads.reserve(numThreads);

        size_t blocksPerThread = numBlocks / numThreads;
        size_t extra = numBlocks % numThreads;
        size_t start = 0;

        for (int t = 0; t < numThreads; ++t)
        {
            size_t count = blocksPerThread + (t < (int)extra ? 1 : 0);
            if (count == 0) break;

            threads.emplace_back([&, start, count]() {    //std::thread t(функция, аргументы...);
                std::vector<uint8_t> inBlock(bs), outBlock(bs);

                for (size_t b = start; b < start + count; ++b)
                {
                    std::copy(input.begin() + b * bs,
                        input.begin() + b * bs + bs,
                        inBlock.begin());
                    if (enc)
                        algo.encryptBlock(inBlock, outBlock);

                    else
                        algo.decryptBlock(inBlock, outBlock);

                    std::copy(outBlock.begin(), outBlock.end(),
                        output.begin() + b * bs);
                }
                });

            start += count;
        }
        for (auto& th : threads) th.join();
    }
};


class CBCMode : public IEncryptionMode {
public:
    void encrypt(IEncryptionAlgorithm const& algo,
        std::vector<uint8_t> const& input,
        std::vector<uint8_t>& output,
        std::vector<uint8_t> const& iv,
        int /*numThreads*/) const override
    {
        size_t const bs = algo.blockSize();
        if (input.size() % bs != 0)
            throw std::invalid_argument("CBC: input size not multiple of block size");

        output.resize(input.size());
        std::vector<uint8_t> prev = normalizeIV(iv, bs);
        std::vector<uint8_t> xored(bs), enc(bs);

        for (size_t b = 0; b < (input.size() / bs); ++b) {
            std::vector<uint8_t> block(input.begin() + b * bs,
                input.begin() + b * bs + bs);
            xorBlocks(block, prev, xored);
            algo.encryptBlock(xored, enc);
            std::copy(enc.begin(), enc.end(), output.begin() + b * bs);
            prev = enc;
        }
    }

    void decrypt(IEncryptionAlgorithm const& algo,
        std::vector<uint8_t> const& input,
        std::vector<uint8_t>& output,
        std::vector<uint8_t> const& iv,
        int numThreads) const override
    {
        size_t const bs = algo.blockSize();
        if (input.size() % bs != 0)
            throw std::invalid_argument("CBC: input size not multiple of block size");

        size_t const numBlocks = input.size() / bs;
        output.resize(input.size());

        numThreads = std::max(1, numThreads);
        std::vector<std::thread> threads;
        size_t blocksPerThread = numBlocks / numThreads;
        size_t extra = numBlocks % numThreads;
        size_t start = 0;
        std::vector<uint8_t> normIV = normalizeIV(iv, bs);

        for (int t = 0; t < numThreads; ++t)
        {
            size_t count = blocksPerThread + (t < (int)extra ? 1 : 0);
            if (count == 0)
                break;

            threads.emplace_back([&, start, count]() {
                std::vector<uint8_t> inBlock(bs), decBlock(bs), xored(bs);
                for (size_t b = start; b < start + count; ++b) {
                    std::copy(input.begin() + b * bs,
                        input.begin() + b * bs + bs,
                        inBlock.begin());

                    algo.decryptBlock(inBlock, decBlock);
                    std::vector<uint8_t> const& prevC =
                        (b == 0) ? normIV
                        : std::vector<uint8_t>(input.begin() + (b - 1) * bs,
                            input.begin() + b * bs);
                    xorBlocks(decBlock, prevC, xored);
                    std::copy(xored.begin(), xored.end(), output.begin() + b * bs);
                }
                });
            start += count;
        }
        for (auto& th : threads) th.join();
    }

private:
    std::vector<uint8_t> normalizeIV(std::vector<uint8_t> const& iv,
        size_t bs) const
    {
        std::vector<uint8_t> result(bs, 0);
        size_t len = std::min(iv.size(), bs);
        std::copy(iv.begin(), iv.begin() + len, result.begin());
        return result;
    }
};


class PCBCMode : public IEncryptionMode {
public:
    void encrypt(IEncryptionAlgorithm const& algo,
        std::vector<uint8_t> const& input,
        std::vector<uint8_t>& output,
        std::vector<uint8_t> const& iv,
        int /*numThreads*/) const override
    {
        size_t const bs = algo.blockSize();
        if (input.size() % bs != 0)
            throw std::invalid_argument("PCBC: input size not multiple of block size");

        output.resize(input.size());
        std::vector<uint8_t> prevP(bs, 0), prevC = normalizeIV(iv, bs);
        std::vector<uint8_t> xored(bs), enc(bs);

        for (size_t b = 0; b < (input.size() / bs); ++b) {
            std::vector<uint8_t> block(input.begin() + b * bs,
                input.begin() + b * bs + bs);

            for (size_t i = 0; i < bs; ++i)
                xored[i] = block[i] ^ prevP[i] ^ prevC[i];

            algo.encryptBlock(xored, enc);
            std::copy(enc.begin(), enc.end(), output.begin() + b * bs);
            prevP = block;
            prevC = enc;
        }
    }

    void decrypt(IEncryptionAlgorithm const& algo,
        std::vector<uint8_t> const& input,
        std::vector<uint8_t>& output,
        std::vector<uint8_t> const& iv,
        int /*numThreads*/) const override
    {
        size_t const bs = algo.blockSize();
        if (input.size() % bs != 0)
            throw std::invalid_argument("PCBC: input size not multiple of block size");

        output.resize(input.size());
        std::vector<uint8_t> prevP(bs, 0), prevC = normalizeIV(iv, bs);
        std::vector<uint8_t> dec(bs), plain(bs);

        for (size_t b = 0; b < input.size() / bs; ++b) {
            std::vector<uint8_t> block(input.begin() + b * bs,
                input.begin() + b * bs + bs);
            algo.decryptBlock(block, dec);
            for (size_t i = 0; i < bs; ++i)
                plain[i] = dec[i] ^ prevP[i] ^ prevC[i];

            std::copy(plain.begin(), plain.end(), output.begin() + b * bs);
            prevP = plain;
            prevC = block;
        }
    }

private:
    std::vector<uint8_t> normalizeIV(std::vector<uint8_t> const& iv,
        size_t bs) const
    {
        std::vector<uint8_t> result(bs, 0);
        size_t len = std::min(iv.size(), bs);
        std::copy(iv.begin(), iv.begin() + len, result.begin());
        return result;
    }
};