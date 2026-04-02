#pragma once

#include "rsa.hpp"
#include "number_theory.hpp"
#include "miller_rabin_test.hpp"

enum class Vulnerability {
    Fermat,
    Wiener
};

class VulnerableRSAKeyGenerator {
public:
    static std::pair<PublicKey, PrivateKey> generate(
        int bitLength,
        Vulnerability vuln,
        double minProbability = 0.9999
    );

private:
    static const int MAX_ATTEMPTS = 100000;

    static BigInt generateCandidate(int bitLength, std::mt19937_64& rng);
    static BigInt generatePrime(int bitLength, double minProbability, std::mt19937_64& rng);

    static std::pair<PublicKey, PrivateKey> generateFermatVulnerable(
        int bitLength, double minProbability, std::mt19937_64& rng);

    static std::pair<PublicKey, PrivateKey> generateWienerVulnerable(
        int bitLength, double minProbability, std::mt19937_64& rng);
};
