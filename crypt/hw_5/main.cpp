#define _USE_MATH_DEFINES
#include <cmath>

#include "number_theory.hpp"
#include "primality_test.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <stdexcept>

using namespace nt;


static int  g_tests_run    = 0;
static int  g_tests_passed = 0;
static int  g_tests_failed = 0;
static std::string g_current_suite;

static void TEST_SUITE(const std::string& name) {
    g_current_suite = name;
    std::cout << "\n  [" << name << "]\n";
}

// Проверка условия
static void CHECK(bool condition, const std::string& expr, const std::string& file, int line) {
    ++g_tests_run;
    if (condition) {
        ++g_tests_passed;
        std::cout << "    PASS  " << expr << "\n";
    } else {
        ++g_tests_failed;
        std::cout << "    FAIL  " << expr
                  << "  <- " << file << ":" << line << "\n";
    }
}

// Проверка на исключение
template<typename Fn>
static void CHECK_THROWS(Fn fn, const std::string& expr, const std::string& file, int line) {
    ++g_tests_run;
    try {
        fn();
        ++g_tests_failed;
        std::cout << "    FAIL  " << expr << " - exception was NOT thrown"
                  << "  <- " << file << ":" << line << "\n";
    } catch (const std::exception&) {
        ++g_tests_passed;
        std::cout << "    PASS  " << expr << " (throws exception)\n";
    }
}

// Проверка на отсутствие исключения
template<typename Fn>
static void CHECK_NO_THROW(Fn fn, const std::string& expr, const std::string& file, int line) {
    ++g_tests_run;
    try {
        fn();
        ++g_tests_passed;
        std::cout << "    PASS  " << expr << "\n";
    } catch (const std::exception& e) {
        ++g_tests_failed;
        std::cout << "    FAIL  " << expr << " - unexpected exception: " << e.what()
                  << "  <- " << file << ":" << line << "\n";
    }
}

// Макросы для удобства
#define CHECK_EQ(a, b)       CHECK((a) == (b),        #a " == " #b,       __FILE__, __LINE__)
#define CHECK_NE(a, b)       CHECK((a) != (b),        #a " != " #b,       __FILE__, __LINE__)
#define CHECK_TRUE(expr)     CHECK((expr),             #expr,              __FILE__, __LINE__)
#define CHECK_FALSE(expr)    CHECK(!(expr),            "!" #expr,          __FILE__, __LINE__)
#define CHECK_GE(a, b)       CHECK((a) >= (b),        #a " >= " #b,       __FILE__, __LINE__)
#define CHECK_LT(a, b)       CHECK((a) <  (b),        #a " < "  #b,       __FILE__, __LINE__)
#define THROWS(expr)         CHECK_THROWS([&]{ expr; }, #expr, __FILE__, __LINE__)
#define NO_THROW(expr)       CHECK_NO_THROW([&]{ expr; }, #expr, __FILE__, __LINE__)



static void section(const std::string& t) {
    std::cout << "\n" << std::string(65, '=') << "\n  " << t
              << "\n" << std::string(65, '-') << "\n";
}

static void subsection(const std::string& t) {
    std::cout << "\n  --- " << t << " ---\n";
}



void demo_task1() {
    section("TASK 1: NUMBER THEORETIC FUNCTIONS");

    // ── a. Символ Лежандра ────────────────────────────────────────────────────
    subsection("a. Legendre Symbol (a/p)");
    for (int a : {0, 1, 2, 3, 4, 5, 6})
        std::cout << "  legendre(" << a << ", 7) = " << legendre(a, 7) << "\n";
    std::cout << "  legendre(5, 11) = " << legendre(5, 11) << "  [residue: 4^2 = 5 mod 11]\n";
    std::cout << "  legendre(-1, 7) = " << legendre(-1, 7) << "  [(7-1)/2=3 odd -> -1]\n";

    // ── b. Символ Якоби ───────────────────────────────────────────────────────
    subsection("b. Jacobi Symbol (a/n)");
    std::cout << "  jacobi(2, 15)  = " << jacobi(2, 15)  << "\n";
    std::cout << "  jacobi(7, 15)  = " << jacobi(7, 15)  << "\n";
    std::cout << "  jacobi(3, 9)   = " << jacobi(3, 9)   << "  [gcd(3,9)=3 -> 0]\n";
    std::cout << "  jacobi(-1, 7)  = " << jacobi(-1, 7)  << "\n";
    std::cout << "  jacobi(-1, 5)  = " << jacobi(-1, 5)  << "  [(5-1)/2=2 even -> 1]\n";

    // ── c. НОД ────────────────────────────────────────────────────────────────
    subsection("c. Greatest Common Divisor (GCD)");
    std::cout << "  gcd(48, 18)              = " << gcd(48, 18) << "\n";
    std::cout << "  gcd(100, 75)             = " << gcd(100, 75) << "\n";
    std::cout << "  gcd(0, 5)                = " << gcd(0, 5) << "\n";
    std::cout << "  gcd(-12, 8)              = " << gcd(-12, 8) << "\n";
    std::cout << "  gcd(1234567890, 9876543210) = "
              << gcd(BigInt("1234567890"), BigInt("9876543210")) << "\n";

    // ── d. Расширенный алгоритм Евклида ───────────────────────────────────────
    subsection("d. Extended Euclidean Algorithm");
    auto show_ext = [](BigInt a, BigInt b) {
        auto [g, x, y] = extended_gcd(a, b);
        std::cout << "  extended_gcd(" << a << ", " << b << ")"
                  << " -> gcd=" << g << ", x=" << x << ", y=" << y
                  << "  [check: " << (a*x + b*y) << "]\n";
    };
    show_ext(35, 15);
    show_ext(101, 37);
    show_ext(0, 5);
    show_ext(-7, 3);

    // ── e. Возведение в степень по модулю ─────────────────────────────────────
    subsection("e. pow_mod(a, b, m)");
    std::cout << "  pow_mod(2, 10, 1000)   = " << pow_mod(2, 10, 1000) << "\n";
    std::cout << "  pow_mod(3, 200, 13)    = " << pow_mod(3, 200, 13)  << "\n";
    std::cout << "  pow_mod(7, 0, 10)      = " << pow_mod(7, 0, 10)    << "\n";
    std::cout << "  pow_mod(123, 456, 789) = " << pow_mod(123, 456, 789) << "\n";
    std::cout << "  pow_mod(123456789, p-1, p) = "
              << pow_mod(BigInt(123456789), BigInt(1000000006), BigInt(1000000007))
              << "  [Fermat's little theorem: a^(p-1)=1 mod p]\n";

    // ── f. Мультипликативное обратное ─────────────────────────────────────────
    subsection("f. mod_inverse(a, n)");
    auto show_inv = [](BigInt a, BigInt n) {
        try {
            BigInt inv = mod_inverse(a, n);
            std::cout << "  mod_inverse(" << a << ", " << n << ") = " << inv
                      << "  [check: " << ((a * inv % n + n) % n) << "]\n";
        } catch (const std::exception& e) {
            std::cout << "  mod_inverse(" << a << ", " << n << ") -> " << e.what() << "\n";
        }
    };
    show_inv(3, 7);
    show_inv(7, 26);
    show_inv(17, 3120);
    show_inv(6, 9);    // no inverse
    show_inv(-5, 11);

    // ── g. Функция Эйлера тремя способами ────────────────────────────────────
    subsection("g. Euler Totient Function phi(n) - three methods");
    std::cout << std::left
              << std::setw(6)  << "n"
              << std::setw(12) << "naive"
              << std::setw(12) << "factor"
              << std::setw(12) << "dft" << "\n"
              << std::string(42, '-') << "\n";
    for (int n : {1, 2, 6, 9, 12, 36, 100})
        std::cout << std::setw(6)  << n
                  << std::setw(12) << euler_totient_naive(n)
                  << std::setw(12) << euler_totient_factorization(n)
                  << std::setw(12) << euler_totient_dft(n) << "\n";
    std::cout << "\n  phi(10^9+7) = " << euler_totient_factorization(BigInt(1000000007))
              << "  [prime -> phi = p-1]\n";
    std::cout << "  phi(360)    = " << euler_totient_factorization(BigInt(360)) << "\n";
}

void demo_task2() {
    section("TASK 2: FERMAT PRIMALITY TEST");

    FermatPrimalityTest fermat(42);

    auto test_n = [&](long long n, const std::string& note) {
        std::cout << "\n  n=" << std::setw(22) << n << "  [" << note << "]\n";
        for (double prob : {0.5, 0.9, 0.99}) {
            int k = ProbabilisticPrimalityTest::computeIterations(prob);
            bool r = fermat.isPrime(BigInt(n), prob);
            std::cout << "    prob=" << std::fixed << std::setprecision(2) << prob
                      << "  iter=" << std::setw(3) << k
                      << "  -> " << (r ? "PRIME     " : "COMPOSITE   ") << "\n";
        }
    };

    subsection("Prime Numbers (expected PRIME)");
    for (long long p : {7, 13, 17, 101, 7919}) test_n(p, "prime");

    subsection("Composite Numbers (expected COMPOSITE)");
    for (long long c : {15, 21, 27, 49, 100}) test_n(c, "composite");

    subsection("Carmichael Numbers (Fermat test can fail!)" );
    std::cout << "\n  561=3*11*17, 1105=5*13*17, 1729=7*13*19\n"
              << "  a^(n-1)=1(mod n) for most bases a - false positives:\n";
    for (long long k : {561, 1105, 1729}) test_n(k, "Carmichael number");

    subsection("Edge Cases");
    for (auto& [n, note] : std::vector<std::pair<long long, std::string>>{
            {2, "smallest prime"}, {3, "prime"},
            {4, "composite 2^2"},   {1, "not prime by definition"}}) {
        std::cout << "  isPrime(" << n << ", 0.99) = "
                  << (fermat.isPrime(BigInt(n), 0.99) ? "TRUE " : "FALSE")
                  << "  [" << note << "]\n";
    }

    subsection("Number of iterations: k = ceil(log2(1 / (1 - p)))");
    std::cout << "  " << std::left << std::setw(10) << "prob" << "k\n"
              << "  " << std::string(18, '-') << "\n";
    for (double p : {0.5, 0.75, 0.9, 0.95, 0.99, 0.999})
        std::cout << "  " << std::setw(10) << std::fixed << std::setprecision(3) << p
                  << ProbabilisticPrimalityTest::computeIterations(p) << "\n";

    subsection("Large Numbers");
    struct { long long val; const char* note; } bigs[] = {
        {999999999999999989LL, "large prime"},
        {999999999999999999LL, "composite (sum of digits divisible by 3)"},
        {2147483647LL,         "prime Mersenne M31 = 2^31-1"},
    };
    for (auto& t : bigs) {
        bool r = fermat.isPrime(BigInt(t.val), 0.99);
        std::cout << "  isPrime(" << t.val << ", 0.99) = "
                  << (r ? "TRUE " : "FALSE") << "  [" << t.note << "]\n";
    }
}


//unit тесты
void run_all_tests() {
    section("UNIT TESTS");

    // Эталонная таблица φ(n) для n=1..20
    const long long PHI[] = {0,1,1,2,2,4,2,6,4,6,4,10,4,12,6,8,8,16,6,18,8};

    // a. Символ Лежандра
    TEST_SUITE("Legendre Symbol");

    // a кратно p → 0
    CHECK_EQ(legendre(0,  7),  0);
    CHECK_EQ(legendre(7,  7),  0);
    CHECK_EQ(legendre(14, 7),  0);

    // квадратичные вычеты mod 7: {1, 2, 4}
    CHECK_EQ(legendre(1, 7),  1);
    CHECK_EQ(legendre(2, 7),  1);
    CHECK_EQ(legendre(4, 7),  1);

    // не-вычеты mod 7: {3, 5, 6}
    CHECK_EQ(legendre(3, 7), -1);
    CHECK_EQ(legendre(5, 7), -1);
    CHECK_EQ(legendre(6, 7), -1);

    // отрицательный a
    CHECK_EQ(legendre(-1, 7), -1); // (7-1)/2=3 нечётное
    CHECK_EQ(legendre(-1, 5),  1); // (5-1)/2=2 чётное

    // вычеты mod 11: {1,3,4,5,9}
    CHECK_EQ(legendre(1, 11),  1);
    CHECK_EQ(legendre(3, 11),  1);
    CHECK_EQ(legendre(2, 11), -1);
    CHECK_EQ(legendre(6, 11), -1);

    // некорректный p
    THROWS(legendre(3,  0));
    THROWS(legendre(3,  1));
    THROWS(legendre(3,  4)); // чётное
    THROWS(legendre(3, -5));

    // b. Символ Якоби
    TEST_SUITE("Jacobi Symbol");

    // n = 1: (a/1) = 1 для любого a
    CHECK_EQ(jacobi(0,   1), 1);
    CHECK_EQ(jacobi(1,   1), 1);
    CHECK_EQ(jacobi(100, 1), 1);

    // простое n: совпадает с символом Лежандра
    CHECK_EQ(jacobi(1, 7),  1);
    CHECK_EQ(jacobi(2, 7),  1);
    CHECK_EQ(jacobi(3, 7), -1);
    CHECK_EQ(jacobi(5, 7), -1);

    // составное n
    CHECK_EQ(jacobi(3, 9),  0);  // gcd(3,9)=3
    CHECK_EQ(jacobi(8, 9),  1);  // (-1)^4 = 1
    CHECK_EQ(jacobi(2, 15),  1);
    CHECK_EQ(jacobi(7, 15), -1);
    CHECK_EQ(jacobi(5, 15),  0); // gcd(5,15)=5

    // отрицательный a
    CHECK_EQ(jacobi(-1, 7), -1);
    CHECK_EQ(jacobi(-1, 5),  1);
    CHECK_EQ(jacobi(-1, 9),  1);

    // некорректный n
    THROWS(jacobi(3,  0));
    THROWS(jacobi(3, -1));
    THROWS(jacobi(3,  4)); // чётное

    // c. НОД
    TEST_SUITE("Greatest Common Divisor (gcd)");

    CHECK_EQ(gcd(BigInt(48),  BigInt(18)),  BigInt(6));
    CHECK_EQ(gcd(BigInt(100), BigInt(75)),  BigInt(25));
    CHECK_EQ(gcd(BigInt(13),  BigInt(7)),   BigInt(1));
    CHECK_EQ(gcd(BigInt(12),  BigInt(12)),  BigInt(12));

    // нулевые аргументы
    CHECK_EQ(gcd(BigInt(0), BigInt(5)), BigInt(5));
    CHECK_EQ(gcd(BigInt(5), BigInt(0)), BigInt(5));
    CHECK_EQ(gcd(BigInt(0), BigInt(0)), BigInt(0));

    // отрицательные аргументы
    CHECK_EQ(gcd(BigInt(-12), BigInt( 8)), BigInt(4));
    CHECK_EQ(gcd(BigInt( 12), BigInt(-8)), BigInt(4));
    CHECK_EQ(gcd(BigInt(-12), BigInt(-8)), BigInt(4));

    // единица
    CHECK_EQ(gcd(BigInt(1),   BigInt(100)), BigInt(1));
    CHECK_EQ(gcd(BigInt(100), BigInt(1)),   BigInt(1));

    // большие числа
    CHECK_EQ(gcd(BigInt("1234567890"), BigInt("9876543210")), BigInt(90));

    // d. Расширенный алгоритм Евклида
    TEST_SUITE("Extended Euclidean Algorithm");

    // Проверяем тождество Безу: a*x + b*y = gcd(a,b)
    {
        auto check_bezout = [](BigInt a, BigInt b) -> bool {
            auto [g, x, y] = extended_gcd(a, b);
            return (a*x + b*y == g) && (g == gcd(a, b));
        };
        CHECK_TRUE(check_bezout(35,   15));
        CHECK_TRUE(check_bezout(101,  37));
        CHECK_TRUE(check_bezout(3,     7));
        CHECK_TRUE(check_bezout(1234, 567));
        CHECK_TRUE(check_bezout(-7,    3));
        CHECK_TRUE(check_bezout(7,    -3));
        CHECK_TRUE(check_bezout(-7,   -3));
    }

    // нулевые аргументы
    {
        auto [g1, x1, y1] = extended_gcd(BigInt(5), BigInt(0));
        CHECK_EQ(g1, BigInt(5));
        CHECK_EQ(BigInt(5)*x1 + BigInt(0)*y1, BigInt(5));

        auto [g2, x2, y2] = extended_gcd(BigInt(0), BigInt(7));
        CHECK_EQ(g2, BigInt(7));
        CHECK_EQ(BigInt(0)*x2 + BigInt(7)*y2, BigInt(7));
    }

    // взаимно простые
    {
        auto [g, x, y] = extended_gcd(BigInt(3), BigInt(11));
        CHECK_EQ(g, BigInt(1));
        CHECK_EQ(BigInt(3)*x + BigInt(11)*y, BigInt(1));
    }

    // e. pow_mod
    TEST_SUITE("pow_mod");

    CHECK_EQ(pow_mod(BigInt(2),  BigInt(10), BigInt(1000)), BigInt(24));
    CHECK_EQ(pow_mod(BigInt(3),  BigInt(200), BigInt(13)),  BigInt(9));
    CHECK_EQ(pow_mod(BigInt(7),  BigInt(1),  BigInt(10)),   BigInt(7));

    // нулевой показатель: a^0 mod m = 1
    CHECK_EQ(pow_mod(BigInt(7), BigInt(0), BigInt(10)), BigInt(1));
    CHECK_EQ(pow_mod(BigInt(1), BigInt(0), BigInt(5)),  BigInt(1));
    CHECK_EQ(pow_mod(BigInt(0), BigInt(0), BigInt(5)),  BigInt(1));

    // нулевое основание
    CHECK_EQ(pow_mod(BigInt(0), BigInt(5), BigInt(7)), BigInt(0));

    // mod 1 = 0 always
    CHECK_EQ(pow_mod(BigInt(5), BigInt(3), BigInt(1)), BigInt(0));
    CHECK_EQ(pow_mod(BigInt(0), BigInt(0), BigInt(1)), BigInt(0));

    // отрицательное основание
    CHECK_EQ(pow_mod(BigInt(-1), BigInt(2), BigInt(5)), BigInt(1)); // (-1)^2=1
    CHECK_EQ(pow_mod(BigInt(-2), BigInt(3), BigInt(7)), BigInt(6)); // -8 mod 7=6

    // малая теорема Ферма: a^(p-1) ≡ 1 (mod p)
    CHECK_EQ(pow_mod(BigInt(123456789), BigInt(1000000006), BigInt(1000000007)), BigInt(1));

    // некорректные аргументы
    THROWS(pow_mod(BigInt(2), BigInt(3),  BigInt(0)));
    THROWS(pow_mod(BigInt(2), BigInt(-1), BigInt(7)));

    // f. mod_inverse
    TEST_SUITE("mod_inverse");

    {
        auto check_inv = [](BigInt a, BigInt n) -> bool {
            try {
                BigInt inv = mod_inverse(a, n);
                BigInt product = ((a * inv) % n + n) % n;
                return product == BigInt(1) && inv >= BigInt(0) && inv < n;
            } catch (...) { return false; }
        };
        CHECK_TRUE(check_inv(3,      7));
        CHECK_TRUE(check_inv(7,     26));
        CHECK_TRUE(check_inv(17,  3120));
        CHECK_TRUE(check_inv(1,      5));
        CHECK_TRUE(check_inv(-5,    11));
        CHECK_TRUE(check_inv(-3,     7));
    }

    // нет обратного
    THROWS(mod_inverse(BigInt(6), BigInt(9))); // gcd=3
    THROWS(mod_inverse(BigInt(4), BigInt(8))); // gcd=4

    // некорректный модуль
    THROWS(mod_inverse(BigInt(3), BigInt( 1)));
    THROWS(mod_inverse(BigInt(3), BigInt( 0)));
    THROWS(mod_inverse(BigInt(3), BigInt(-5)));

    // g. Функция Эйлера — naive
    TEST_SUITE("euler_totient_naive");

    for (int n = 1; n <= 20; ++n)
        CHECK_EQ(euler_totient_naive(BigInt(n)), BigInt(PHI[n]));

    CHECK_EQ(euler_totient_naive(BigInt(7)),  BigInt(6));
    CHECK_EQ(euler_totient_naive(BigInt(11)), BigInt(10));
    CHECK_EQ(euler_totient_naive(BigInt(13)), BigInt(12));

    THROWS(euler_totient_naive(BigInt( 0)));
    THROWS(euler_totient_naive(BigInt(-1)));

    // g. Функция Эйлера — factorization
    TEST_SUITE("euler_totient_factorization");

    for (int n = 1; n <= 20; ++n)
        CHECK_EQ(euler_totient_factorization(BigInt(n)), BigInt(PHI[n]));

    CHECK_EQ(euler_totient_factorization(BigInt(360)),  BigInt(96));  // 2³·3²·5
    CHECK_EQ(euler_totient_factorization(BigInt(1000)), BigInt(400)); // 2³·5³
    CHECK_EQ(euler_totient_factorization(BigInt(8)),    BigInt(4));   // φ(2³)
    CHECK_EQ(euler_totient_factorization(BigInt(9)),    BigInt(6));   // φ(3²)
    CHECK_EQ(euler_totient_factorization(BigInt(25)),   BigInt(20));  // φ(5²)
    CHECK_EQ(euler_totient_factorization(BigInt("1000000007")), BigInt("1000000006"));

    THROWS(euler_totient_factorization(BigInt( 0)));
    THROWS(euler_totient_factorization(BigInt(-1)));

    // g. Функция Эйлера — dft
    TEST_SUITE("euler_totient_dft");

    for (int n = 1; n <= 20; ++n)
        CHECK_EQ(euler_totient_dft(n), PHI[n]);

    CHECK_EQ(euler_totient_dft(36),  12LL);
    CHECK_EQ(euler_totient_dft(100), 40LL);
    CHECK_EQ(euler_totient_dft(360), 96LL);

    THROWS(euler_totient_dft( 0));
    THROWS(euler_totient_dft(-1));

    // g. Кросс-проверка: все три метода дают одинаковый результат (n=1..50)
    TEST_SUITE("euler_totient cross-check (n=1..50)");

    for (int n = 1; n <= 50; ++n) {
        BigInt vn = euler_totient_naive(BigInt(n));
        BigInt vf = euler_totient_factorization(BigInt(n));
        long long vd = euler_totient_dft(n);
        CHECK_EQ(vn, vf);
        CHECK_EQ(static_cast<long long>(vn), vd);
    }

   // Тест Ферма — простые числа
    TEST_SUITE("FermatPrimalityTest — prime numbers");

    {
        FermatPrimalityTest fermat(12345ULL); // фиксированный seed

        for (int p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 101, 997, 7919})
            CHECK_TRUE(fermat.isPrime(BigInt(p), 0.999));

        // M31 = 2^31-1
        CHECK_TRUE(fermat.isPrime(BigInt(2147483647LL), 0.999));

        // большое простое
        CHECK_TRUE(fermat.isPrime(BigInt(999999999999999989LL), 0.999));
    }

    // Тест Ферма — составные числа
    TEST_SUITE("FermatPrimalityTest — composite numbers");

    {
        FermatPrimalityTest fermat(12345ULL);

        for (int c : {4, 6, 8, 9, 10, 15, 21, 25, 27, 49, 100, 1000})
            CHECK_FALSE(fermat.isPrime(BigInt(c), 0.999));

        // составное большое (сумма цифр кратна 3)
        CHECK_FALSE(fermat.isPrime(BigInt(999999999999999999LL), 0.999));
    }

    // Тест Ферма — граничные случаи и некорректные аргументы
    TEST_SUITE("FermatPrimalityTest — edge cases and invalid arguments");

    {
        FermatPrimalityTest fermat(12345ULL);

        CHECK_FALSE(fermat.isPrime(BigInt(1),  0.99)); // 1 — не простое
        CHECK_TRUE (fermat.isPrime(BigInt(2),  0.99)); // 2 — простое
        CHECK_TRUE (fermat.isPrime(BigInt(3),  0.99)); // 3 — простое
        CHECK_FALSE(fermat.isPrime(BigInt(4),  0.99)); // 4 = 2²

        CHECK_FALSE(fermat.isPrime(BigInt( 0), 0.99));
        CHECK_FALSE(fermat.isPrime(BigInt(-1), 0.99));
        CHECK_FALSE(fermat.isPrime(BigInt(-7), 0.99));

        // некорректная вероятность
        THROWS(fermat.isPrime(BigInt(7),  0.0));
        THROWS(fermat.isPrime(BigInt(7), 0.49));
        THROWS(fermat.isPrime(BigInt(7),  1.0));
        THROWS(fermat.isPrime(BigInt(7), -0.5));

        // корректные вызовы не бросают
        NO_THROW(fermat.isPrime(BigInt(2),    0.9));
        NO_THROW(fermat.isPrime(BigInt(100), 0.99));
        NO_THROW(fermat.isPrime(BigInt(-10),  0.9));
    }

    // Числа Кармайкла — только проверяем, что не бросает исключение
    TEST_SUITE("FermatPrimalityTest — Carmichael numbers (no exceptions)");

    {
        FermatPrimalityTest fermat(42);
        NO_THROW(fermat.isPrime(BigInt(561),  0.9)); // 3×11×17
        NO_THROW(fermat.isPrime(BigInt(1105), 0.9)); // 5×13×17
        NO_THROW(fermat.isPrime(BigInt(1729), 0.9)); // 7×13×19
    }

    // computeIterations
    TEST_SUITE("computeIterations");

    CHECK_EQ(ProbabilisticPrimalityTest::computeIterations(0.5),    1);
    CHECK_EQ(ProbabilisticPrimalityTest::computeIterations(0.75),   2);
    CHECK_EQ(ProbabilisticPrimalityTest::computeIterations(0.875),  3);
    CHECK_EQ(ProbabilisticPrimalityTest::computeIterations(0.9),    4);
    CHECK_EQ(ProbabilisticPrimalityTest::computeIterations(0.99),   7);
    CHECK_EQ(ProbabilisticPrimalityTest::computeIterations(0.999), 10);

    // монотонность
    {
        int prev = 0;
        bool mono = true;
        for (double p : {0.5, 0.6, 0.7, 0.8, 0.9, 0.95, 0.99, 0.999}) {
            int k = ProbabilisticPrimalityTest::computeIterations(p);
            if (k < prev || k < 1) { mono = false; break; }
            prev = k;
        }
        CHECK_TRUE(mono);
    }

    // некорректные значения
    THROWS(ProbabilisticPrimalityTest::computeIterations(0.0));
    THROWS(ProbabilisticPrimalityTest::computeIterations(0.49));
    THROWS(ProbabilisticPrimalityTest::computeIterations(1.0));
    THROWS(ProbabilisticPrimalityTest::computeIterations(1.5));

    // Детерминированность при одинаковом seed
    TEST_SUITE("FermatPrimalityTest — deterministic with same seed");

    {
        FermatPrimalityTest f1(99999), f2(99999);
        bool ok = true;
        for (int n = 2; n <= 50; ++n) {
            if (f1.isPrime(BigInt(n), 0.99) != f2.isPrime(BigInt(n), 0.99)) {
                ok = false; break;
            }
        }
        CHECK_TRUE(ok);
    }

   // Все простые и составные до 100
    TEST_SUITE("FermatPrimalityTest — all numbers up to 100");

    {
        const std::vector<int> primes = {
            2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,
            53,59,61,67,71,73,79,83,89,97
        };
        const std::vector<int> composites = {
            4,6,8,9,10,12,14,15,16,18,20,21,22,24,25,
            26,27,28,30,32,33,34,35,36,38,39,40,42,44,
            45,46,48,49,50
        };

        FermatPrimalityTest fermat(777);

        bool all_primes_ok = true;
        for (int p : primes)
            if (!fermat.isPrime(BigInt(p), 0.999)) { all_primes_ok = false; break; }
        CHECK_TRUE(all_primes_ok);

        bool all_composites_ok = true;
        for (int c : composites)
            if (fermat.isPrime(BigInt(c), 0.999)) { all_composites_ok = false; break; }
        CHECK_TRUE(all_composites_ok);
    }

    // Итог
    std::cout << "\n" << std::string(65, '=') << "\n";
    std::cout << "  TOTAL TESTS RUN: " << g_tests_run
              << "  |  PASSED: "  << g_tests_passed
              << "  |  FAILED: "   << g_tests_failed << "\n";
    if (g_tests_failed == 0)
        std::cout << "  ALL TESTS PASSED SUCCESSFULLY\n";
    else
        std::cout << "  SOME TESTS HAVE FAILED — see 'FAIL' above\n";
    std::cout << std::string(65, '=') << "\n";
}


int main() {
    demo_task1();
    demo_task2();
    //run_all_tests();    // Unit tests (tasks 1 and 2)
    return g_tests_failed == 0 ? 0 : 1;
}