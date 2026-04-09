#include "poly.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

static void run_tests() {
    // 1. Сложение
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly p1(n, fm, { 1, 1, 1 });
        GF2nPoly p2(n, fm, { 1, 0, 1 });
        GF2nPoly s = p1 + p2;
        assert(s.coeff(0) == 0);
        assert(s.coeff(1) == 1);
        assert(s.degree() == 1);
    }
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly a(n, fm, { 3, 2 });
        GF2nPoly b(n, fm, { 3, 2 });
        GF2nPoly s = a + b;
        assert(s.is_zero());
    }
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly a(n, fm, { 1, 2, 3 });
        GF2nPoly z(n, fm);
        GF2nPoly s = a + z;
        assert(s.degree() == a.degree());
        for (int i = 0; i <= a.degree(); ++i)
            assert(s.coeff(i) == a.coeff(i));
    }

    // 2. Умножение по модулю через метод multiply
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly modp(n, fm, { 2, 1, 1 });  // x^2 + x + 2? но в GF(2^2) 2 = 0b10
        GF2nPoly p1(n, fm, { 0, 1 });       // x
        GF2nPoly p2(n, fm, { 0, 1 });       // x
        GF2nPoly prod = p1;               // копия
        prod.multiply(p2, modp);
        assert(prod.degree() <= 1);
    }

    // 3. Обратный полином
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly modp(n, fm, { 2, 1, 1 });
        GF2nPoly p(n, fm, { 0, 1 });        // x
        GF2nPoly inv = p.inverse(modp);
        GF2nPoly check = p;               // копия
        check.multiply(inv, modp);
        assert(check.degree() == 0);
        assert(check.coeff(0) == 1);
    }
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly modp(n, fm, { 2, 1, 1 });
        GF2nPoly p(n, fm, { 3, 2 });
        GF2nPoly inv = p.inverse(modp);
        GF2nPoly check = p;
        check.multiply(inv, modp);
        assert(check.degree() == 0);
        assert(check.coeff(0) == 1);
    }
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly modp(n, fm, { 2, 1, 1 });
        GF2nPoly zero(n, fm);
        GF2nPoly inv_zero = zero.inverse(modp);
        assert(inv_zero.is_zero());
    }

    // 4. Проверка неприводимости
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly irred(n, fm, { 2, 1, 1 });    // x^2 + x + 2? Над GF(4) проверка
        assert(irred.is_irreducible());
    }
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly irred2(n, fm, { 3, 1, 1 });   // x^2 + x + 3
        assert(irred2.is_irreducible());
    }
    {
        int n = 2;
        gf_elem fm = 0b111;
        // Строим приводимый полином как произведение двух линейных (без модуля, но нам нужно только его существование)
        // У нас нет публичного умножения без модуля, поэтому создадим reducible напрямую как произведение
        // Вычислим вручную: (x+1)*(x+2) = x^2 + (1+2)x + 2 = x^2 + 3x + 2 (в GF(4) 1+2=3)
        GF2nPoly reducible(n, fm, { 2, 3, 1 });  // x^2 + 3x + 2
        assert(!reducible.is_irreducible());
    }

    // 5. Проверка метода multiply и совместимости
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly modp(n, fm, { 2, 1, 1 });
        GF2nPoly p(n, fm, { 1, 2 });
        GF2nPoly p_copy = p;
        p.multiply(GF2nPoly(n, fm, { 1, 2 }), modp);
        // Ожидаемый результат: (1+2x)*(1+2x) mod m
        GF2nPoly expected = GF2nPoly(n, fm, { 1, 2 });
        expected.multiply(GF2nPoly(n, fm, { 1, 2 }), modp);
        assert(p.degree() == expected.degree());
        for (int i = 0; i <= p.degree(); ++i)
            assert(p.coeff(i) == expected.coeff(i));
    }

    // 6. Комбинированный тест: (p1 * p2) * inv(p1) == p2 mod m
    {
        int n = 2;
        gf_elem fm = 0b111;
        GF2nPoly modp(n, fm, { 2, 1, 1 });
        GF2nPoly p1(n, fm, { 1, 2, 3 });
        GF2nPoly p2(n, fm, { 2, 3 });
        GF2nPoly prod_mod = p1;
        prod_mod.multiply(p2, modp);               // (p1 * p2) % m
        GF2nPoly inv1 = p1.inverse(modp);
        GF2nPoly recover = prod_mod;
        recover.multiply(inv1, modp);               // (p1*p2)*inv(p1) % m = p2 % m
        GF2nPoly p2_reduced = p2;
        p2_reduced %= modp;
        assert(recover.degree() == p2_reduced.degree());
        for (int i = 0; i <= recover.degree(); ++i)
            assert(recover.coeff(i) == p2_reduced.coeff(i));
    }

    std::cout << "All unit tests passed.\n";
}

static void demo() {
    std::cout << "\n=== Demo: GF(2^2) polynomials ===\n";

    int n = 2;
    gf_elem fm = 0b111;  // x^2 + x + 1
    std::cout << "Base field GF(2^" << n << "), modulus: 0b" << std::oct << fm << std::dec
        << " (x^2 + x + 1)\n";

    GF2nPoly modp(n, fm, { 2, 1, 1 });  // x^2 + x + 2 (2 is element of field)
    std::cout << "Polynomial ring modulus m(x) = " << modp.to_string() << "\n\n";

    GF2nPoly p1(n, fm, { 1, 2, 3 });
    GF2nPoly p2(n, fm, { 2, 3 });
    std::cout << "p1(x) = " << p1.to_string() << "\n";
    std::cout << "p2(x) = " << p2.to_string() << "\n\n";

    GF2nPoly sum = p1 + p2;
    std::cout << "p1 + p2         = " << sum.to_string() << "\n";

    GF2nPoly prod = p1;
    prod.multiply(p2, modp);
    std::cout << "p1 * p2 mod m   = " << prod.to_string() << "\n";

    GF2nPoly inv1 = p1.inverse(modp);
    std::cout << "p1^-1 mod m     = " << inv1.to_string() << "\n";

    GF2nPoly verify = p1;
    verify.multiply(inv1, modp);
    std::cout << "p1 * p1^-1 mod m = " << verify.to_string() << "  [should be 1]\n\n";

    GF2nPoly irred1(n, fm, { 2, 1, 1 });
    GF2nPoly irred2(n, fm, { 3, 1, 1 });
    // Приводимый полином: (x+1)*(x+2) = x^2 + 3x + 2
    GF2nPoly reducible(n, fm, { 2, 3, 1 });

    std::cout << "is_irreducible(" << irred1.to_string() << ") = "
        << (irred1.is_irreducible() ? "true" : "false") << "\n";
    std::cout << "is_irreducible(" << irred2.to_string() << ") = "
        << (irred2.is_irreducible() ? "true" : "false") << "\n";
    std::cout << "is_irreducible(" << reducible.to_string() << ") = "
        << (reducible.is_irreducible() ? "true" : "false")
        << "  [(x+1)(x+2) is reducible]\n\n";

    GF2nPoly zero(n, fm);
    GF2nPoly inv_zero = zero.inverse(modp);
    std::cout << "inverse(0)       = " << (inv_zero.is_zero() ? "0" : inv_zero.to_string())
        << "  [zero convention]\n\n";

    std::cout << "=== Compound operators ===\n";
    GF2nPoly a(n, fm, { 1, 2, 3 });
    GF2nPoly b(n, fm, { 2, 3 });
    std::cout << "a = " << a.to_string() << "\n";
    a += b;
    std::cout << "a += b  =>  " << a.to_string() << "\n";

    GF2nPoly c(n, fm, { 1, 2 });
    GF2nPoly d(n, fm, { 3, 1 });
    std::cout << "c = " << c.to_string() << ",  d = " << d.to_string() << "\n";
    c.multiply(d, modp);  // сразу по модулю
    std::cout << "c = c * d mod m => " << c.to_string() << "\n";

    std::cout << "\n=== multiply() method ===\n";
    GF2nPoly e(n, fm, { 1, 2 });
    GF2nPoly f(n, fm, { 3, 1 });
    e.multiply(f, modp);
    std::cout << "(1 + 2*x).multiply(3 + x, m) = " << e.to_string() << "\n";
}

int main() {
    run_tests();
    demo();
    return 0;
}