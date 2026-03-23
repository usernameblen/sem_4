
#include <iostream>
#include <complex>
#include "polynomial.h"

static void separator(const std::string& title) 
{
    std::cout << "\n===== " << title << " =====\n";
}

void demoInt() 
{
    separator("Integer coefficients  (x, y)");

    // p = 3*x^2*y + y^3
    Polynomial<int> p({ "x", "y" });
    p.addTerm(3, { {"x",2}, {"y",1} });
    p.addTerm(1, { {"y",3} });
    std::cout << "p = " << p << "\n";

    // q = 2*x^2*y - x
    Polynomial<int> q({ "x", "y" });
    q.addTerm(2, { {"x",2}, {"y",1} });
    q.addTerm(-1, { {"x",1} });
    std::cout << "q = " << q << "\n";

    auto sum = p + q;
    std::cout << "p + q = " << sum << "\n";

    auto diff = p - q;
    std::cout << "p - q = " << diff << "\n";

    auto prod = p * q;
    std::cout << "p * q = " << prod << "\n";

    Polynomial<int> r(p);
    r += q;
    std::cout << "p += q = " << r << "\n";

    std::cout << "p == p : " << (p == p ? "true" : "false") << "\n";
    std::cout << "p == q : " << (p == q ? "true" : "false") << "\n";

    int val = p.evaluate({ {"x",2}, {"y",1} });
    std::cout << "p(2,1)   = " << val
        << "  (: 3*4*1 + 1*1 = 13)\n";

    std::cout << "support(p):\n";
    for (auto& m : p.support()) {
        std::cout << "  coeff=" << m.coeff << "  exp={";
        for (auto& kv : m.exponents) 
            std::cout << kv.first << "^" << kv.second << " ";
        std::cout << "}\n";
    }

    std::cout << "homogeneousDegree(p) = " << p.homogeneousDegree()
        << "  (3 because all monomials have total degree 3)\n";

    auto h3 = p.homogeneousPart(3);
    std::cout << "homogeneousPart(p,3) = " << h3 << "\n";
}


void demoDouble() {
    separator("Double coefficients  (x, y, z)");

    // p = 1.5*x^2 + 2.0*y*z
    Polynomial<double> p({ "x","y","z" });
    p.addTerm(1.5, { {"x",2} });
    p.addTerm(2.0, { {"y",1},{"z",1} });
    std::cout << "p = " << p << "\n";

    auto sq = p * p;
    std::cout << "p^2 = " << sq << "\n";

    double val = p.evaluate({ {"x",1.0}, {"y",2.0}, {"z",3.0} });
    std::cout << "p(1,2,3) = " << val
        << "  (expected: 1.5 + 12.0 = 13.5)\n";

    std::cout << "homogeneousDegree(p)  = " << p.homogeneousDegree()
        << "  (expected: 2 — both monomials have total degree 2)\n";

    auto h2 = p.homogeneousPart(2);
    std::cout << "homogeneousPart(p,2) = " << h2 << "\n";

    Polynomial<double> zero({ "x","y","z" });
    std::cout << "isZero(zero) = " << (zero.isZero() ? "true" : "false") << "\n";
    std::cout << "isZero(p) = " << (p.isZero() ? "true" : "false") << "\n";

    auto diff = p - p;
    std::cout << "p - p isZero? = " << (diff.isZero() ? "true" : "false") << "\n";
}


void demoComplex() {
    separator("Complex<double> coefficients  (z)");

    using C = std::complex<double>;
    using namespace std::complex_literals;

    // pc = (1+2i)*z^2 + (3-i)*z
    Polynomial<C> pc({ "z" });
    pc.addTerm(1.0 + 2.0i, { {"z",2} });
    pc.addTerm(3.0 - 1.0i, { {"z",1} });
    std::cout << "pc = " << pc << "\n";

    C valc = pc.evaluate({ {"z", 1.0i} });
    std::cout << "pc(i) = " << valc
        << "  (expected: (0,1) = i)\n";

    Polynomial<C> qc({ "z" });
    qc.addTerm(C(-1.0, -2.0), { {"z",2} }); // -(1+2i)*z^2
    auto sumC = pc + qc;
    std::cout << "pc + qc = " << sumC
        << "  (expected: (3-i)*z only)\n";


    auto prodC = pc * qc;
    std::cout << "pc * qc = " << prodC << "\n";


    Polynomial<C> r({ "u","v" });
    r.addTerm(1.0 + 0.0i, { {"u",2} });
    r.addTerm(0.0 + 1.0i, { {"v",2} });
    r.addTerm(2.0 + 0.0i, { {"u",1},{"v",1} });
    std::cout << "r = " << r << "\n";
    std::cout << "r degree = " << r.homogeneousDegree() << "\n";
}



int main() 
{
    try {
        demoInt();
        demoDouble();
        demoComplex();
    }
    catch (const std::exception& ex) {
        std::cerr << "Unexpected exception: " << ex.what() << "\n";
        return 1;
    }
    std::cout << "\nAll demos completed successfully.\n";
    return 0;
}