
#include "continued_fraction.hpp"
#include "stern_brocot.hpp"
#include "calkin_wilf.hpp"
#include <iostream>
#include <string>
#include <vector>

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

static void testExpand()
{
    std::cout << "\n--- testExpand ---\n";

    auto c = ContinuedFraction::expand(BigInt(649), BigInt(200));
    std::vector<BigInt> expected = {3, 4, 12, 4};
    CHECK_EQ(c.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
        CHECK_EQ(c[i], expected[i]);

    auto c2 = ContinuedFraction::expand(BigInt(1), BigInt(1));
    CHECK_EQ(c2.size(), size_t(1));
    CHECK_EQ(c2[0], BigInt(1));

    auto c3 = ContinuedFraction::expand(BigInt(3), BigInt(5));
    std::vector<BigInt> exp3 = {0, 1, 1, 2};
    CHECK_EQ(c3.size(), exp3.size());
    for (size_t i = 0; i < exp3.size(); ++i)
        CHECK_EQ(c3[i], exp3[i]);

    THROWS(ContinuedFraction::expand(BigInt(2), BigInt(4)));
    THROWS(ContinuedFraction::expand(BigInt(-1), BigInt(3)));
    THROWS(ContinuedFraction::expand(BigInt(0), BigInt(3)));

    std::cout << "  OK\n";
}

static void testEvaluate()
{
    std::cout << "\n--- testEvaluate ---\n";

    std::vector<BigInt> c = {3, 4, 12, 4};
    auto [num, den] = ContinuedFraction::evaluate(c);
    CHECK_EQ(num, BigInt(649));
    CHECK_EQ(den, BigInt(200));

    std::vector<BigInt> c2 = {1};
    auto [n2, d2] = ContinuedFraction::evaluate(c2);
    CHECK_EQ(n2, BigInt(1));
    CHECK_EQ(d2, BigInt(1));

    std::vector<BigInt> c3 = {0, 1, 1, 2};
    auto [n3, d3] = ContinuedFraction::evaluate(c3);
    CHECK_EQ(n3, BigInt(3));
    CHECK_EQ(d3, BigInt(5));

    THROWS(ContinuedFraction::evaluate({}));

    std::cout << "  OK\n";
}

static void testConvergents()
{
    std::cout << "\n--- testConvergents ---\n";

    auto cvg = ContinuedFraction::convergents(BigInt(649), BigInt(200));
    CHECK_EQ(cvg.size(), size_t(4));
    CHECK_EQ(cvg[0].first,  BigInt(3));
    CHECK_EQ(cvg[0].second, BigInt(1));
    CHECK_EQ(cvg[1].first,  BigInt(13));
    CHECK_EQ(cvg[1].second, BigInt(4));
    CHECK_EQ(cvg[2].first,  BigInt(159));
    CHECK_EQ(cvg[2].second, BigInt(49));
    CHECK_EQ(cvg[3].first,  BigInt(649));
    CHECK_EQ(cvg[3].second, BigInt(200));

    auto cvg2 = ContinuedFraction::convergents(BigInt(1), BigInt(1));
    CHECK_EQ(cvg2.size(), size_t(1));
    CHECK_EQ(cvg2[0].first,  BigInt(1));
    CHECK_EQ(cvg2[0].second, BigInt(1));

    std::cout << "  OK\n";
}

static void testSternBrocotPath()
{
    std::cout << "\n--- testSternBrocotPath ---\n";

    CHECK_EQ(SternBrocot::path(BigInt(1), BigInt(1)), std::string(""));
    CHECK_EQ(SternBrocot::path(BigInt(1), BigInt(2)), std::string("L"));
    CHECK_EQ(SternBrocot::path(BigInt(2), BigInt(1)), std::string("R"));
    CHECK_EQ(SternBrocot::path(BigInt(3), BigInt(5)), std::string("LRL"));
    CHECK_EQ(SternBrocot::path(BigInt(3), BigInt(2)), std::string("RL"));

    THROWS(SternBrocot::path(BigInt(2), BigInt(4)));
    THROWS(SternBrocot::path(BigInt(0), BigInt(1)));

    std::cout << "  OK\n";
}

static void testSternBrocotFromPath()
{
    std::cout << "\n--- testSternBrocotFromPath ---\n";

    auto [n1, d1] = SternBrocot::fromPath("");
    CHECK_EQ(n1, BigInt(1));
    CHECK_EQ(d1, BigInt(1));

    auto [n2, d2] = SternBrocot::fromPath("L");
    CHECK_EQ(n2, BigInt(1));
    CHECK_EQ(d2, BigInt(2));

    auto [n3, d3] = SternBrocot::fromPath("R");
    CHECK_EQ(n3, BigInt(2));
    CHECK_EQ(d3, BigInt(1));

    auto [n4, d4] = SternBrocot::fromPath("LRL");
    CHECK_EQ(n4, BigInt(3));
    CHECK_EQ(d4, BigInt(5));

    THROWS(SternBrocot::fromPath("LXR"));

    std::cout << "  OK\n";
}

static void testSternBrocotConvergents()
{
    std::cout << "\n--- testSternBrocotConvergents ---\n";

    auto cv = SternBrocot::convergents("");
    CHECK_EQ(cv.size(), size_t(1));
    CHECK_EQ(cv[0].first,  BigInt(1));
    CHECK_EQ(cv[0].second, BigInt(1));

    auto cv2 = SternBrocot::convergents("LRL");
    CHECK_EQ(cv2.size(), size_t(4));
    CHECK_EQ(cv2[0].first,  BigInt(1));
    CHECK_EQ(cv2[0].second, BigInt(1));
    CHECK_EQ(cv2[3].first,  BigInt(3));
    CHECK_EQ(cv2[3].second, BigInt(5));

     THROWS(SternBrocot::convergents("LXR"));

     std::cout << "  OK\n";
}

static void testCalkinWilfPath()
{
     std::cout << "\n--- testCalkinWilfPath ---\n";

     CHECK_EQ(CalkinWilf::path(BigInt(1), BigInt(1)), std::string(""));
     CHECK_EQ(CalkinWilf::path(BigInt(1), BigInt(2)), std::string("L"));
     CHECK_EQ(CalkinWilf::path(BigInt(2), BigInt(1)), std::string("R"));
     CHECK_EQ(CalkinWilf::path(BigInt(3), BigInt(5)), std::string("LRL"));
     CHECK_EQ(CalkinWilf::path(BigInt(3), BigInt(2)), std::string("LR"));

     THROWS(CalkinWilf::path(BigInt(2), BigInt(4)));
     THROWS(CalkinWilf::path(BigInt(0), BigInt(1)));

     std::cout << "  OK\n";
}

static void testCalkinWilfFromPath()
{
     std::cout << "\n--- testCalkinWilfFromPath ---\n";

     auto [n1, d1] = CalkinWilf::fromPath("");
     CHECK_EQ(n1, BigInt(1));
     CHECK_EQ(d1, BigInt(1));

     auto [n2, d2] = CalkinWilf::fromPath("L");
     CHECK_EQ(n2, BigInt(1));
     CHECK_EQ(d2, BigInt(2));

     auto [n3, d3] = CalkinWilf::fromPath("R");
     CHECK_EQ(n3, BigInt(2));
     CHECK_EQ(d3, BigInt(1));

     auto [n4, d4] = CalkinWilf::fromPath("LRL");
     CHECK_EQ(n4, BigInt(3));
     CHECK_EQ(d4, BigInt(5));

      THROWS(CalkinWilf::fromPath("LXR"));

      std::cout << "  OK\n";
}

static void demo()
{
      std::cout << "\n========== DEMO ==========\n";

      std::cout << "\n[a/b] Converting 649/200 to continued fraction:\n";
      auto coeffs = ContinuedFraction::expand(BigInt(649), BigInt(200));
      std::cout << "  [";
      for (size_t i = 0; i < coeffs.size(); ++i)
          std::cout << coeffs[i] << (i + 1 < coeffs.size() ? "; " : "");
      std::cout << "]\n";

      std::cout << "\n[b] Restoring fraction from continued fraction [3; 4, 12, 4]:\n";
      auto [rn, rd] = ContinuedFraction::evaluate(coeffs);
      std::cout << "  " << rn << "/" << rd << "\n";

      std::cout << "\n[c] Convergents for 649/200:\n";
      auto cvg = ContinuedFraction::convergents(BigInt(649), BigInt(200));
      for (auto& [n, d] : cvg)
          std::cout << "  " << n << "/" << d << "\n";

      std::cout << "\n[d] Paths in trees for 3/5:\n";
      std::string sb_path = SternBrocot::path(BigInt(3), BigInt(5));
      std::string cw_path = CalkinWilf::path(BigInt(3), BigInt(5));
      std::cout << "  Stern-Brocot: \"" << sb_path << "\"\n";
      std::cout << "  Calkin-Wilf: \"" << cw_path << "\"\n";

      std::cout << "\n[e] Restoring fraction from path:\n";
      auto [sn, sd] = SternBrocot::fromPath(sb_path);
      std::cout << "  Stern-Brocot \"" << sb_path << "\" => " << sn << "/" << sd << "\n";
      auto [cn, cd] = CalkinWilf::fromPath(cw_path);
      std::cout << "  Calkin-Wilf \"" << cw_path << "\" => " << cn << "/" << cd << "\n";

      std::cout << "\n[f] Convergents by path \"" << sb_path << "\" in Stern-Brocot tree:\n";
      auto sb_cvg = SternBrocot::convergents(sb_path);
      for (auto& [n, d] : sb_cvg)
          std::cout << "  " << n << "/" << d << "\n";
}

int main()
{
      std::cout << "========== UNIT TESTS ==========\n";

      testExpand();
      testEvaluate();
      testConvergents();
      testSternBrocotPath();
      testSternBrocotFromPath();
      testSternBrocotConvergents();
      testCalkinWilfPath();
      testCalkinWilfFromPath();

      std::cout << "\n========== RESULTS ==========\n";
      std::cout << "Passed: " << tests_passed << "\n";
      std::cout << "Failed: " << tests_failed << "\n";

      demo();

      return tests_failed == 0 ? 0 : 1;
}