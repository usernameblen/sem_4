//5. Напишите функцию, которая ищет мультипликативный 
// обратный для элемента из GF(2^n).

#include <iostream>
#include <stdexcept>
#include "..\utils\utils.h"

int main() 
{
    // ----- Example 1: GF(2^4) with modulus x^4 + x + 1 -----
    int n1 = 4;
    uint64_t mod1 = 0b10011; // x^4 + x + 1
    uint64_t a1 = 0b0011;    // x + 1
    uint64_t inv1 = gf_inverse(a1, mod1, n1);
    uint64_t check1 = gf_multiply(a1, inv1, mod1, n1);
    std::cout << "1) GF(2^4) modulus: " << poly_to_string(mod1) << "\n";
    std::cout << "   a = " << poly_to_string(a1) << "\n";
    std::cout << "   inverse = " << poly_to_string(inv1) << "\n";
    std::cout << "   a * inverse = " << poly_to_string(check1) << (check1 == 1 ? " [OK]" : " [FAIL]") << "\n\n";

    // Another element in same field
    uint64_t a2 = 0b0111; // x^2 + x + 1
    uint64_t inv2 = gf_inverse(a2, mod1, n1);
    uint64_t check2 = gf_multiply(a2, inv2, mod1, n1);
    std::cout << "   a = " << poly_to_string(a2) << "\n";
    std::cout << "   inverse = " << poly_to_string(inv2) << "\n";
    std::cout << "   a * inverse = " << poly_to_string(check2) << (check2 == 1 ? " [OK]" : " [FAIL]") << "\n\n";

    // ----- Example 3: Different modulus for GF(2^8) to show dependence -----
    uint64_t mod3 = 0x11D; // x^8 + x^4 + x^3 + x^2 + 1
    int n2 = 8;
    uint64_t a5 = 0x57;
    uint64_t inv5 = gf_inverse(a5, mod3, n2);
    uint64_t check5 = gf_multiply(a5, inv5, mod3, n2);
    std::cout << "2) GF(2^8) with different modulus: " << poly_to_string(mod3) << "\n";
    std::cout << "   a = 0x" << std::hex << a5 << std::dec << " (" << poly_to_string(a5) << ")\n";
    std::cout << "   inverse = 0x" << std::hex << inv5 << std::dec << " (" << poly_to_string(inv5) << ")\n";
    std::cout << "   a * inverse = 0x" << std::hex << check5 << std::dec << (check5 == 1 ? " [OK]" : " [FAIL]") << "\n\n";

    // ----- example 4: Zero element -----
    std::cout << "3) Zero element:\n";
    try {
        uint64_t inv = gf_inverse(0, mod1, n1);
        std::cout << "   inverse(0) = " << poly_to_string(inv) << " (unexpected)\n";
    }
    catch (const std::exception& e) {
        std::cout << "   Error: " << e.what() << "\n";
    }

    return 0;
}
