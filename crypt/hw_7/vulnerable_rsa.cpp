#include "vulnerable_rsa.hpp"
#include <stdexcept>
#include <string>

BigInt VulnerableRSAKeyGenerator::generateCandidate(int bitLength, std::mt19937_64& rng)
{
    if (bitLength < 2)
        throw std::invalid_argument("generateCandidate: bitLength must be >= 2");

    BigInt result = BigInt(0);
    int bits_left = bitLength;

    while (bits_left > 0) { //rng - 64битные значения
        int chunk = std::min(bits_left, 64);
        uint64_t r = rng();
        if (chunk < 64)
            r &= ((uint64_t(1) << chunk) - 1);
        result = (result << chunk) | BigInt(r);
        bits_left -= chunk;
    }

    result |= (BigInt(1) << (bitLength - 1));//гарантия старший бит 1
    result |= BigInt(1);
    return result;
}

BigInt VulnerableRSAKeyGenerator::generatePrime(int bitLength, double minProbability, std::mt19937_64& rng)
{
    MillerRabinPrimalityTest test(rng());
    for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
        BigInt candidate = generateCandidate(bitLength, rng);
        if (test.isPrime(candidate, minProbability))
            return candidate;
    }
    throw std::runtime_error("generatePrime: failed to find prime in " +
        std::to_string(MAX_ATTEMPTS) + " attempts");
}

//|p - q| <= 2 * N^(1/4)
std::pair<PublicKey, PrivateKey> VulnerableRSAKeyGenerator::generateFermatVulnerable(
    int bitLength, double minProbability, std::mt19937_64& rng)
{
    const BigInt e = BigInt(65537);
    MillerRabinPrimalityTest test(rng());

    for (int i = 0; i < MAX_ATTEMPTS; ++i) {
        BigInt p = generatePrime(bitLength, minProbability, rng);

        uint64_t r = rng();
        BigInt delta = BigInt(rng() % 1000) + 1;
        delta <<= 1; //гарантия четности

        //проверка на простоту q
        static const std::vector<BigInt> small_primes = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};

        BigInt q_candidate = p + delta;
        while (true) {
            bool divisible = false;
            bool is_small_prime = false;
            for (const auto& sp : small_primes) {
                if (q_candidate == sp) {
                    is_small_prime = true;
                    break;
                }
                if (q_candidate % sp == 0) {
                    divisible = true;
                    break;
                }
            }

            if (is_small_prime) {
                break;
            }

            if (divisible) {
                q_candidate += 2;
                continue;
            }

            if (test.isPrime(q_candidate, minProbability)) 
                break;
            q_candidate += 2;
        }

        BigInt q = q_candidate;
        BigInt N = p * q;
        BigInt threshold = BigInt(2) * sqrt(sqrt(N));

        BigInt diff = (p > q) ? (p - q) : (q - p);
        if (diff > threshold)
            continue;

        BigInt phi = (p - BigInt(1)) * (q - BigInt(1));

        if (gcd(e, phi) != BigInt(1))
            continue;

        BigInt d = mod_inverse(e, phi); //e * d = 1 (mod phi)

        PublicKey  pub  { N, e };
        PrivateKey priv { N, d, p, q };
        return { pub, priv };
    }

    throw std::runtime_error("generateFermatVulnerable: failed in " +
        std::to_string(MAX_ATTEMPTS) + " attempts");
}

// d < (1/3) * N^(1/4)
std::pair<PublicKey, PrivateKey> VulnerableRSAKeyGenerator::generateWienerVulnerable(
    int bitLength, double minProbability, std::mt19937_64& rng)
{
    const int D_BITS = bitLength / 4 - 1;

    for (int i = 0; i < MAX_ATTEMPTS; ++i) {
        BigInt p = generatePrime(bitLength, minProbability, rng);
        BigInt q = generatePrime(bitLength, minProbability, rng);

        if (p == q)
            continue;

        BigInt N   = p * q;
        BigInt phi = (p - BigInt(1)) * (q - BigInt(1));

        BigInt threshold = sqrt(sqrt(N));

        BigInt d_candidate = generateCandidate(D_BITS, rng);
        if (d_candidate >= threshold / BigInt(3))
            continue;

        if (gcd(d_candidate, phi) != BigInt(1))
            continue;

        BigInt e_candidate = mod_inverse(d_candidate, phi);

        if (e_candidate <= BigInt(1) || e_candidate >= phi)
            continue;

        if (gcd(e_candidate, phi) != BigInt(1))
            continue;

        PublicKey  pub  { N, e_candidate };
        PrivateKey priv { N, d_candidate, p, q };
        return { pub, priv };
    }

    throw std::runtime_error("generateWienerVulnerable: failed in " +
        std::to_string(MAX_ATTEMPTS) + " attempts");
}

std::pair<PublicKey, PrivateKey> VulnerableRSAKeyGenerator::generate(
    int bitLength, Vulnerability vuln, double minProbability)
{
    if (bitLength < 16)
        throw std::invalid_argument("generate: bitLength must be >= 16");

    if (minProbability < 0.5 || minProbability >= 1.0)
        throw std::invalid_argument("generate: minProbability must be in [0.5, 1.0)");

    std::random_device rd;
    std::mt19937_64 rng(rd());

    if (vuln == Vulnerability::Fermat)
        return generateFermatVulnerable(bitLength, minProbability, rng);
    else
        return generateWienerVulnerable(bitLength, minProbability, rng);
}
