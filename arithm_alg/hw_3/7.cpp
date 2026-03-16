#define _USE_MATH_DEFINES

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>


static constexpr double PI = M_PI;

auto F = [](double x) -> double {
    const double u = x - 1.0;
    const double s = std::sin(PI * x);
    const double c = std::cos(2 * PI * x) - 1.0;
    return u * u * u * s * c;
    };

auto dF = [](double x) -> double {
    const double u = (x - 1.0) * (x - 1.0) * (x - 1.0);
    const double du = 3.0 * (x - 1.0) * (x - 1.0);
    const double v = std::sin(PI * x);
    const double dv = PI * std::cos(PI * x); 
    const double w = std::cos(2.0 * PI * x) - 1.0;    // cos(2πx)-1
    const double dw = -2.0 * PI * std::sin(2.0 * PI * x);// -2π·sin(2πx)

    return du * v * w + u * dv * w + u * v * dw;
    };


struct NewtonResult {
    double       root;
    int          iterations;
    bool         converged;
    std::string  note;
};

NewtonResult newton(
    const std::function<double(double)>& f,
    const std::function<double(double)>& df,
    double x0,
    double eps = 1e-12,
    int    max_iter = 10000,
    int    mult = 1) 
{
    NewtonResult res{ x0, 0, false, "" };

    double x = x0;
    for (int i = 0; i < max_iter; ++i) {
        const double fx = f(x);
        const double dfx = df(x);

        if (std::abs(dfx) < 1e-30) 
        {
            res.root = x;
            res.iterations = i;
            res.note = "derivative near zero, iterations stopped";
            return res;
        }

        // модифицированный шаг Ньютона: умножаем на кратность корня
        // обычный шаг: x -= f/f'
        // модифицированный: x -= mult * f/f'  (ускоряет сходимость при кратных корнях)
        const double step = mult * fx / dfx;
        const double x_new = x - step;

        ++res.iterations;


        if (std::abs(fx) < eps || std::abs(step) < eps) {
            res.root = x_new;
            res.converged = true;
            return res;
        }

        x = x_new;
    }

    // Достигнут лимит итераций
    res.root = x;
    res.note = "max iterations reached (" + std::to_string(max_iter) + ")";
    return res;
}

void printResult(const std::string& label, const NewtonResult& r) {
    std::cout << std::left << std::setw(30) << label;
    std::cout << std::right << std::fixed << std::setprecision(15);
    std::cout << "  root = " << std::setw(20) << r.root;
    std::cout << "  |F(x)| = " << std::scientific << std::setprecision(3)
        << std::abs(F(r.root));
    std::cout << "  iter. = " << std::setw(5) << r.iterations;
    if (!r.note.empty())
        std::cout << "  [" << r.note << "]";
    std::cout << "\n";
}

int main() {
    std::cout << std::string(100, '=') << "\n";
    std::cout << "  NEWTON'S METHOD: roots of F(x) = (x-1)^3 * sin(pi*x) * (cos(2*pi*x) - 1)\n";
    std::cout << std::string(100, '=') << "\n\n";

    constexpr double EPS = 1e-12;

    std::cout << "--- Part 1: Standard Newton method (mult=1) ---\n";
    std::cout << "(Linear convergence for multiple roots: q = 1 - 1/p)\n\n";

    auto r1 = newton(F, dF, 1.2, EPS, 10000, 1);
    printResult("x0=1.2 -> root x=1 (p=3)", r1);

    auto r2 = newton(F, dF, 2.1, EPS, 10000, 1);
    printResult("x0=2.1 -> root x=2 (p>=2)", r2);

    auto r3 = newton(F, dF, 3.1, EPS, 10000, 1);
    printResult("x0=3.1 -> root x=3 (p>=2)", r3);

    std::cout << "\n--- Part 2: Modified Newton method (mult=p) ---\n";
    std::cout << "(Restores quadratic convergence when multiplicity is known)\n\n";

    auto r1m = newton(F, dF, 1.2, EPS, 10000, 3);
    printResult("x0=1.2 -> x=1, mult=3", r1m);

    auto r2m = newton(F, dF, 2.1, EPS, 10000, 2);
    printResult("x0=2.1 -> x=2, mult=2", r2m);

    auto r3m = newton(F, dF, 3.1, EPS, 10000, 2);
    printResult("x0=3.1 -> x=3, mult=2", r3m);

    std::cout << "\n" << std::string(100, '=') << "\n";
    std::cout << "  SUMMARY: found roots\n";
    std::cout << std::string(100, '=') << "\n";

    std::vector<std::pair<std::string, NewtonResult>> results = {
        {"x=1 (p=3, standard)", r1},
        {"x=2 (standard)",      r2},
        {"x=3 (standard)",      r3},
        {"x=1 (p=3, mult=3)",   r1m},
        {"x=2 (mult=2)",        r2m},
        {"x=3 (mult=2)",        r3m},
    };

    std::cout << std::left << std::setw(25) << "Root"
        << std::setw(22) << "Value"
        << std::setw(15) << "|F(root)|"
        << std::setw(10) << "Iterations"
        << "Converged?\n";
    std::cout << std::string(80, '-') << "\n";
    for (auto& [label, r] : results) {
        std::cout << std::left << std::setw(25) << label
            << std::fixed << std::setprecision(12) << std::setw(22) << r.root
            << std::scientific << std::setprecision(2) << std::setw(15) << std::abs(F(r.root))
            << std::setw(10) << r.iterations
            << (r.converged ? "Yes" : "No") << "\n";
    }

    return 0;
}
