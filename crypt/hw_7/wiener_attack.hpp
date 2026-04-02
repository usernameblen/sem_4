#pragma once

#include "number_theory.hpp"
#include "continued_fraction.hpp"
#include "rsa.hpp"
#include <utility>

std::pair<BigInt, BigInt> wienerAttack(const PublicKey& pub)
{
    if (pub.N < BigInt(4))
        throw std::invalid_argument("wienerAttack: N too small");
    if (pub.e <= BigInt(1) || pub.e >= pub.N)
        throw std::invalid_argument("wienerAttack: invalid e");
 
    auto convergents = ContinuedFraction::convergents(pub.e, pub.N);
 
    for (auto& [k, d] : convergents) {
        if (k == BigInt(0))
            continue;
 
        BigInt edm1 = pub.e * d - BigInt(1);
 
        if (edm1 % k != BigInt(0))
            continue;
 
        BigInt phi = edm1 / k;
 
        BigInt s = pub.N - phi + BigInt(1);
        BigInt D = s * s - BigInt(4) * pub.N;
 
        if (D < BigInt(0))
            continue;
 
        BigInt sqrtD = sqrt(D);
 
        if (sqrtD * sqrtD != D)
            continue;
 
        if ((s - sqrtD) % BigInt(2) != BigInt(0))
            continue;
 
        BigInt p = (s - sqrtD) / BigInt(2);
        BigInt q = (s + sqrtD) / BigInt(2);
 
        if (p > BigInt(1) && q > BigInt(1) && p * q == pub.N)
            return { p, q };
    }
 
    throw std::runtime_error("wienerAttack: attack failed, key may not be vulnerable");
}