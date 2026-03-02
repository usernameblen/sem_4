//3. Напишите функцию умножения двух элементов из GF(2^n).
// При выводе продемонстрируйте результаты для различных 
// неприводимых многочленов, с помощью которых определяется операция умножения.

#include <iostream>
#include "..\utils\utils.h"

int main() 
{
    // ----- Example 1: GF(2^4) with modulus x^4 + x + 1 (0b10011) -----
    int n = 4;
    uint64_t mod = 0b10011; // x^4 + x + 1
    uint64_t a = 0b0101;    // x^2 + 1
    uint64_t b = 0b0011;    // x + 1
    uint64_t r = gf_multiply(a, b, mod, n);
    std::cout << "1) GF(2^4) with modulus " << poly_to_string(mod) << "\n";
    std::cout << "  (" << poly_to_string(a) << ") * (" << poly_to_string(b)
        << ") = " << poly_to_string(r) << "\n\n";

    // ----- Example 2: Another multiplication in same field -----
    a = 0b1011; // x^3 + x + 1
    b = 0b0111; // x^2 + x + 1
    r = gf_multiply(a, b, mod, n);
    std::cout << "2) Same field: (" << poly_to_string(a) << ") * (" << poly_to_string(b) << ") = "
        << poly_to_string(r) << "\n\n";


    // ----- Example 4: Same elements, different moduli (GF(2^4)) -----
    uint64_t x = 0b0111; // x^2 + x + 1
    uint64_t y = 0b0101; // x^2 + 1
    uint64_t mod1 = 0b10011; // x^4 + x + 1
    uint64_t mod2 = 0b11001; // x^4 + x^3 + 1
    uint64_t r1 = gf_multiply(x, y, mod1, 4);
    uint64_t r2 = gf_multiply(x, y, mod2, 4);
    std::cout << "4) Dependence on modulus (GF(2^4)):\n";
    std::cout << "   With modulus " << poly_to_string(mod1) << ": ("
        << poly_to_string(x) << ")*(" << poly_to_string(y) << ") = " << poly_to_string(r1) << "\n";
    std::cout << "   With modulus " << poly_to_string(mod2) << ": ("
        << poly_to_string(x) << ")*(" << poly_to_string(y) << ") = " << poly_to_string(r2) << "\n\n";

    // ----- Example 5: Multiplication by 0 and 1 -----
    n = 4;
    mod = 0b10011;
    a = 0b1011; // x^3 + x + 1
    uint64_t zero = 0;
    uint64_t one = 1;
    std::cout << "5) Multiplication by 0 and 1:\n";
    std::cout << "   (" << poly_to_string(a) << ") * 0 = " << poly_to_string(gf_multiply(a, zero, mod, n)) << "\n";
    std::cout << "   (" << poly_to_string(a) << ") * 1 = " << poly_to_string(gf_multiply(a, one, mod, n)) << "\n";

    return 0;
}