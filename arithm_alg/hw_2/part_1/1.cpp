// 1. Напишите функцию, представляющую элемент из GF(2^n) 
// в полиномиальной форме и наоборот.

#include <iostream>
#include <stdexcept>
#include "..\utils\utils.h"

int main() 
{
    try {
        // 1.poly_to_string
        std::cout << "--- poly_to_string ---\n";
        std::cout << "0b1011 (x^3+x+1) -> \"" << poly_to_string(0b1011) << "\"\n";
        std::cout << "0b100011011 (AES-polynomial) -> \"" << poly_to_string(0b100011011) << "\"\n";
        std::cout << "0b10011 (x^4+x+1) -> \"" << poly_to_string(0b10011) << "\"\n";
        std::cout << "0 -> \"" << poly_to_string(0) << "\"\n";
        std::cout << "1 -> \"" << poly_to_string(1) << "\"\n";
        std::cout << "2 (x) -> \"" << poly_to_string(2) << "\"\n";

        // 2.string_to_poly
        std::cout << "\n--- string_to_poly ---\n";
        std::cout << "\"x^3 + x + 1\" -> 0x" << std::hex << string_to_poly("x^3 + x + 1") << std::dec << "\n";
        std::cout << "\"x^8 + x^4 + x^3 + x + 1\" -> 0x" << std::hex << string_to_poly("x^8 + x^4 + x^3 + x + 1") << std::dec << "\n";
        std::cout << "\"x^4 + x + 1\" -> 0x" << std::hex << string_to_poly("x^4 + x + 1") << std::dec << "\n";
        std::cout << "\"0\" -> 0x" << std::hex << string_to_poly("0") << std::dec << "\n";
        std::cout << "\"1\" -> 0x" << std::hex << string_to_poly("1") << std::dec << "\n";
        std::cout << "\"x\" -> 0x" << std::hex << string_to_poly("x") << std::dec << "\n";

        // 3.проверка туда-обратно (round-trip) для нескольких значений
        std::cout << "\n--- Round-trip examination ---\n";
        uint64_t vals[] = { 0b1011, 0b100011011, 0, 1, 2 };
        for (uint64_t v : vals) 
        {
            std::string s = poly_to_string(v);
            uint64_t back = string_to_poly(s);
            std::cout << "0x" << std::hex << v << " -> \"" << s << "\" -> 0x" << back << std::dec
                << (v == back ? " [OK]" : " [ERROR]") << "\n";
        }

        // 4. Проверка некорректного ввода (пример с ошибкой)
        std::cout << "\n--- Checking for incorrect input ---\n";
        try {
            string_to_poly("x^65"); // степень >63
        }
        catch (const std::exception& e) {
            std::cout << "Error for \"x^65\": " << e.what() << "\n";
        }
        try {
            string_to_poly("2x+1");
        }
        catch (const std::exception& e) {
            std::cout << "Error for \"2x+1\": " << e.what() << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}