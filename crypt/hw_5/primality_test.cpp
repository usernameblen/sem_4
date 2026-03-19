
#include "primality_test.hpp"
#include <cmath>
#include <stdexcept>

namespace nt 
{
int ProbabilisticPrimalityTest::computeIterations(double minProbability) 
{
    if (minProbability < 0.5 || minProbability >= 1.0) {
        throw std::invalid_argument(
            "computeIterations: minProbability should be in range [0.5, 1.0)");
    }

    double k = std::ceil(std::log2(1.0 / (1.0 - minProbability)));
    return std::max(1, static_cast<int>(k));
}

bool ProbabilisticPrimalityTest::isPrime(const BigInt& n, double minProbability) const 
{
    if (minProbability < 0.5 || minProbability >= 1.0) {
        throw std::invalid_argument(
            "isPrime: minProbability should be in range [0.5, 1.0)");
    }


    if (n < 2) return false;
    if (n == 2) return true;
    if (n == 3) return true;
    if (n % 2 == 0) return false;

    
    static const int small_primes[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31};
    for (int p : small_primes) {
        if (n == BigInt(p)) return true;
        if (n % BigInt(p) == 0) return false;
    }

    int k = computeIterations(minProbability);

    for (int i = 0; i < k; ++i) {
        if (!runOneIteration(n)) {
            return false;
        }
    }

    return true;
}

FermatPrimalityTest::FermatPrimalityTest(uint64_t seed)
    : rng_(seed)
{}

BigInt FermatPrimalityTest::randomInRange(const BigInt& lo, const BigInt& hi) const 
{
    BigInt range = hi - lo;

    uint64_t r = rng_();
    BigInt result = lo + (BigInt(r) % (range + BigInt(1)));
    return result;
}

bool FermatPrimalityTest::runOneIteration(const BigInt& n) const 
{

    if (n <= 3) return true;

    BigInt lo = BigInt(2);
    BigInt hi = n - BigInt(1);

    if (hi < lo)
        return true;

    BigInt a = randomInRange(lo, hi);

    BigInt result = pow_mod(a, n - BigInt(1), n);

    return (result == BigInt(1));
}

}
