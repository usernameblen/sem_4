// #include "vulnerable_rsa.hpp"
// #include "fermat_attack.hpp"
// #include <iostream>
// #include <stdexcept>
// #include <string>

// static int tests_passed = 0;
// static int tests_failed = 0;

// #define CHECK(expr) \
//     do { \
//         if (!(expr)) { \
//             std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__ \
//                       << "  CHECK(" #expr ")\n"; \
//             ++tests_failed; \
//         } else { \
//             ++tests_passed; \
//         } \
//     } while(0)

// #define CHECK_EQ(a, b) \
//     do { \
//         auto _a = (a); auto _b = (b); \
//         if (_a != _b) { \
//             std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__ \
//                       << "  CHECK_EQ(" #a ", " #b ")\n" \
//                       << "       lhs = " << _a << "\n" \
//                       << "       rhs = " << _b << "\n"; \
//             ++tests_failed; \
//         } else { \
//             ++tests_passed; \
//         } \
//     } while(0)

// #define THROWS(expr) \
//     do { \
//         bool _threw = false; \
//         try { (void)(expr); } catch (...) { _threw = true; } \
//         if (!_threw) { \
//             std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__ \
//                       << "  THROWS(" #expr ") did not throw\n"; \
//             ++tests_failed; \
//         } else { \
//             ++tests_passed; \
//         } \
//     } while(0)

// static void testFermatKeyGeneration()
// {
//     std::cout << "\n--- testFermatKeyGeneration ---\n";

//     auto [pub, priv] = VulnerableRSAKeyGenerator::generate(64, Vulnerability::Fermat, 0.999);

//     CHECK(pub.N > BigInt(1));
//     CHECK(pub.e == BigInt(65537));
//     CHECK(priv.p > BigInt(1));
//     CHECK(priv.q > BigInt(1));
//     CHECK_EQ(priv.p * priv.q, pub.N);

//     BigInt phi = (priv.p - BigInt(1)) * (priv.q - BigInt(1));
//     CHECK_EQ((pub.e * priv.d) % phi, BigInt(1));

//     std::cout << "  p*q == N: OK\n";
//     std::cout << "  e*d mod phi == 1: OK\n";
// }

// static void testFermatVulnerabilityCondition()
// {
//     std::cout << "\n--- testFermatVulnerabilityCondition ---\n";

//     auto [pub, priv] = VulnerableRSAKeyGenerator::generate(64, Vulnerability::Fermat, 0.999);

//     BigInt diff = (priv.p > priv.q) ? (priv.p - priv.q) : (priv.q - priv.p);
//     BigInt threshold = BigInt(2) * sqrt(sqrt(pub.N));

//     CHECK(diff <= threshold);
//     std::cout << "  |p-q| = " << diff << "\n";
//     std::cout << "  2*N^(1/4) = " << threshold << "\n";
//     std::cout << "  |p-q| <= 2*N^(1/4): " << (diff <= threshold ? "OK" : "FAIL") << "\n";
// }

// static void testWienerKeyGeneration()
// {
//     std::cout << "\n--- testWienerKeyGeneration ---\n";

//     auto [pub, priv] = VulnerableRSAKeyGenerator::generate(64, Vulnerability::Wiener, 0.999);

//     CHECK(pub.N > BigInt(1));
//     CHECK(priv.p > BigInt(1));
//     CHECK(priv.q > BigInt(1));
//     CHECK_EQ(priv.p * priv.q, pub.N);

//     BigInt phi = (priv.p - BigInt(1)) * (priv.q - BigInt(1));
//     CHECK_EQ((pub.e * priv.d) % phi, BigInt(1));

//     std::cout << "  p*q == N: OK\n";
//     std::cout << "  e*d mod phi == 1: OK\n";
// }

// static void testWienerVulnerabilityCondition()
// {
//     std::cout << "\n--- testWienerVulnerabilityCondition ---\n";

//     auto [pub, priv] = VulnerableRSAKeyGenerator::generate(64, Vulnerability::Wiener, 0.999);

//     BigInt threshold = sqrt(sqrt(pub.N));

//     CHECK(priv.d < threshold);
//     std::cout << "  d = " << priv.d << "\n";
//     std::cout << "  N^(1/4) = " << threshold << "\n";
//     std::cout << "  d < N^(1/4): " << (priv.d < threshold ? "OK" : "FAIL") << "\n";
// }

// static void testErrorHandling()
// {
//     std::cout << "\n--- testErrorHandling ---\n";

//     THROWS(VulnerableRSAKeyGenerator::generate(8, Vulnerability::Fermat, 0.999));
//     THROWS(VulnerableRSAKeyGenerator::generate(64, Vulnerability::Fermat, 0.3));
//     THROWS(VulnerableRSAKeyGenerator::generate(64, Vulnerability::Fermat, 1.0));
//     THROWS(VulnerableRSAKeyGenerator::generate(0, Vulnerability::Wiener, 0.999));

//     std::cout << "  All error cases threw as expected: OK\n";
// }

// static void testFermatAttackKnownExample()
// {
//     std::cout << "\n--- testFermatAttackKnownExample ---\n";

//     BigInt p = BigInt(9999991);
//     BigInt q = BigInt(9999973);
//     BigInt N = p * q;

//     auto [fp, fq] = fermatAttack(N);

//     CHECK(fp * fq == N);
//     CHECK(fp > BigInt(1));
//     CHECK(fq > BigInt(1));

//     BigInt recovered_p = (fp < fq) ? fp : fq;
//     BigInt recovered_q = (fp < fq) ? fq : fp;
//     BigInt orig_p      = (p  < q)  ? p  : q;
//     BigInt orig_q      = (p  < q)  ? q  : p;

//     CHECK_EQ(recovered_p, orig_p);
//     CHECK_EQ(recovered_q, orig_q);

//     std::cout << "  N = " << N << "\n";
//     std::cout << "  Recovered p = " << fp << ", q = " << fq << "\n";
//     std::cout << "  p*q == N: " << (fp * fq == N ? "OK" : "FAIL") << "\n";
// }

// static void testFermatAttackOnGeneratedKey()
// {
//     std::cout << "\n--- testFermatAttackOnGeneratedKey ---\n";

//     auto [pub, priv] = VulnerableRSAKeyGenerator::generate(64, Vulnerability::Fermat, 0.999);

//     auto [fp, fq] = fermatAttack(pub.N);

//     CHECK(fp * fq == pub.N);
//     CHECK(fp > BigInt(1));
//     CHECK(fq > BigInt(1));

//     std::cout << "  N = " << pub.N << "\n";
//     std::cout << "  Recovered p = " << fp << "\n";
//     std::cout << "  Recovered q = " << fq << "\n";
//     std::cout << "  p*q == N: " << (fp * fq == pub.N ? "OK" : "FAIL") << "\n";
// }

// static void testFermatAttackErrors()
// {
//     std::cout << "\n--- testFermatAttackErrors ---\n";

//     THROWS(fermatAttack(BigInt(2)));
//     THROWS(fermatAttack(BigInt(0)));
//     THROWS(fermatAttack(BigInt(4)));

//     std::cout << "  All error cases threw as expected: OK\n";
// }

// int main()
// {
//     std::cout << "========== UNIT TESTS ==========\n";

//     testFermatKeyGeneration();
//     testFermatVulnerabilityCondition();
//     testWienerKeyGeneration();
//     testWienerVulnerabilityCondition();
//     testErrorHandling();
//     testFermatAttackKnownExample();
//     testFermatAttackOnGeneratedKey();
//     testFermatAttackErrors();

//     std::cout << "\n========== RESULTS ==========\n";
//     std::cout << "Passed: " << tests_passed << "\n";
//     std::cout << "Failed: " << tests_failed << "\n";

//     std::cout << "\n========== DEMO ==========\n";

//     std::cout << "\n[1] Generating RSA key vulnerable to Fermat attack (64-bit)...\n";
//     auto [fermat_pub, fermat_priv] = VulnerableRSAKeyGenerator::generate(
//         64, Vulnerability::Fermat, 0.9999);

//     std::cout << "  N = " << fermat_pub.N << "\n";
//     std::cout << "  e = " << fermat_pub.e << "\n";
//     std::cout << "  p = " << fermat_priv.p << "\n";
//     std::cout << "  q = " << fermat_priv.q << "\n";

//     BigInt diff = (fermat_priv.p > fermat_priv.q)
//         ? (fermat_priv.p - fermat_priv.q)
//         : (fermat_priv.q - fermat_priv.p);
//     BigInt threshold_fermat = BigInt(2) * sqrt(sqrt(fermat_pub.N));
//     std::cout << "  |p-q| = " << diff
//               << " <= 2*N^(1/4) = " << threshold_fermat
//               << ": " << (diff <= threshold_fermat ? "YES (vulnerable)" : "NO") << "\n";

//     std::cout << "\n[2] Running Fermat attack on N...\n";
//     auto [fp, fq] = fermatAttack(fermat_pub.N);
//     std::cout << "  Recovered p = " << fp << "\n";
//     std::cout << "  Recovered q = " << fq << "\n";
//     std::cout << "  p*q == N: " << (fp * fq == fermat_pub.N ? "YES" : "NO") << "\n";

//     std::cout << "\n[3] Generating RSA key vulnerable to Wiener attack (64-bit)...\n";
//     auto [wiener_pub, wiener_priv] = VulnerableRSAKeyGenerator::generate(
//         64, Vulnerability::Wiener, 0.9999);

//     BigInt threshold_wiener = sqrt(sqrt(wiener_pub.N));
//     std::cout << "  N       = " << wiener_pub.N << "\n";
//     std::cout << "  e       = " << wiener_pub.e << "\n";
//     std::cout << "  d       = " << wiener_priv.d << "\n";
//     std::cout << "  N^(1/4) = " << threshold_wiener << "\n";
//     std::cout << "  d < N^(1/4): "
//               << (wiener_priv.d < threshold_wiener ? "YES (vulnerable)" : "NO") << "\n";

//     return tests_failed == 0 ? 0 : 1;
// }
