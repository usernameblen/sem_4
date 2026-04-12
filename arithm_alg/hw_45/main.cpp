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




//// hw_5
//#include "Polynomial.h"
//#include <iostream>
//#include <iomanip>
//
//int main() {
//    using Poly = Polynomial<double>;
//
//    //1(a)
//    std::vector<std::string> vars_xyz = { "x", "y", "z" };
//
//    Poly f_a(vars_xyz);
//    f_a.addTerm(2.0, { {"x",1} });
//    f_a.addTerm(3.0, { {"y",1} });
//    f_a.addTerm(1.0, { {"z",1} });
//    f_a.addTerm(1.0, { {"x",2} });
//    f_a.addTerm(-1.0, { {"z",2} });
//    f_a.addTerm(1.0, { {"x",3} });
//
//    std::cout << "\n=== TASK 1(a): f(x,y,z) = x^3 + x^2 + 2x + 3y + z - z^2 ===\n";
//    std::cout << "f = " << f_a << "\n\n";
//
//    for (auto order : { MonomialOrder::LEX, MonomialOrder::GRLEX, MonomialOrder::GREVLEX }) {
//        std::string orderName = (order == MonomialOrder::LEX ? "LEX" :
//            order == MonomialOrder::GRLEX ? "GRLEX" : "GREVLEX");
//
//        std::cout << "--- " << orderName << " ---\n";
//
//        auto lm = f_a.leadingMonomial(order);
//        auto lt = f_a.leadingTerm(order);
//        auto md = f_a.multiDegree(order);
//
//        std::cout << "LM(f) = " << Poly::monomToString(1.0, lm.exponents) << "\n";
//        std::cout << "LT(f) = " << Poly::monomToString(lt.coeff, lt.exponents) << "\n";
//
//        std::cout << "multideg(f) = [";
//        for (size_t i = 0; i < md.size(); ++i) {
//            std::cout << md[i];
//            if (i < md.size() - 1) std::cout << ", ";
//        }
//        std::cout << "]\n\n";
//    }
//
//    //ЗАДАНИЕ 1(b)
//    Poly f_b(vars_xyz);
//    f_b.addTerm(2.0, { {"x",2},{"y",8} });
//    f_b.addTerm(-3.0, { {"x",5},{"y",1},{"z",4} });
//    f_b.addTerm(1.0, { {"x",1},{"y",1},{"z",3} });
//    f_b.addTerm(-1.0, { {"x",1},{"y",4} });
//
//    std::cout << "\n=== TASK 1(b): f(x,y,z) = 2x^2 y^8 - 3x^5 y z^4 + x y z^3 - x y^4 ===\n";
//    std::cout << "f = " << f_b << "\n\n";
//
//    for (auto order : { MonomialOrder::LEX, MonomialOrder::GRLEX, MonomialOrder::GREVLEX }) {
//        std::string orderName = (order == MonomialOrder::LEX ? "LEX" :
//            order == MonomialOrder::GRLEX ? "GRLEX" : "GREVLEX");
//
//        std::cout << "--- " << orderName << " ---\n";
//
//        auto lm = f_b.leadingMonomial(order);
//        auto lt = f_b.leadingTerm(order);
//        auto md = f_b.multiDegree(order);
//
//        std::cout << "LM(f) = " << Poly::monomToString(1.0, lm.exponents) << "\n";
//        std::cout << "LT(f) = " << Poly::monomToString(lt.coeff, lt.exponents) << "\n";
//
//        std::cout << "multideg(f) = [";
//        for (size_t i = 0; i < md.size(); ++i) {
//            std::cout << md[i];
//            if (i < md.size() - 1) std::cout << ", ";
//        }
//        std::cout << "]\n\n";
//    }
//
//    //2
//    std::vector<std::string> vars_xyz2 = { "x", "y", "z" };
//
//    std::cout << "\n=== TASK 2: ===\n";

    //// 2(a): 7x^2 * y^4 * z - 2xy^6 + x^2 * y^2
    //Poly f2a(vars_xyz2);
    //f2a.addTerm(7.0, { {"x",2},{"y",4},{"z",1} });
    //f2a.addTerm(-2.0, { {"x",1},{"y",6} });
    //f2a.addTerm(1.0, { {"x",2},{"y",2} });

    //std::cout << "\nf = 7x^2 * y^4 * z - 2xy^6 + x^2 * y^2\n";
    //std::cout << "\n2(a): LM(GRLEX) = " << Poly::monomToString(1.0, f2a.leadingMonomial(MonomialOrder::GRLEX).exponents) << "\n";
    //std::cout << "2(a): LM(LEX) = " << Poly::monomToString(1.0, f2a.leadingMonomial(MonomialOrder::LEX).exponents) << "\n";
    //std::cout << "2(a): LM(GREVLEX) = " << Poly::monomToString(1.0, f2a.leadingMonomial(MonomialOrder::GREVLEX).exponents) << "\n\n";

    //// 2(b): xy^3 * z + xy^2 * z^2 + x^2 * z^3
    //Poly f2b(vars_xyz2);
    //f2b.addTerm(1.0, { {"x",1},{"y",3},{"z",1} });
    //f2b.addTerm(1.0, { {"x",1},{"y",2},{"z",2} });
    //f2b.addTerm(1.0, { {"x",2},{"z",3} });

    //std::cout << "\nf = xy^3 * z + xy^2 * z^2 + x^2 * z^3\n";
    //std::cout << "2(b): LM(GREVLEX) = " << Poly::monomToString(1.0, f2b.leadingMonomial(MonomialOrder::GREVLEX).exponents) << "\n";
    //std::cout << "2(b): LM(LEX) = " << Poly::monomToString(1.0, f2b.leadingMonomial(MonomialOrder::LEX).exponents) << "\n";
    //std::cout << "2(b): LM(GRLEX) = " << Poly::monomToString(1.0, f2b.leadingMonomial(MonomialOrder::GRLEX).exponents) << "\n";

    //// 3: z > y > x
    //std::vector<std::string> vars_zyx = { "z", "y", "x" };

    //Poly f3a(vars_zyx);
    //f3a.addTerm(2.0, { {"x",1} });
    //f3a.addTerm(3.0, { {"y",1} });
    //f3a.addTerm(1.0, { {"z",1} });
    //f3a.addTerm(1.0, { {"x",2} });
    //f3a.addTerm(-1.0, { {"z",2} });
    //f3a.addTerm(1.0, { {"x",3} });

    //std::cout << "\n=== TASK 3: z > y > x ===\n";
    //std::cout << "1(a), LEX: LM(f) = " << Poly::monomToString(1.0, f3a.leadingMonomial(MonomialOrder::LEX).exponents) << "\n";

    //std::cout << "f = " << f3a << "\n\n";

    //for (auto order : { MonomialOrder::LEX, MonomialOrder::GRLEX, MonomialOrder::GREVLEX }) {
    //    std::string orderName = (order == MonomialOrder::LEX ? "LEX" :
    //        order == MonomialOrder::GRLEX ? "GRLEX" : "GREVLEX");

    //    std::cout << "--- " << orderName << " ---\n";

    //    auto lm = f3a.leadingMonomial(order);
    //    auto lt = f3a.leadingTerm(order);
    //    auto md = f3a.multiDegree(order);

    //    std::cout << "LM(f) = " << Poly::monomToString(1.0, lm.exponents) << "\n";
    //    std::cout << "LT(f) = " << Poly::monomToString(lt.coeff, lt.exponents) << "\n";

    //    std::cout << "multideg(f) = [";
    //    for (size_t i = 0; i < md.size(); ++i) {
    //        std::cout << md[i];
    //        if (i < md.size() - 1) std::cout << ", ";
    //    }
    //    std::cout << "]\n\n";
    //}

    ////дел 1(a)
    //std::vector<std::string> vars_xy = { "x", "y" };

    //Poly f_div(vars_xy);
    //f_div.addTerm(1.0, { {"x",7},{"y",2} });
    //f_div.addTerm(1.0, { {"x",3},{"y",2} });
    //f_div.addTerm(-1.0, { {"y",1} });
    //f_div.addTerm(1.0, {});

    //Poly g1(vars_xy); // xy² - x
    //g1.addTerm(1.0, { {"x",1},{"y",2} });
    //g1.addTerm(-1.0, { {"x",1} });

    //Poly g2(vars_xy); // x - y³
    //g2.addTerm(1.0, { {"x",1} });
    //g2.addTerm(-1.0, { {"y",3} });

    //std::vector<Poly> F1 = { g1, g2 };
    //std::vector<Poly> F2 = { g2, g1 };

    //std::cout << "\n=== DIVISION TASK 1(a) ===\n";
    //std::cout << "f = x^7 y^2 + x^3 y^2 - y + 1\n";

    //std::cout << "\nGRLEX, F=(xy^2 - x, x - y^3):\n";
    //auto res1 = f_div.divideBy(F1, MonomialOrder::GRLEX);
    //std::cout << "q1 = " << res1.quotients[0] << "\n";
    //std::cout << "q2 = " << res1.quotients[1] << "\n";
    //std::cout << "r  = " << res1.remainder << "\n";

    //std::cout << "\nGRLEX, F=(x-y^3, xy^2 - x):\n";
    //auto res2 = f_div.divideBy(F2, MonomialOrder::GRLEX);
    //std::cout << "q1 = " << res2.quotients[0] << "\n";
    //std::cout << "q2 = " << res2.quotients[1] << "\n";
    //std::cout << "r  = " << res2.remainder << "\n";

    ////дел 2(a)
    //std::vector<std::string> vars_xyz_div = { "x", "y", "z" };

    //Poly f_div2(vars_xyz_div);
    //f_div2.addTerm(1.0, { {"x",1},{"y",2},{"z",2} });
    //f_div2.addTerm(1.0, { {"x",1},{"y",1} });
    //f_div2.addTerm(-1.0, { {"y",1},{"z",1} });

    //Poly h1(vars_xyz_div); 
    //h1.addTerm(1.0, { {"x",1} }); 
    //h1.addTerm(-1.0, { {"y",2} });

    //Poly h2(vars_xyz_div); 
    //h2.addTerm(1.0, { {"y",1} }); 
    //h2.addTerm(-1.0, { {"z",3} });

    //Poly h3(vars_xyz_div); 
    //h3.addTerm(1.0, { {"z",2} }); 
    //h3.addTerm(-1.0, {});

    //std::vector<Poly> F3_1 = { h1, h2, h3 };

    //std::cout << "\n=== DIVISION TASK 2(a) ===\n";
    //std::cout << "f = xy^2 * z^2 + xy - yz, F = (x - y^2, y - z^3, z^2 - 1)\n";

    //auto res3_1 = f_div2.divideBy(F3_1, MonomialOrder::GRLEX);
    //std::cout << "q1 = " << res3_1.quotients[0] << "\n";
    //std::cout << "q2 = " << res3_1.quotients[1] << "\n";
    //std::cout << "q3 = " << res3_1.quotients[2] << "\n";
    //std::cout << "r  = " << res3_1.remainder << "\n\n";

    //std::vector<Poly> F3_2 = { h3, h1, h2 };

    //auto res3_2 = f_div2.divideBy(F3_2, MonomialOrder::GRLEX);
    //std::cout << "q1 = " << res3_2.quotients[0] << "\n";
    //std::cout << "q2 = " << res3_2.quotients[1] << "\n";
    //std::cout << "q3 = " << res3_2.quotients[2] << "\n";
    //std::cout << "r  = " << res3_2.remainder << "\n\n";


    //std::vector<Poly> F3_3 = { h2, h3, h1 };

    //auto res3_3 = f_div2.divideBy(F3_2, MonomialOrder::GRLEX);
    //std::cout << "q1 = " << res3_3.quotients[0] << "\n";
    //std::cout << "q2 = " << res3_3.quotients[1] << "\n";
    //std::cout << "q3 = " << res3_3.quotients[2] << "\n";
    //std::cout << "r  = " << res3_3.remainder << "\n\n";



//    //21.6
//    Poly f_216(vars_xyz);
//    f_216.addTerm(2.0, { {"x",4},{"y",2},{"z",1} });
//    f_216.addTerm(-6.0, { {"x",4},{"y",1},{"z",2} });
//    f_216.addTerm(4.0, { {"x",1},{"y",4},{"z",2} });
//    f_216.addTerm(-3.0, { {"x",1},{"y",2},{"z",4} });
//    f_216.addTerm(1.0, { {"x",2},{"y",4},{"z",1} });
//    f_216.addTerm(-5.0, { {"x",2},{"y",1},{"z",4} });
//
//    std::cout << "\n=== TASK 21.6 ===\n";
//    for (auto order : { MonomialOrder::LEX, MonomialOrder::GRLEX, MonomialOrder::GREVLEX }) {
//        std::string orderName = (order == MonomialOrder::LEX ? "LEX" :
//            order == MonomialOrder::GRLEX ? "GRLEX" : "GREVLEX");
//
//        auto lm = f_216.leadingMonomial(order);
//        auto lt = f_216.leadingTerm(order);
//
//        std::cout << orderName << ":\n";
//        std::cout << "  LM(f) = " << Poly::monomToString(1.0, lm.exponents) << "\n";
//        std::cout << "  LT(f) = " << Poly::monomToString(lt.coeff, lt.exponents) << "\n";
//        std::cout << "  LC(f) = " << lt.coeff << "\n";
//        std::cout << "  mdeg(f) = [4, 2, 1]\n\n";
//    }
//
//    return 0;
//}




//hw 7
#include "Polynomial.h"
#include <iostream>

using Poly = Polynomial<double>;

static void sep(const std::string& title) {
    std::cout << "\n------>" << title << "\n";
}

static void printSPoly(const std::string& label, const Poly& sp, MonomialOrder ord) {
    std::cout << label << " S(f,g) = " << sp << "\n";
    if (!sp.isZero()) {
        auto md = sp.multiDegree(ord);
        std::cout << "  multideg(S) = [";
        for (size_t k = 0; k < md.size(); ++k) { std::cout << md[k]; if (k + 1 < md.size()) std::cout << ","; }
        std::cout << "]\n";
    }
    else {
        std::cout << "  S(f,g) = 0\n";
    }
}

static void checkGB(const std::string& label, const std::vector<Poly>& G,
    MonomialOrder ord, const std::string& ordName) {
    bool ok = Poly::isGroebnerBasis(G, ord);
    std::cout << label << " [" << ordName << "]: "
        << (ok ? "IS a Groebner basis" : "is NOT a Groebner basis") << "\n";
    if (!ok) {
        int s = (int)G.size();
        for (int i = 0; i < s; ++i)
            for (int j = i + 1; j < s; ++j) {
                Poly sp = Poly::S_polynomial(G[i], G[j], ord);
                auto res = sp.divideBy(G, ord);
                if (!res.remainder.isZero())
                    std::cout << "  S(G[" << i << "],G[" << j << "]) rem G = "
                    << res.remainder << "  (nonzero!)\n";
            }
    }
}

int main() {

    sep("Cox 2.5.7 - G={x^4 * y^2 - z^5, x^3 * y^3 - 1, x^2 * y^4 - 2z}, grlex x>y>z");
    {
        std::vector<std::string> v = { "x","y","z" };
        Poly g1(v), g2(v), g3(v);
        g1.addTerm(1.0, { {"x",4},{"y",2} });
        g1.addTerm(-1.0, { {"z",5} });
        g2.addTerm(1.0, { {"x",3},{"y",3} });
        g2.addTerm(-1.0, {});
        g3.addTerm(1.0, { {"x",2},{"y",4} });
        g3.addTerm(-2.0, { {"z",1} });

        std::cout << "g1 = " << g1 << "\n";
        std::cout << "g2 = " << g2 << "\n";
        std::cout << "g3 = " << g3 << "\n\n";

        std::vector<Poly> G = { g1, g2, g3 };
        checkGB("G", G, MonomialOrder::GRLEX, "grlex");
    }

    sep("Cox 2.6.5 - S-polynomials (lex order)");
    {
        std::vector<std::string> v = { "x","y","z" };

        // (a) f=4x^2z-7y^2, g=xyz^2+3xz^4
        {
            Poly f(v), g(v);
            f.addTerm(4.0, { {"x",2},{"z",1} });
            f.addTerm(-7.0, { {"y",2} });
            g.addTerm(1.0, { {"x",1},{"y",1},{"z",2} });
            g.addTerm(3.0, { {"x",1},{"z",4} });
            Poly sp = Poly::S_polynomial(f, g, MonomialOrder::LEX);
            printSPoly("(a)", sp, MonomialOrder::LEX);
        }
        // (b) f=x^4y-z^2, g=3xz^2-y
        {
            Poly f(v), g(v);
            f.addTerm(1.0, { {"x",4},{"y",1} });
            f.addTerm(-1.0, { {"z",2} });
            g.addTerm(3.0, { {"x",1},{"z",2} });
            g.addTerm(-1.0, { {"y",1} });
            Poly sp = Poly::S_polynomial(f, g, MonomialOrder::LEX);
            printSPoly("(b)", sp, MonomialOrder::LEX);
        }
        // (c) f=x^7y^2z+2xyz, g=2x^7y^2z+4
        {
            Poly f(v), g(v);
            f.addTerm(1.0, { {"x",7},{"y",2},{"z",1} });
            f.addTerm(2.0, { {"x",1},{"y",1},{"z",1} });
            g.addTerm(2.0, { {"x",7},{"y",2},{"z",1} });
            g.addTerm(4.0, {});
            Poly sp = Poly::S_polynomial(f, g, MonomialOrder::LEX);
            printSPoly("(c)", sp, MonomialOrder::LEX);
        }
        // (d) f=xy+z^3, g=z^2-3z
        {
            Poly f(v), g(v);
            f.addTerm(1.0, { {"x",1},{"y",1} });
            f.addTerm(1.0, { {"z",3} });
            g.addTerm(1.0, { {"z",2} });
            g.addTerm(-3.0, { {"z",1} });
            Poly sp = Poly::S_polynomial(f, g, MonomialOrder::LEX);
            printSPoly("(d)", sp, MonomialOrder::LEX);
        }
    }

    sep("Cox 2.6.7 - multideg(S(f,g)) < gamma for each pair above");
    {
        std::vector<std::string> v = { "x","y","z" };
        auto printCheck = [&](const std::string& label, Poly& f, Poly& g) {
            MonomialOrder ord = MonomialOrder::LEX;
            auto ltF = f.leadingMonomial(ord);
            auto ltG = g.leadingMonomial(ord);
            auto gamma_exp = Poly::lcm_exponents(ltF.exponents, ltG.exponents);
            int gamma_deg = 0;
            for (auto& kv : gamma_exp)
                gamma_deg += kv.second;
            Poly sp = Poly::S_polynomial(f, g, ord);
            if (sp.isZero()) {
                std::cout << "  S=0, multideg=-inf < gamma=" << gamma_deg << "  OK\n";
            }
            else {
                auto md = sp.multiDegree(ord);
                int s_deg = 0; for (int d : md) s_deg += d;
                std::cout << "  total_deg(S)=" << s_deg
                    << " < gamma_total=" << gamma_deg
                    << (s_deg < gamma_deg ? "  OK" : "  FAIL") << "\n";
            }
            };

        // (a) f = x^4 y - z^2, g = 3 x z^2 - y
        {
            Poly f(v), g(v);
            f.addTerm(1.0, { {"x",4},{"y",1} }); f.addTerm(-1.0, { {"z",2} });
            g.addTerm(3.0, { {"x",1},{"z",2} }); g.addTerm(-1.0, { {"y",1} });
            printCheck("(a)", f, g);
        }
        // (b) f = x^7 y^2 z + 2 x y z, g = 2 x^7 y^2 z + 4
        {
            Poly f(v), g(v);
            f.addTerm(1.0, { {"x",7},{"y",2},{"z",1} }); f.addTerm(2.0, { {"x",1},{"y",1},{"z",1} });
            g.addTerm(2.0, { {"x",7},{"y",2},{"z",1} }); g.addTerm(4.0, {});
            printCheck("(b)", f, g);
        }
        // (c) f = x y + z^3, g = z^2 - 3 z
        {
            Poly f(v), g(v);
            f.addTerm(1.0, { {"x",1},{"y",1} }); f.addTerm(1.0, { {"z",3} });
            g.addTerm(1.0, { {"z",2} }); g.addTerm(-3.0, { {"z",1} });
            printCheck("(c)", f, g);
        }
    }


    sep("Cox 2.6.8 - G={y-x^2, z-x^3}, lex x>y>z (should NOT be GB)");
    {
        std::vector<std::string> v = { "x","y","z" };
        Poly g1(v), g2(v);
        g1.addTerm(1.0, { {"y",1} });
        g1.addTerm(-1.0, { {"x",2} });
        g2.addTerm(1.0, { {"z",1} });
        g2.addTerm(-1.0, { {"x",3} });

        std::cout << "g1 = " << g1 << ",  LT(g1) = "
            << Poly::monomToString(g1.leadingTerm(MonomialOrder::LEX).coeff,
                g1.leadingTerm(MonomialOrder::LEX).exponents) << "\n";
        std::cout << "g2 = " << g2 << ",  LT(g2) = "
            << Poly::monomToString(g2.leadingTerm(MonomialOrder::LEX).coeff,
                g2.leadingTerm(MonomialOrder::LEX).exponents) << "\n";

        Poly sp = Poly::S_polynomial(g1, g2, MonomialOrder::LEX);
        std::cout << "S(g1,g2) = " << sp << "\n";
        std::vector<Poly> G = { g1, g2 };
        auto res = sp.divideBy(G, MonomialOrder::LEX);
        std::cout << "S(g1,g2) rem G = " << res.remainder << "\n";
        checkGB("G", G, MonomialOrder::LEX, "lex");
    }

    sep("Cox 2.6.9 - Buchberger criterion");
    {
        std::vector<std::string> v = { "x","y","z" };

        // (a)
        {
            Poly g1(v), g2(v);
            g1.addTerm(1.0, { {"x",2} }); g1.addTerm(-1.0, { {"y",1} });
            g2.addTerm(1.0, { {"x",3} }); g2.addTerm(-1.0, { {"z",1} });
            std::vector<Poly> G = { g1, g2 };
            Poly sp = Poly::S_polynomial(g1, g2, MonomialOrder::GRLEX);
            auto res = sp.divideBy(G, MonomialOrder::GRLEX);
            std::cout << "(a) G={x^2-y, x^3-z}, grlex\n";
            std::cout << "  S(g1,g2) = " << sp << "\n";
            checkGB("  G", G, MonomialOrder::GRLEX, "grlex");
        }
        // (b)
        {
            Poly g1(v), g2(v);
            g1.addTerm(1.0, { {"x",2} }); g1.addTerm(-1.0, { {"y",1} });
            g2.addTerm(1.0, { {"x",3} }); g2.addTerm(-1.0, { {"z",1} });
            std::vector<Poly> G = { g1, g2 };
            Poly sp = Poly::S_polynomial(g1, g2, MonomialOrder::INVLEX);
            auto res = sp.divideBy(G, MonomialOrder::INVLEX);
            std::cout << "(b) G={x^2-y, x^3-z}, invlex\n";
            std::cout << "  S(g1,g2) = " << sp << "\n";
            checkGB("  G", G, MonomialOrder::INVLEX, "invlex");
        }
        // (c)
        {
            Poly g1(v), g2(v), g3(v);
            g1.addTerm(1.0, { {"x",1},{"y",2} }); g1.addTerm(-1.0, { {"x",1},{"z",1} }); g1.addTerm(1.0, { {"y",1} });
            g2.addTerm(1.0, { {"x",1},{"y",1} }); g2.addTerm(-1.0, { {"z",2} });
            g3.addTerm(1.0, { {"x",1} });          g3.addTerm(-1.0, { {"y",1},{"z",4} });
            std::vector<Poly> G = { g1, g2, g3 };
            std::cout << "(c) G={xy^2-xz+y, xy-z^2, x-yz^4}, lex\n";
            checkGB("  G", G, MonomialOrder::LEX, "lex");
        }
    }

    sep("Gathen 21.21 - which G are Groebner bases? (lex)");
    {
        // (i) {x+y, y^2-1} lex x>y
        {
            std::vector<std::string> v = { "x","y" };
            Poly g1(v), g2(v);
            g1.addTerm(1.0, { {"x",1} }); g1.addTerm(1.0, { {"y",1} });
            g2.addTerm(1.0, { {"y",2} }); g2.addTerm(-1.0, {});
            std::vector<Poly> G = { g1,g2 };
            std::cout << "(i) G={x+y, y^2-1}, lex x>y\n";
            Poly sp = Poly::S_polynomial(g1, g2, MonomialOrder::LEX);
            auto res = sp.divideBy(G, MonomialOrder::LEX);
            std::cout << "  S(g1,g2)=" << sp << ",  rem=" << res.remainder << "\n";
            checkGB("  G", G, MonomialOrder::LEX, "lex");
        }
        // (ii) {y+x, y^2-1} lex y>x
        {
            std::vector<std::string> v = { "y","x" };
            Poly g1(v), g2(v);
            g1.addTerm(1.0, { {"y",1} }); g1.addTerm(1.0, { {"x",1} });
            g2.addTerm(1.0, { {"y",2} }); g2.addTerm(-1.0, {});
            std::vector<Poly> G = { g1,g2 };
            std::cout << "(ii) G={y+x, y^2-1}, lex y>x\n";
            Poly sp = Poly::S_polynomial(g1, g2, MonomialOrder::LEX);
            auto res = sp.divideBy(G, MonomialOrder::LEX);
            std::cout << "  S(g1,g2)=" << sp << ",  rem=" << res.remainder << "\n";
            checkGB("  G", G, MonomialOrder::LEX, "lex");
        }
        // (iii) {x^2+y^2-1, xy-1, x+y^3-y} lex x>y>z
        {
            std::vector<std::string> v = { "x","y","z" };
            Poly g1(v), g2(v), g3(v);
            g1.addTerm(1.0, { {"x",2} }); g1.addTerm(1.0, { {"y",2} }); g1.addTerm(-1.0, {});
            g2.addTerm(1.0, { {"x",1},{"y",1} }); g2.addTerm(-1.0, {});
            g3.addTerm(1.0, { {"x",1} }); g3.addTerm(1.0, { {"y",3} }); g3.addTerm(-1.0, { {"y",1} });
            std::vector<Poly> G = { g1,g2,g3 };
            std::cout << "(iii) G={x^2+y^2-1, xy-1, x+y^3-y}, lex x>y>z\n";
            for (int i = 0; i < 3; ++i)
                for (int j = i + 1; j < 3; ++j) {
                    Poly sp = Poly::S_polynomial(G[i], G[j], MonomialOrder::LEX);
                    auto res = sp.divideBy(G, MonomialOrder::LEX);
                    std::cout << "  S(g" << i + 1 << ",g" << j + 1 << ") rem G = " << res.remainder << "\n";
                }
            checkGB("  G", G, MonomialOrder::LEX, "lex");
        }
        // (iv) {xyz-1, x-y^2z^2-1} lex x>y>z
        {
            std::vector<std::string> v = { "x","y","z" };
            Poly g1(v), g2(v);
            g1.addTerm(1.0, { {"x",1},{"y",1},{"z",1} }); g1.addTerm(-1.0, {});
            g2.addTerm(1.0, { {"x",1} }); g2.addTerm(-1.0, { {"y",2},{"z",2} }); g2.addTerm(-1.0, {});
            std::vector<Poly> G = { g1,g2 };
            std::cout << "(iv) G={xyz-1, x-y^2z^2-1}, lex x>y>z\n";
            Poly sp = Poly::S_polynomial(g1, g2, MonomialOrder::LEX);
            auto res = sp.divideBy(G, MonomialOrder::LEX);
            std::cout << "  S(g1,g2)=" << sp << "\n  rem=" << res.remainder << "\n";
            checkGB("  G", G, MonomialOrder::LEX, "lex");
        }
    }

    sep("Additional: multideg(f), multideg(g), multideg(S(f,g))");
    {
        std::vector<std::string> v = { "x","y","z" };
        Poly f(v), g(v);
        f.addTerm(2.0, { {"x",2},{"y",1} });
        f.addTerm(-1.0, { {"z",3} });
        g.addTerm(3.0, { {"x",1},{"y",2} });
        g.addTerm(1.0, { {"y",1},{"z",2} });
        MonomialOrder ord = MonomialOrder::LEX;
        auto mdf = f.multiDegree(ord);
        auto mdg = g.multiDegree(ord);
        Poly sp = Poly::S_polynomial(f, g, ord);
        auto mdsp = sp.multiDegree(ord);
        auto printMD = [](const std::vector<int>& md) {
            std::cout << "[";
            for (size_t i = 0; i < md.size(); ++i) {
                std::cout << md[i];
                if (i + 1 < md.size()) std::cout << ",";
            }
            std::cout << "]";
            };
        std::cout << "f = " << f << "\n";
        std::cout << "multideg(f) = "; printMD(mdf); std::cout << "\n";
        std::cout << "g = " << g << "\n";
        std::cout << "multideg(g) = "; printMD(mdg); std::cout << "\n";
        std::cout << "S(f,g) = " << sp << "\n";
        std::cout << "multideg(S) = "; printMD(mdsp); std::cout << "\n";
    }

    std::cout << "\nDone.\n";
    return 0;
}