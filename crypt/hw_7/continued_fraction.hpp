#pragma once

#include "number_theory.hpp"
#include <vector>
#include <utility>
#include <stdexcept>


struct ContinuedFraction {

    //a
    static std::vector<BigInt> expand(const BigInt& a, const BigInt& b)
    {
        if (a <= 0 || b <= 0)
            throw std::invalid_argument("expand: a and b must be positive");

        if (gcd(a, b) != BigInt(1))
            throw std::invalid_argument("expand: gcd(a, b) must be 1");
 
        std::vector<BigInt> coeffs;
        BigInt x = a, y = b;
        while (y != 0) {
            coeffs.push_back(x / y);
            BigInt r = x % y;
            x = y;
            y = r;
        }
        return coeffs;
    }

    //b
    static std::pair<BigInt, BigInt> evaluate(const std::vector<BigInt>& coeffs)
    {
        if (coeffs.empty())
            throw std::invalid_argument("evaluate: coeffs must not be empty");
 
        BigInt num = BigInt(1);
        BigInt den = BigInt(0);
 
        for (int i = static_cast<int>(coeffs.size()) - 1; i >= 0; --i) {
            BigInt new_num = coeffs[i] * num + den;
            BigInt new_den = num;
            num = new_num;
            den = new_den;
        }
        return { num, den };
    }

    //c
    static std::vector<std::pair<BigInt, BigInt>> convergents(const BigInt& a, const BigInt& b)
    {
        auto coeffs = expand(a, b);
 
        std::vector<std::pair<BigInt, BigInt>> result;
 
        BigInt h_prev = BigInt(1), h_curr = coeffs[0];
        BigInt k_prev = BigInt(0), k_curr = BigInt(1);
 
        result.push_back({ h_curr, k_curr });
 
        for (size_t i = 1; i < coeffs.size(); ++i) {
            BigInt h_next = coeffs[i] * h_curr + h_prev;
            BigInt k_next = coeffs[i] * k_curr + k_prev;
            result.push_back({ h_next, k_next });
            h_prev = h_curr; 
            h_curr = h_next;
            k_prev = k_curr; 
            k_curr = k_next;
        } //с каждой итерацией ближе к исходному значению
 
        return result;
    }
};