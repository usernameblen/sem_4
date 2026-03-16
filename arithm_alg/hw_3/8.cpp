
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

static constexpr double PI = 3.14159265358979323846;

// f(x) = (x-1)^3 * sin(pi*x) * (cos(2*pi*x) - 1)
inline double f(double x) noexcept {
    double u = (x - 1.0) * (x - 1.0) * (x - 1.0);
    double v = std::sin(PI * x);
    double w = std::cos(2.0 * PI * x) - 1.0;
    return u * v * w;
}

inline double df(double x) noexcept {
    double u = (x - 1.0) * (x - 1.0) * (x - 1.0), du = 3.0 * (x - 1.0) * (x - 1.0);
    double v = std::sin(PI * x), dv = PI * std::cos(PI * x);
    double w = std::cos(2.0 * PI * x) - 1.0, dw = -2.0 * PI * std::sin(2.0 * PI * x);
    return du * v * w + u * dv * w + u * v * dw;
}

// обобщённый метод Ньютона: x_{n+1} = x_n + sigma * f(x_n) / f'(x_n)
//  {корень, число итераций}
std::pair<double, int> newton_method(
    const std::function<double(double)>& func,
    const std::function<double(double)>& dfunc,
    double x0, double sigma, double eps, int max_iter = 10000
) {
    double x = x0;
    for (int i = 1; i <= max_iter; ++i) {
        double fx = func(x), dfx = dfunc(x);

        if (std::abs(dfx) < 1e-300) {
            if (std::abs(fx) < eps) return { x, i };
            throw std::runtime_error("f'=0 away from root, x=" + std::to_string(x));
        }
        double xn = x + sigma * fx / dfx;
        double step = std::abs(xn - x);
        x = xn;
        if (step < eps) return { x, i };
    }
    throw std::runtime_error("No convergence after " + std::to_string(max_iter) + " iterations");
}

struct Root { double x0; int p; std::string label; };

int main() 
{
    // начальные приближения:
    //   x=1: x0=0.8 -- оба метода сходятся к x=1
    //   x=2: x0=1.9 -- оба метода сходятся к x=2
    //   x=3: x0=2.9 -- оба метода сходятся к x=3
    const std::vector<Root> roots = {
        {0.8, 6, "x = 1  (p = 6)"},
        {1.9, 3, "x = 2  (p = 3)"},
        {2.9, 3, "x = 3  (p = 3)"},
    };
    const std::vector<int> ns = { 3, 4, 5, 6 };

    constexpr int CW = 20;
    std::string hline(2 + CW * 4, '-');

    std::cout << "\n  Newton's Method: iteration count comparison\n";
    std::cout << "  f(x) = (x-1)^3 * sin(pi*x) * (cos(2*pi*x) - 1)\n\n";
    std::cout << hline << "\n";
    std::cout << std::left
        << "  " << std::setw(CW) << "Root"
        << std::setw(CW) << "eps"
        << std::setw(CW) << "Standard (sigma=-1)"
        << std::setw(CW) << "Modified (sigma=-p)"
        << "\n" << hline << "\n";

    for (const auto& r : roots) 
    {
        double sm = -static_cast<double>(r.p);
        for (int n : ns) 
        {
            double eps = std::pow(10.0, -n);
            auto [r1, i1] = newton_method(f, df, r.x0, -1.0, eps);
            auto [r2, i2] = newton_method(f, df, r.x0, sm, eps);
            std::cout << "  " << std::left
                << std::setw(CW) << r.label
                << std::setw(CW) << ("1e-" + std::to_string(n))
                << std::setw(CW) << i1
                << std::setw(CW) << i2 << "\n";
        }
        std::cout << hline << "\n";
    }

    std::cout << "\n  Root values (eps = 1e-12):\n";
    std::cout << std::fixed << std::setprecision(12);
    for (const auto& r : roots) 
    {
        auto [root, iters] = newton_method(f, df, r.x0, -static_cast<double>(r.p), 1e-12);
        std::cout << "  " << r.label
            << "  ->  x = " << root
            << ",  |f(x)| = " << std::abs(f(root)) << "\n";
    }
    return 0;
}
