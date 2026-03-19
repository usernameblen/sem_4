#pragma once

#include "number_theory.hpp"

#include <cmath>
#include <random>
#include <stdexcept>
#include <string>

namespace nt 
{
class IPrimalityTest {
public:
    virtual ~IPrimalityTest() = default;

    virtual bool isPrime(const BigInt& n, double minProbability) const = 0;
};

class ProbabilisticPrimalityTest : public IPrimalityTest {
public:
    ~ProbabilisticPrimalityTest() override = default;

    bool isPrime(const BigInt& n, double minProbability) const final;

    static int computeIterations(double minProbability);

protected:
    virtual bool runOneIteration(const BigInt& n) const = 0;
};

class FermatPrimalityTest : public ProbabilisticPrimalityTest {
public:
    explicit FermatPrimalityTest(uint64_t seed = std::random_device{}());

    ~FermatPrimalityTest() override = default;

protected:
    bool runOneIteration(const BigInt& n) const override;

private:
    mutable std::mt19937_64 rng_;

    BigInt randomInRange(const BigInt& lo, const BigInt& hi) const;
};

}