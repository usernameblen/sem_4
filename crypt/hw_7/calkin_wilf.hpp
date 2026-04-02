#pragma once

#include "number_theory.hpp"
#include <string>
#include <utility>

//a/b
//L = a/(a+b)
//R = (a+b)/b
struct CalkinWilf {

    //d
    static std::string path(const BigInt& a, const BigInt& b)
    {
        if (a <= 0 || b <= 0)
            throw std::invalid_argument("CalkinWilf::path: a and b must be positive");
        if (gcd(a, b) != BigInt(1))
            throw std::invalid_argument("CalkinWilf::path: gcd(a, b) must be 1");
 
        std::string result;
        BigInt x = a, y = b;
 
        while (x != y) {
            if (x < y) {
                result += 'L';
                y = y - x;
            } else {
                result += 'R';
                x = x - y;
            }
        }
 
        std::reverse(result.begin(), result.end());
        return result;
    }


    //e
    static std::pair<BigInt, BigInt> fromPath(const std::string& p)
    {
        BigInt a = BigInt(1), b = BigInt(1);
 
        for (char c : p) {
            if (c == 'L') {
                b = a + b;
            } else if (c == 'R') {
                a = a + b;
            } else {
                throw std::invalid_argument("CalkinWilf::fromPath: invalid character in path");
            }
        }
 
        return { a, b };
    }
};