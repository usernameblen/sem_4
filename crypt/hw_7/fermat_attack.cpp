#include "fermat_attack.hpp"
#include <stdexcept>

//|p - q| <= 2 * N^(1/4)
std::pair<BigInt, BigInt> fermatAttack(const BigInt& N)
{
    if (N < BigInt(4))
        throw std::invalid_argument("fermatAttack: N must be >= 4");

    if (N % 2 == 0)
        throw std::invalid_argument("fermatAttack: N must be odd");

    BigInt a = sqrt(N);
    if (a * a < N)
        ++a;

    const BigInt MAX_ITER = BigInt(1) << 20;

    for (BigInt iter = BigInt(0); iter < MAX_ITER; ++iter, ++a) {
        BigInt a2 = a * a;
        BigInt diff = a2 - N;

        BigInt b = sqrt(diff); //N = a^2 - b^2, a = (p+q)/2, b = (p-q)/2

        if (b * b == diff) {
            BigInt p = a - b;
            BigInt q = a + b;

            return { p, q };
        }
    }

    throw std::runtime_error("fermatAttack: factorization failed");
}
