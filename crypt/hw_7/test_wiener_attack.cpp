#include "wiener_attack.hpp"
#include "vulnerable_rsa.hpp"
#include "continued_fraction.hpp"
#include <iostream>
#include <stdexcept>

static int tests_passed = 0;
static int tests_failed = 0;

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__ \
                      << "  CHECK(" #expr ")\n"; \
            ++tests_failed; \
        } else { \
            ++tests_passed; \
        } \
    } while(0)

#define CHECK_EQ(a, b) \
    do { \
        auto _a = (a); auto _b = (b); \
        if (_a != _b) { \
            std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__ \
                      << "  CHECK_EQ(" #a ", " #b ")\n" \
                      << "       lhs = " << _a << "\n" \
                      << "       rhs = " << _b << "\n"; \
            ++tests_failed; \
        } else { \
            ++tests_passed; \
        } \
    } while(0)

#define THROWS(expr) \
    do { \
        bool _threw = false; \
        try { (void)(expr); } catch (...) { _threw = true; } \
        if (!_threw) { \
            std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__ \
                      << "  THROWS(" #expr ") did not throw\n"; \
            ++tests_failed; \
        } else { \
            ++tests_passed; \
        } \
    } while(0)

static void testWienerAttackOnVulnerableKey()
{
    std::cout << "\n--- testWienerAttackOnVulnerableKey (64-bit) ---\n";

    auto [pub, priv] = VulnerableRSAKeyGenerator::generate(64, Vulnerability::Wiener, 0.9999);

    auto [p, q] = wienerAttack(pub);

    CHECK(p * q == pub.N);
    CHECK(p > BigInt(1));
    CHECK(q > BigInt(1));

    BigInt recovered_p = (p < q) ? p : q;
    BigInt recovered_q = (p < q) ? q : p;
    BigInt orig_p      = (priv.p < priv.q) ? priv.p : priv.q;
    BigInt orig_q      = (priv.p < priv.q) ? priv.q : priv.p;

    CHECK_EQ(recovered_p, orig_p);
    CHECK_EQ(recovered_q, orig_q);

    std::cout << "  p*q == N: OK\n";
    std::cout << "  Recovered factors match originals: OK\n";
}

static void testWienerAttackOnVulnerableKey128()
{
    std::cout << "\n--- testWienerAttackOnVulnerableKey (128-bit) ---\n";

    auto [pub, priv] = VulnerableRSAKeyGenerator::generate(128, Vulnerability::Wiener, 0.9999);

    auto [p, q] = wienerAttack(pub);

    CHECK(p * q == pub.N);
    CHECK(p > BigInt(1));
    CHECK(q > BigInt(1));

    std::cout << "  p*q == N: OK\n";
}

static void testWienerAttackFailsOnSafeKey()
{
    std::cout << "\n--- testWienerAttackFailsOnSafeKey ---\n";

    auto [pub, priv] = VulnerableRSAKeyGenerator::generate(64, Vulnerability::Fermat, 0.9999);

    bool threw = false;
    try {
        wienerAttack(pub);
    } catch (const std::runtime_error&) {
        threw = true;
    } catch (...) {
    }

    CHECK(threw);
    std::cout << "  Attack correctly failed on non-Wiener key: OK\n";
}

static void testWienerAttackInputValidation()
{
    std::cout << "\n--- testWienerAttackInputValidation ---\n";

    PublicKey bad1 { BigInt(3), BigInt(1) };
    THROWS(wienerAttack(bad1));

    PublicKey bad2 { BigInt(1), BigInt(65537) };
    THROWS(wienerAttack(bad2));

    PublicKey bad3 { BigInt(15), BigInt(0) };
    THROWS(wienerAttack(bad3));

    std::cout << "  All invalid inputs threw: OK\n";
}

static void testWienerAttackVerifyD()
{
    std::cout << "\n--- testWienerAttackVerifyD ---\n";

    auto [pub, priv] = VulnerableRSAKeyGenerator::generate(64, Vulnerability::Wiener, 0.9999);

    auto [p, q] = wienerAttack(pub);

    BigInt phi = (p - BigInt(1)) * (q - BigInt(1));
    BigInt d_recovered = mod_inverse(pub.e, phi);

    CHECK_EQ(d_recovered, priv.d);
    std::cout << "  Recovered d matches original: OK\n";
}

static void demo()
{
    std::cout << "\n========== DEMO ==========\n";

    for (int bits : {64, 128}) {
        std::cout << "\n[" << bits << "-bit] Generating Wiener-vulnerable RSA key...\n";

        auto [pub, priv] = VulnerableRSAKeyGenerator::generate(bits, Vulnerability::Wiener, 0.9999);

        BigInt threshold = sqrt(sqrt(pub.N));
        std::cout << "  N        = " << pub.N << "\n";
        std::cout << "  e        = " << pub.e << "\n";
        std::cout << "  d        = " << priv.d << "\n";
        std::cout << "  N^(1/4)  = " << threshold << "\n";
        std::cout << "  d < N^(1/4): " << (priv.d < threshold ? "YES (vulnerable)" : "NO") << "\n";

        std::cout << "\n  Running Wiener attack...\n";
        auto [p, q] = wienerAttack(pub);
        std::cout << "  Recovered p = " << p << "\n";
        std::cout << "  Recovered q = " << q << "\n";
        std::cout << "  p*q == N: " << (p * q == pub.N ? "YES" : "NO") << "\n";

        BigInt phi = (p - BigInt(1)) * (q - BigInt(1));
        BigInt d_check = mod_inverse(pub.e, phi);
        std::cout << "  Recovered d = " << d_check << "\n";
        std::cout << "  d matches original: " << (d_check == priv.d ? "YES" : "NO") << "\n";
    }
}

int main()
{
    std::cout << "========== UNIT TESTS ==========\n";

    testWienerAttackOnVulnerableKey();
    testWienerAttackOnVulnerableKey128();
    testWienerAttackFailsOnSafeKey();
    testWienerAttackInputValidation();
    testWienerAttackVerifyD();

    std::cout << "\n========== RESULTS ==========\n";
    std::cout << "Passed: " << tests_passed << "\n";
    std::cout << "Failed: " << tests_failed << "\n";

    demo();

    return tests_failed == 0 ? 0 : 1;
}