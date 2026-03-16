#include <iostream>
#include <vector>
#include <iomanip>
#include <functional>
#include <stdexcept>
#include <string>

std::vector<double> invert_series(
    std::size_t n,
    const std::function<double(std::size_t)>& a_coeff)
{
    if (n == 0)
        return {};

    const double a0 = a_coeff(0);
    if (a0 == 0.0)
        throw std::runtime_error("a_0 = 0: series not invertible");

    const double inv_a0 = 1.0 / a0;

    std::vector<double> b;
    b.reserve(n);

    b.push_back(inv_a0);

    for (std::size_t i = 1; i < n; ++i) {
        double sum = 0.0;
        for (std::size_t k = 1; k <= i; ++k) {
            sum += a_coeff(k) * b[i - k];
        }
        b.push_back(-inv_a0 * sum);
    }

    return b;
}

void print_coefficients(const std::string& title,
    const std::vector<double>& coeffs)
{
    std::cout << "\n" << title << "\n";
    for (std::size_t i = 0; i < coeffs.size(); ++i) {
        std::cout << "  b" << i << " = "
            << std::setprecision(15) << coeffs[i] << "\n";
    }
}

std::vector<double> make_exp_coeffs(std::size_t n)
{
    std::vector<double> a;
    a.reserve(n);
    double ak = 1.0;
    for (std::size_t k = 0; k < n; ++k) {
        a.push_back(ak);
        ak /= static_cast<double>(k + 1);
    }
    return a;
}

int main()
{
    std::size_t n = 0;
    std::cout << "Enter n (number of inverse series coefficients): ";
    std::cin >> n;
    if (n == 0) {
        std::cerr << "n must be >= 1\n";
        return 1;
    }

    {
        const std::vector<double> exp_a = make_exp_coeffs(n);

        auto exp_coeff = [&exp_a](std::size_t k) -> double {
            return (k < exp_a.size()) ? exp_a[k] : 0.0;
            };

        const auto b = invert_series(n, exp_coeff);
        print_coefficients("Inverse series for f(x) = exp(x):", b);
    }

    {
        auto poly_coeff = [](std::size_t k) -> double {
            switch (k) {
            case 0: return -1.0;
            case 1: return -1.0;
            case 2: return  1.0;
            default: return 0.0;
            }
            };

        const auto b = invert_series(n, poly_coeff);
        print_coefficients("Inverse series for f(x) = x^2 - x - 1:", b);
        std::cout << "  (coefficients related to Fibonacci numbers)\n";
    }

    return 0;
}
