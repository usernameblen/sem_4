//#include <iostream>
//#include <complex>
//#include "polynomial.h"
//
//static void separator(const std::string& title)
//{
//    std::cout << "\n===== " << title << " =====\n";
//}
//
//void demoInt()
//{
//    separator("Integer coefficients  (x, y)");
//
//    // p = 3*x^2*y + y^3
//    Polynomial<int> p({ "x", "y" });
//    p.addTerm(3, { {"x",2}, {"y",1} });
//    p.addTerm(1, { {"y",3} });
//    std::cout << "p = " << p << "\n";
//
//    // q = 2*x^2*y - x
//    Polynomial<int> q({ "x", "y" });
//    q.addTerm(2, { {"x",2}, {"y",1} });
//    q.addTerm(-1, { {"x",1} });
//    std::cout << "q = " << q << "\n";
//
//    auto sum = p + q;
//    std::cout << "p + q = " << sum << "\n";
//
//    auto diff = p - q;
//    std::cout << "p - q = " << diff << "\n";
//
//    auto prod = p * q;
//    std::cout << "p * q = " << prod << "\n";
//
//    Polynomial<int> r(p);
//    r += q;
//    std::cout << "p += q = " << r << "\n";
//
//    std::cout << "p == p : " << (p == p ? "true" : "false") << "\n";
//    std::cout << "p == q : " << (p == q ? "true" : "false") << "\n";
//
//    int val = prod.evaluate({ {"x",2}, {"y",1} });
//    std::cout << "prod(2,1) = " << val
//        << "  (expected: (3*4*1 + 1*1) * (2*4*1 - 2) = 13*6 = 78)\n";
//}
//
//
//void demoDouble() {
//    separator("Double coefficients  (x, y, z)");
//
//    // p = 1.5*x^2 + 2.0*y*z
//    Polynomial<double> p({ "x","y","z" });
//    p.addTerm(1.5, { {"x",2} });
//    p.addTerm(2.0, { {"y",1},{"z",1} });
//    std::cout << "p = " << p << "\n";
//
//    double val = p.evaluate({ {"x",1.0}, {"y",2.0}, {"z",3.0} });
//    std::cout << "p(1,2,3) = " << val
//        << "  (1.5 + 12.0 = 13.5)\n";
//}
//
//
//void demoComplex() {
//    separator("Complex<double> coefficients  (z)");
//
//    using C = std::complex<double>;
//    using namespace std::complex_literals;
//
//    // pc = (1+2i)*z^2 + (3-i)*z
//    Polynomial<C> pc({ "z" });
//    pc.addTerm(1.0 + 2.0i, { {"z",2} });
//    pc.addTerm(3.0 - 1.0i, { {"z",1} });
//    std::cout << "pc = " << pc << "\n";
//
//    C valc = pc.evaluate({ {"z", 1.0i} });
//    std::cout << "pc(i) = " << valc
//        << "  (expected: (1+2i)*(-1) + (3-i)*i = (-1-2i) + (i+1) = (0+i) = i)\n";
//}
//
//
//void demoVariableOrder() {
//    separator("Variable order independence  (x, y) vs (y, x)");
//
//    // p1 built with {"x","y"}, p2 built with {"y","x"}
//    // After canonical sorting both will use {"x","y"} internally
//    Polynomial<int> p1({ "x", "y" });
//    p1.addTerm(3, { {"x",2}, {"y",1} });
//    p1.addTerm(1, { {"y",3} });
//    std::cout << "p1 (declared x,y) = " << p1 << "\n";
//
//    Polynomial<int> p2({ "y", "x" });   // reversed declaration order
//    p2.addTerm(2, { {"x",2}, {"y",1} });
//    p2.addTerm(-1, { {"x",1} });
//    std::cout << "p2 (declared y,x) = " << p2 << "\n";
//
//    auto sum = p1 + p2;
//    std::cout << "p1 + p2 = " << sum << "\n";
//
//    auto prod = p1 * p2;
//    std::cout << "p1 * p2 = " << prod << "\n";
//
//    int val = sum.evaluate({ {"x",1}, {"y",1} });
//    std::cout << "sum(1,1) = " << val << "  (expected: 3+1+2-1 = 5)\n";
//}
//
//
//int main()
//{
//    try {
//        demoInt();
//        demoDouble();
//        demoComplex();
//        demoVariableOrder();
//    }
//    catch (const std::exception& ex) {
//        std::cerr << "Unexpected exception: " << ex.what() << "\n";
//        return 1;
//    }
//    std::cout << "\nAll demos completed successfully.\n";
//    return 0;
//}




// hw_5
#include "Polynomial.h"
#include <iostream>
#include <iomanip>

int main() {
    using Poly = Polynomial<double>;

    //1(a)
    std::vector<std::string> vars_xyz = { "x", "y", "z" };

    Poly f_a(vars_xyz);
    f_a.addTerm(2.0, { {"x",1} });
    f_a.addTerm(3.0, { {"y",1} });
    f_a.addTerm(1.0, { {"z",1} });
    f_a.addTerm(1.0, { {"x",2} });
    f_a.addTerm(-1.0, { {"z",2} });
    f_a.addTerm(1.0, { {"x",3} });

    std::cout << "\n=== TASK 1(a): f(x,y,z) = x^3 + x^2 + 2x + 3y + z - z^2 ===\n";
    std::cout << "f = " << f_a << "\n\n";

    for (auto order : { MonomialOrder::LEX, MonomialOrder::GRLEX, MonomialOrder::GREVLEX }) {
        std::string orderName = (order == MonomialOrder::LEX ? "LEX" :
            order == MonomialOrder::GRLEX ? "GRLEX" : "GREVLEX");

        std::cout << "--- " << orderName << " ---\n";

        auto lm = f_a.leadingMonomial(order);
        auto lt = f_a.leadingTerm(order);
        auto md = f_a.multiDegree(order);

        std::cout << "LM(f) = " << Poly::monomToString(1.0, lm.exponents) << "\n";
        std::cout << "LT(f) = " << Poly::monomToString(lt.coeff, lt.exponents) << "\n";

        std::cout << "multideg(f) = [";
        for (size_t i = 0; i < md.size(); ++i) {
            std::cout << md[i];
            if (i < md.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n\n";
    }

    //ЗАДАНИЕ 1(b)
    Poly f_b(vars_xyz);
    f_b.addTerm(2.0, { {"x",2},{"y",8} });
    f_b.addTerm(-3.0, { {"x",5},{"y",1},{"z",4} });
    f_b.addTerm(1.0, { {"x",1},{"y",1},{"z",3} });
    f_b.addTerm(-1.0, { {"x",1},{"y",4} });

    std::cout << "\n=== TASK 1(b): f(x,y,z) = 2x^2 y^8 - 3x^5 y z^4 + x y z^3 - x y^4 ===\n";
    std::cout << "f = " << f_b << "\n\n";

    for (auto order : { MonomialOrder::LEX, MonomialOrder::GRLEX, MonomialOrder::GREVLEX }) {
        std::string orderName = (order == MonomialOrder::LEX ? "LEX" :
            order == MonomialOrder::GRLEX ? "GRLEX" : "GREVLEX");

        std::cout << "--- " << orderName << " ---\n";

        auto lm = f_b.leadingMonomial(order);
        auto lt = f_b.leadingTerm(order);
        auto md = f_b.multiDegree(order);

        std::cout << "LM(f) = " << Poly::monomToString(1.0, lm.exponents) << "\n";
        std::cout << "LT(f) = " << Poly::monomToString(lt.coeff, lt.exponents) << "\n";

        std::cout << "multideg(f) = [";
        for (size_t i = 0; i < md.size(); ++i) {
            std::cout << md[i];
            if (i < md.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n\n";
    }

    //2
    std::vector<std::string> vars_xyz2 = { "x", "y", "z" };

    std::cout << "\n=== TASK 2: ===\n";

    // 2(a): 7x^2 * y^4 * z - 2xy^6 + x^2 * y^2
    Poly f2a(vars_xyz2);
    f2a.addTerm(7.0, { {"x",2},{"y",4},{"z",1} });
    f2a.addTerm(-2.0, { {"x",1},{"y",6} });
    f2a.addTerm(1.0, { {"x",2},{"y",2} });

    std::cout << "\nf = 7x^2 * y^4 * z - 2xy^6 + x^2 * y^2\n";
    std::cout << "\n2(a): LM(GRLEX) = " << Poly::monomToString(1.0, f2a.leadingMonomial(MonomialOrder::GRLEX).exponents) << "\n";
    std::cout << "2(a): LM(GRLEX) = " << Poly::monomToString(1.0, f2a.leadingMonomial(MonomialOrder::LEX).exponents) << "\n";
    std::cout << "2(a): LM(GRLEX) = " << Poly::monomToString(1.0, f2a.leadingMonomial(MonomialOrder::GREVLEX).exponents) << "\n\n";

    // 2(b): xy^3 * z + xy^2 * z^2 + x^2 * z^3
    Poly f2b(vars_xyz2);
    f2b.addTerm(1.0, { {"x",1},{"y",3},{"z",1} });
    f2b.addTerm(1.0, { {"x",1},{"y",2},{"z",2} });
    f2b.addTerm(1.0, { {"x",2},{"z",3} });

    std::cout << "\nf = xy^3 * z + xy^2 * z^2 + x^2 * z^3\n";
    std::cout << "2(b): LM(GREVLEX) = " << Poly::monomToString(1.0, f2b.leadingMonomial(MonomialOrder::GREVLEX).exponents) << "\n";
    std::cout << "2(b): LM(GREVLEX) = " << Poly::monomToString(1.0, f2b.leadingMonomial(MonomialOrder::LEX).exponents) << "\n";
    std::cout << "2(b): LM(GREVLEX) = " << Poly::monomToString(1.0, f2b.leadingMonomial(MonomialOrder::GRLEX).exponents) << "\n";

    // 3: z > y > x
    std::vector<std::string> vars_zyx = { "z", "y", "x" };

    Poly f3a(vars_zyx);
    f3a.addTerm(2.0, { {"x",1} });
    f3a.addTerm(3.0, { {"y",1} });
    f3a.addTerm(1.0, { {"z",1} });
    f3a.addTerm(1.0, { {"x",2} });
    f3a.addTerm(-1.0, { {"z",2} });
    f3a.addTerm(1.0, { {"x",3} });

    std::cout << "\n=== TASK 3: z > y > x ===\n";
    std::cout << "1(a), LEX: LM(f) = " << Poly::monomToString(1.0, f3a.leadingMonomial(MonomialOrder::LEX).exponents) << "\n";

    std::cout << "f = " << f3a << "\n\n";

    for (auto order : { MonomialOrder::LEX, MonomialOrder::GRLEX, MonomialOrder::GREVLEX }) {
        std::string orderName = (order == MonomialOrder::LEX ? "LEX" :
            order == MonomialOrder::GRLEX ? "GRLEX" : "GREVLEX");

        std::cout << "--- " << orderName << " ---\n";

        auto lm = f3a.leadingMonomial(order);
        auto lt = f3a.leadingTerm(order);
        auto md = f3a.multiDegree(order);

        std::cout << "LM(f) = " << Poly::monomToString(1.0, lm.exponents) << "\n";
        std::cout << "LT(f) = " << Poly::monomToString(lt.coeff, lt.exponents) << "\n";

        std::cout << "multideg(f) = [";
        for (size_t i = 0; i < md.size(); ++i) {
            std::cout << md[i];
            if (i < md.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n\n";
    }

    //дел 1(a)
    std::vector<std::string> vars_xy = { "x", "y" };

    Poly f_div(vars_xy);
    f_div.addTerm(1.0, { {"x",7},{"y",2} });
    f_div.addTerm(1.0, { {"x",3},{"y",2} });
    f_div.addTerm(-1.0, { {"y",1} });
    f_div.addTerm(1.0, {});

    Poly g1(vars_xy); // xy² - x
    g1.addTerm(1.0, { {"x",1},{"y",2} });
    g1.addTerm(-1.0, { {"x",1} });

    Poly g2(vars_xy); // x - y³
    g2.addTerm(1.0, { {"x",1} });
    g2.addTerm(-1.0, { {"y",3} });

    std::vector<Poly> F1 = { g1, g2 };
    std::vector<Poly> F2 = { g2, g1 };

    std::cout << "\n=== DIVISION TASK 1(a) ===\n";
    std::cout << "f = x^7 y^2 + x^3 y^2 - y + 1\n";

    std::cout << "\nGRLEX, F=(xy^2 - x, x - y^3):\n";
    auto res1 = f_div.divideBy(F1, MonomialOrder::GRLEX);
    std::cout << "q1 = " << res1.quotients[0] << "\n";
    std::cout << "q2 = " << res1.quotients[1] << "\n";
    std::cout << "r  = " << res1.remainder << "\n";

    std::cout << "\nGRLEX, F=(x-y^3, xy^2 - x):\n";
    auto res2 = f_div.divideBy(F2, MonomialOrder::GRLEX);
    std::cout << "q1 = " << res2.quotients[0] << "\n";
    std::cout << "q2 = " << res2.quotients[1] << "\n";
    std::cout << "r  = " << res2.remainder << "\n";

    //дел 2(a)
    std::vector<std::string> vars_xyz_div = { "x", "y", "z" };

    Poly f_div2(vars_xyz_div);
    f_div2.addTerm(1.0, { {"x",1},{"y",2},{"z",2} });
    f_div2.addTerm(1.0, { {"x",1},{"y",1} });
    f_div2.addTerm(-1.0, { {"y",1},{"z",1} });

    Poly h1(vars_xyz_div); h1.addTerm(1.0, { {"x",1} }); h1.addTerm(-1.0, { {"y",2} });
    Poly h2(vars_xyz_div); h2.addTerm(1.0, { {"y",1} }); h2.addTerm(-1.0, { {"z",3} });
    Poly h3(vars_xyz_div); h3.addTerm(1.0, { {"z",2} }); h3.addTerm(-1.0, {});

    std::vector<Poly> F3 = { h1, h2, h3 };

    std::cout << "\n=== DIVISION TASK 2(a) ===\n";
    std::cout << "f = xy^2 * z^2 + xy - yz, F = (x - y^2, y - z^3, z^2 - 1)\n";

    auto res3 = f_div2.divideBy(F3, MonomialOrder::GRLEX);
    std::cout << "q1 = " << res3.quotients[0] << "\n";
    std::cout << "q2 = " << res3.quotients[1] << "\n";
    std::cout << "q3 = " << res3.quotients[2] << "\n";
    std::cout << "r  = " << res3.remainder << "\n";

    //21.6
    Poly f_216(vars_xyz);
    f_216.addTerm(2.0, { {"x",4},{"y",2},{"z",1} });
    f_216.addTerm(-6.0, { {"x",4},{"y",1},{"z",2} });
    f_216.addTerm(4.0, { {"x",1},{"y",4},{"z",2} });
    f_216.addTerm(-3.0, { {"x",1},{"y",2},{"z",4} });
    f_216.addTerm(1.0, { {"x",2},{"y",4},{"z",1} });
    f_216.addTerm(-5.0, { {"x",2},{"y",1},{"z",4} });

    std::cout << "\n=== TASK 21.6 ===\n";
    for (auto order : { MonomialOrder::LEX, MonomialOrder::GRLEX, MonomialOrder::GREVLEX }) {
        std::string orderName = (order == MonomialOrder::LEX ? "LEX" :
            order == MonomialOrder::GRLEX ? "GRLEX" : "GREVLEX");

        auto lm = f_216.leadingMonomial(order);
        auto lt = f_216.leadingTerm(order);

        std::cout << orderName << ":\n";
        std::cout << "  LM(f) = " << Poly::monomToString(1.0, lm.exponents) << "\n";
        std::cout << "  LT(f) = " << Poly::monomToString(lt.coeff, lt.exponents) << "\n";
        std::cout << "  LC(f) = " << lt.coeff << "\n";
        std::cout << "  mdeg(f) = [4, 2, 1]\n\n";
    }

    return 0;
}