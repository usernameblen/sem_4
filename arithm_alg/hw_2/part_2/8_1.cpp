#include <iostream>
#include <utility>
#include<cmath>

std::pair<double, double> complex_division(double a0, double a1, double b0, double b1) 
{
    double p = a0 * b0;                       // 1 умножение
    double q = a1 * b1;                       // 2 умножение
    double t = (a0 + a1) * (b0 - b1);          // 3 умножение

    double real_num = p + q;
    double imag_num = t - p + q;

    double denom = b0 * b0 + b1 * b1;           // 4 и 5 умножения

    return { real_num / denom, imag_num / denom }; // деление
}

int main() {
    double a0 = 3.0, a1 = 4.0;
    double b0 = 1.0, b1 = 2.0;

    double re = complex_division(a0, a1, b0, b1).first;
    double im = complex_division(a0, a1, b0, b1).second;

    std::cout << "(" << a0 << " + " << a1 << "i) / (" << b0 << " + " << b1 << "i) = "
        << re << ((im > 0) ? " + " : " - ") << std::fabs(im) << "i\n";

    return 0;
}