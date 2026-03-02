//4. Реализуйте расширенный алгоритм Евклида для GF(2^n).

#include <iostream>
#include "..\utils\utils.h"

int main() 
{
    uint64_t a1 = 0b10100; // x^4 + x^2
    uint64_t b1 = 0b1100;  // x^3 + x^2
    uint64_t gcd1, x1, y1;
    extended_euclidean_gf(a1, b1, gcd1, x1, y1);
    std::cout << "1) a = " << poly_to_string(a1) << "\n";
    std::cout << "   b = " << poly_to_string(b1) << "\n";
    std::cout << "   gcd = " << poly_to_string(gcd1) << "\n";
    std::cout << "   x = " << poly_to_string(x1) << ", y = " << poly_to_string(y1) << "\n";
    uint64_t lhs1 = multiply_polys(x1, a1) ^ multiply_polys(y1, b1);
    std::cout << "   x*a + y*b = " << poly_to_string(lhs1) << "\n\n";

    uint64_t a2 = 0b10011; // x^4 + x + 1
    uint64_t b2 = 0b101;   // x^2 + 1
    uint64_t gcd2, x2, y2;
    extended_euclidean_gf(a2, b2, gcd2, x2, y2);
    std::cout << "2) a = " << poly_to_string(a2) << "\n";
    std::cout << "   b = " << poly_to_string(b2) << "\n";
    std::cout << "   gcd = " << poly_to_string(gcd2) << "\n";
    std::cout << "   x = " << poly_to_string(x2) << ", y = " << poly_to_string(y2) << "\n";
    uint64_t lhs2 = multiply_polys(x2, a2) ^ multiply_polys(y2, b2);
    std::cout << "   x*a + y*b = " << poly_to_string(lhs2) << " (should be 1)\n";
    if (gcd2 == 1)
        std::cout << "   => x is the inverse of a modulo b (if b is irreducible).\n\n";

    uint64_t a3 = 0b0111;  // x^2 + x + 1
    uint64_t b3 = 0b10011; // x^4 + x + 1
    uint64_t gcd3, x3, y3;
    extended_euclidean_gf(a3, b3, gcd3, x3, y3);
    std::cout << "3) a = " << poly_to_string(a3) << "\n";
    std::cout << "   b = " << poly_to_string(b3) << " (irreducible modulus)\n";
    std::cout << "   gcd = " << poly_to_string(gcd3) << "\n";
    std::cout << "   x = " << poly_to_string(x3) << ", y = " << poly_to_string(y3) << "\n";
    uint64_t lhs3 = multiply_polys(x3, a3) ^ multiply_polys(y3, b3);
    std::cout << "   x*a + y*b = " << poly_to_string(lhs3) << "\n";
    if (gcd3 == 1)
        std::cout << "   => x is the multiplicative inverse of a in GF(2^4).\n\n";

    uint64_t a4 = 1;
    uint64_t b4 = 0b1011; // x^3 + x + 1
    uint64_t gcd4, x4, y4;
    extended_euclidean_gf(a4, b4, gcd4, x4, y4);
    std::cout << "4) a = " << poly_to_string(a4) << "\n";
    std::cout << "   b = " << poly_to_string(b4) << "\n";
    std::cout << "   gcd = " << poly_to_string(gcd4) << "\n";
    std::cout << "   x = " << poly_to_string(x4) << ", y = " << poly_to_string(y4) << "\n";
    uint64_t lhs4 = multiply_polys(x4, a4) ^ multiply_polys(y4, b4);
    std::cout << "   x*a + y*b = " << poly_to_string(lhs4) << " (should be 1)\n\n";

    uint64_t a5 = 0b1011; // x^3 + x + 1
    uint64_t b5 = 0b1011;
    uint64_t gcd5, x5, y5;
    extended_euclidean_gf(a5, b5, gcd5, x5, y5);
    std::cout << "5) a = " << poly_to_string(a5) << "\n";
    std::cout << "   b = " << poly_to_string(b5) << " (same polynomial)\n";
    std::cout << "   gcd = " << poly_to_string(gcd5) << "\n";
    std::cout << "   x = " << poly_to_string(x5) << ", y = " << poly_to_string(y5) << "\n";
    uint64_t lhs5 = multiply_polys(x5, a5) ^ multiply_polys(y5, b5);
    std::cout << "   x*a + y*b = " << poly_to_string(lhs5) << " (should equal a)\n\n";

    return 0;
}