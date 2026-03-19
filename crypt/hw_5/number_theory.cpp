
#define _USE_MATH_DEFINES

#include "number_theory.hpp"
#include <stdexcept>
#include <cmath>

namespace nt {
static inline bool is_even(const BigInt& n) {
    return (n % 2) == 0;
}

// a. Символ Лежандра 
// p - простое нечет
// Если a кратно p — символ равен 0
// 1 - сущесвтует такой x^2 = a (mod p)
// -1 - не существует такого x
int legendre(const BigInt& a, const BigInt& p) //??
{
    if (p < 2 || is_even(p)) 
        throw std::invalid_argument("Legendre symbol requires an odd prime p >= 3.");
    
    BigInt a_mod = ((a % p) + p) % p;

    if (a_mod == 0) return 0;

    BigInt exp = (p - 1) / 2;
    BigInt result = pow_mod(a_mod, exp, p); //a^(exp) (mod p)

    if (result == 1)     return  1;
    if (result == p - 1) return -1;

    //не должно достигаться для простых p
    return 0;
}

// b. Символ Якоби (итеративная реализация)
//n - нечет положительное (мб составное)
int jacobi(BigInt a, BigInt n) 
{
    if (n <= 0 || is_even(n)) 
        throw std::invalid_argument("Jacobi symbol requires an odd positive integer n.");

    int result = 1;

    if (a < 0) 
    {
        a = -a;
        BigInt nm1_2 = (n - 1) / 2;
        if (nm1_2 % 2 != 0) 
            result = -result;
    }

    while (a != 0) 
    {
        while (is_even(a)) 
        {
            a /= 2;
            BigInt n_mod8 = n % 8;
            if (n_mod8 == 3 || n_mod8 == 5) result = -result;
        }

        std::swap(a, n);
        BigInt am1_2 = (a - 1) / 2;
        BigInt nm1_2 = (n - 1) / 2;
        if ((am1_2 % 2 != 0) && (nm1_2 % 2 != 0)) result = -result;

        a = a % n;
    }

    if (n == 1) 
        return result;
    return 0;
}

// c. НОД (итеративный алгоритм Евклида)
BigInt gcd(BigInt a, BigInt b) 
{
    if (a < 0) a = -a;
    if (b < 0) b = -b;

    while (b != 0) 
    {
        a %= b;
        std::swap(a, b);
    }
    return a;
}

// d. Расширенный алгоритм Евклида
std::tuple<BigInt, BigInt, BigInt> extended_gcd(BigInt a, BigInt b) 
{
    int sign_a = (a >= 0) ? 1 : -1;
    int sign_b = (b >= 0) ? 1 : -1;
    if (a < 0) a = -a;
    if (b < 0) b = -b;

    BigInt old_r = a, r = b;
    BigInt old_s = 1, s = 0;
    BigInt old_t = 0, t = 1;

    while (r != 0) 
    {
        BigInt q = old_r / r;

        BigInt tmp = r;
        r = old_r - q * r;
        old_r = tmp;

        tmp = s;
        s = old_s - q * s;
        old_s = tmp;

        tmp = t;
        t = old_t - q * t;
        old_t = tmp;
    }

    // Корректируем знаки коэффициентов Безу
    BigInt x = old_s * sign_a;
    BigInt y = old_t * sign_b;

    return {old_r, x, y};
}

// e. Быстрое возведение в степень по модулю (right-to-left)
BigInt pow_mod(BigInt base, BigInt exp, const BigInt& mod) 
{
    if (mod <= 0) 
        throw std::invalid_argument("pow_mod: modulus must be > 0");
    
    if (exp < 0) 
        throw std::invalid_argument("pow_mod: exponent must be >= 0");
    
    if (mod == 1) 
        return 0;

    BigInt result = 1;
    base %= mod;

    if (base < 0) base += mod;

    while (exp > 0) //побитовое возведение в степень 
    {
        if (exp % 2 == 1) 
            result = (result * base) % mod;
        
        exp >>= 1;
        base = (base * base) % mod;
    }
    return result;
}

// f. Мультипликативное обратное в кольце Z_n
BigInt mod_inverse(const BigInt& a, const BigInt& n) 
{
    if (n <= 1) 
        throw std::invalid_argument("mod_inverse: modulus n must be > 1");

    auto [g, x, y] = extended_gcd(a, n); //ax + ny = g

    if (g != 1) {
        throw std::invalid_argument(
            "mod_inverse: inverse doesn't exist since gcd(a, n) = " +
            g.str() + " != 1"
        );
    }

    BigInt result = x % n;
    if (result < 0) result += n;
    return result;
}

// g1. Функция Эйлера по определению (перебор)
BigInt euler_totient_naive(const BigInt& n) 
{
    if (n <= 0) 
        throw std::invalid_argument("euler_totient_naive: n must be > 0");
    
    if (n == 1) return 1;

    BigInt count = 0;
    for (BigInt k = 1; k < n; ++k) 
    {
        if (gcd(k, n) == 1) 
            ++count;
    }
    return count;
}


// g2. Функция Эйлера через факторизацию
BigInt euler_totient_factorization(BigInt n) 
{
    if (n <= 0) 
        throw std::invalid_argument("euler_totient_factorization: n must be > 0");

    if (n == 1) return 1;

    BigInt result = n;    // начинаем с n и умножаем на (1 − 1/p) для каждого простого p
    BigInt temp = n;      // копия для факторизации

    // перебираем простые делители от 2 до sqrt(temp)
    for (BigInt p = 2; p * p <= temp; p += 1) {
        if (temp % p == 0) 
        {
            result /= p;
            result *= (p - 1);

            // делим temp на все вхождения p
            while (temp % p == 0) 
            {
                temp /= p;
            }
        }
    }

    // если остался множитель > 1 — он простой
    if (temp > 1) {
        result /= temp;
        result *= (temp - 1);
    }

    return result;
}

// g3. Функция Эйлера через ДПФ
long long euler_totient_dft(long long n) 
{
    if (n <= 0) 
        throw std::invalid_argument("euler_totient_dft: n must be > 0");
    
    if (n == 1) return 1;

    // φ(n) = ∑_{k=1}^{n} gcd(k, n) · cos(2πk / n)
    const double TWO_PI = 2.0 * M_PI;
    double sum = 0.0;

    for (long long k = 1; k <= n; ++k) 
    {
        long long g = static_cast<long long>(nt::gcd(BigInt(k), BigInt(n)));
        double angle = TWO_PI * static_cast<double>(k) / static_cast<double>(n);
        sum += static_cast<double>(g) * std::cos(angle);
    }

    return static_cast<long long>(std::round(sum));
}
}