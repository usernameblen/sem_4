//2. Напишите функцию, умножающую два произвольных 
// двоичных многочлена степени не выше 32.

#include <iostream>
#include <stdexcept>
#include "..\utils\utils.h"

int main() 
{
    // Example 1: (x^3 + x + 1) * (x^2 + 1)
    uint64_t a1 = 0b1011;      // x^3 + x + 1
    uint64_t b1 = 0b101;       // x^2 + 1
    uint64_t r1 = multiply_polys(a1, b1);
    std::cout << "1) a = " << poly_to_string(a1) << "\n";
    std::cout << "   b = " << poly_to_string(b1) << "\n";
    std::cout << "   a*b = " << poly_to_string(r1) << "\n";
    std::cout << "   (expected: x^5 + x^2 + x + 1)\n\n";

    // Example 2: (x^5 + x^2) * (x^4 + x^3 + 1)
    uint64_t a2 = 0b100100;    // x^5 + x^2
    uint64_t b2 = 0b11001;     // x^4 + x^3 + 1
    uint64_t r2 = multiply_polys(a2, b2);
    std::cout << "2) a = " << poly_to_string(a2) << "\n";
    std::cout << "   b = " << poly_to_string(b2) << "\n";
    std::cout << "   a*b = " << poly_to_string(r2) << "\n";
    std::cout << "   (expected: x^9 + x^8 + x^6 + x^2)\n\n";

    // Example 3: (x + 1) * (x + 1) = x^2 + 1
    uint64_t a3 = 0b11;        // x+1
    uint64_t b3 = 0b11;
    uint64_t r3 = multiply_polys(a3, b3);
    std::cout << "3) a = " << poly_to_string(a3) << "\n";
    std::cout << "   b = " << poly_to_string(b3) << "\n";
    std::cout << "   (x+1)^2 = " << poly_to_string(r3) << "\n";
    std::cout << "   (expected: x^2 + 1)\n\n";

    // Example 4: Multiplication by 0
    uint64_t a4 = 0b1011;
    uint64_t b4 = 0;
    uint64_t r4 = multiply_polys(a4, b4);
    std::cout << "4) a = " << poly_to_string(a4) << "\n";
    std::cout << "   b = 0\n";
    std::cout << "   a*0 = " << poly_to_string(r4) << "\n\n";

    // Example 5: Multiplication by 1
    uint64_t a5 = 0b1011;
    uint64_t b5 = 1;
    uint64_t r5 = multiply_polys(a5, b5);
    std::cout << "5) a = " << poly_to_string(a5) << "\n";
    std::cout << "   b = 1\n";
    std::cout << "   a*1 = " << poly_to_string(r5) << "\n\n";

    // Example 6: Large polynomials (degree ~31 and 1)
    uint64_t a6 = 0xFFFFFFFFULL; // polynomial x^31+...+1
    uint64_t b6 = 3;             // x+1
    uint64_t r6 = multiply_polys(a6, b6);
    std::cout << "6) a = polynomial of degree 31 (all ones)\n";
    std::cout << "   b = x+1\n";
    std::cout << "   a*b = " << poly_to_string(r6) << "\n";
    std::cout << "   (result degree should be 32)\n";
    std::cout << "   (expected: x^32 + 1)\n\n";

    return 0;
}