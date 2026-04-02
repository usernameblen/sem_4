#pragma once
 
#include "number_theory.hpp"
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>

 
struct SternBrocot {
    static std::string path(const BigInt& a, const BigInt& b)
    {
        if (a <= 0 || b <= 0)
            throw std::invalid_argument("SternBrocot::path: a and b must be positive");
        if (gcd(a, b) != BigInt(1))
            throw std::invalid_argument("SternBrocot::path: gcd(a, b) must be 1");
 
        std::string result;
 
        BigInt ln = BigInt(0), ld = BigInt(1);
        BigInt rn = BigInt(1), rd = BigInt(0);
 
        while (true) {
            BigInt mn = ln + rn;
            BigInt md = ld + rd;
 
            if (a * md == b * mn)
                break;
 
            if (a * md < b * mn) {
                result += 'L';
                rn = mn; rd = md;
            } else {
                result += 'R';
                ln = mn; ld = md;
            }
        }
 
        return result;
    }


    static std::pair<BigInt, BigInt> fromPath(const std::string& p)
    {
        BigInt ln = BigInt(0), ld = BigInt(1);
        BigInt rn = BigInt(1), rd = BigInt(0);
 
        for (char c : p) {
            BigInt mn = ln + rn;
            BigInt md = ld + rd;
            if (c == 'L') {
                rn = mn; rd = md;
            } else if (c == 'R') {
                ln = mn; ld = md;
            } else {
                throw std::invalid_argument("SternBrocot::fromPath: invalid character in path");
            }
        }
 
        BigInt mn = ln + rn;
        BigInt md = ld + rd;
        return { mn, md };
    }


    //f
    static std::vector<std::pair<BigInt, BigInt>> convergents(const std::string& p)
    {
        std::vector<std::pair<BigInt, BigInt>> result;
 
        BigInt ln = BigInt(0), ld = BigInt(1);
        BigInt rn = BigInt(1), rd = BigInt(0);
 
        BigInt mn = ln + rn;
        BigInt md = ld + rd;
        result.push_back({ mn, md });
 
        for (char c : p) {
            if (c == 'L') {
                rn = mn; rd = md;
            } else if (c == 'R') {
                ln = mn; ld = md;
            } else {
                throw std::invalid_argument("SternBrocot::convergents: invalid character");
            }
            mn = ln + rn;
            md = ld + rd;
            result.push_back({ mn, md });
        }
 
        return result;
    }
};