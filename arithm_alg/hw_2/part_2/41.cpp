#include <iostream>
#include <complex>
#include <iomanip>
#include <cmath>

// стандартное умножение: 4 умножения, 2 сложения
void standard_multiply(double a, double b, double c, double d,
    double& re, double& im) 
{
    re = a * c - b * d;   // 2 умножения, 1 вычитание
    im = a * d + b * c;   // 2 умножения, 1 сложение
}


void fast_multiply(double a, double b, double c, double d,
    double& re, double& im) 
{
    double s = a + b;          // сложение 1: только a и b

    double p1 = a * c;          // умножение 1
    double p2 = b * d;          // умножение 2
    double p3 = s * (c + d);    // умножение 3, сложение 2: (a+b)(c+d)

    re = p1 - p2;               // сложение 3: ac - bd
    im = p3 - p1 - p2;          // сложение 4, 5: (a+b)(c+d) - ac - bd = ad + bc
}

void compare_multiplication(double a, double b, double c, double d) 
{
    double re_fast, im_fast, re_std, im_std;

    fast_multiply(a, b, c, d, re_fast, im_fast);

    standard_multiply(a, b, c, d, re_std, im_std);

    std::cout << "(" << a << (b >= 0 ? "+" : "") << b << "i) * "
        << "(" << c << (d >= 0 ? "+" : "") << d << "i)\n";

    std::cout << "  Fast method (3 multiplications):   "
        << re_fast << (im_fast >= 0 ? "+" : "") << im_fast << "i\n";
    std::cout << "  Standard method (4 multiplications): "
        << re_std << (im_std >= 0 ? "+" : "") << im_std << "i\n";

    const double eps = 1e-9;
    if (std::abs(re_fast - re_std) < eps && std::abs(im_fast - im_std) < eps) {
        std::cout << "  => Results match.\n";
    }
    else {
        std::cout << "  => ERROR: results differ!\n";
    }
    std::cout << std::endl;
}

int main() 
{
    compare_multiplication(3, 2, 1, 4);
    compare_multiplication(1, 1, 1, -1);
    compare_multiplication(0, 1, 0, 1);
    compare_multiplication(2, -3, 4, 5);
    compare_multiplication(1, 0, 5, 7);
    compare_multiplication(-2, 3, -1, -4);

    return 0;
}