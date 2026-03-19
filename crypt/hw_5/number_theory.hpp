#pragma once

#include <stdexcept>
#include <tuple>
#include <cmath>
#include <string>
#include <iostream>


  #include <boost/multiprecision/cpp_int.hpp>
  namespace nt {
    using BigInt = boost::multiprecision::cpp_int;
  }

namespace nt {

// a. Символ Лежандра
int legendre(const BigInt& a, const BigInt& p);

// b. Символ Якоби
int jacobi(BigInt a, BigInt n);

// c. НОД (итеративный алгоритм Евклида)
BigInt gcd(BigInt a, BigInt b);

// d. Расширенный алгоритм Евклида
std::tuple<BigInt, BigInt, BigInt> extended_gcd(BigInt a, BigInt b);

// e. Быстрое возведение в степень по модулю
BigInt pow_mod(BigInt base, BigInt exp, const BigInt& mod);

// f. Мультипликативное обратное в кольце Z_n
BigInt mod_inverse(const BigInt& a, const BigInt& n);

// g. Функция Эйлера φ(n) — три реализации
BigInt euler_totient_naive(const BigInt& n);


BigInt euler_totient_factorization(BigInt n);


long long euler_totient_dft(long long n);

}