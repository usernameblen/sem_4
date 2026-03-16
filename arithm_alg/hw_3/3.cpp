#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <functional>
#include <string>

struct SolveResult 
{
    double root;
    double residual; //значение функции 
    int iterations;
    std::string status;          // "converged", "max_iter", "diverged"
    std::string psi_description;
};


struct TestCase 
{
    double x0;
    std::function<double(double)> psi;
    std::function<double(double)> f;
    std::string description; //текстовое описание
};

SolveResult iterative_solve(std::function<double(double)> psi, //параметры в (), возвр. тип в <>
    std::function<double(double)> f,
    double x0,
    double eps = 1e-8,
    int max_iter = 1000) 
{
    double x = x0;
    double x_new = x0;
    SolveResult result{};

    for (int iter = 0; iter < max_iter; ++iter) 
    {
        x_new = psi(x);

        double delta = std::abs(x_new - x);
        double res = std::abs(f(x_new));

        if (delta < eps && res < eps)  //условия остановки
        {
            result.root = x_new;
            result.residual = res;
            result.iterations = iter + 1;
            result.status = "converged";
            return result;
        }

        x = x_new;
    }

    result.root = x;
    result.residual = std::abs(f(x));
    result.iterations = max_iter;
    result.status = "max_iter";
    return result;
}


void process_equation(const std::vector<TestCase>& tests,
    const std::string& equation_name,
    double eps = 1e-8,
    int max_iter = 1000) 
{
    std::cout << equation_name << "\n";
    for (const auto& test : tests) 
    {
        SolveResult res = iterative_solve(test.psi, test.f, test.x0, eps, max_iter);
        res.psi_description = test.description;

        std::cout << "Initial x0 = " << test.x0 << ", " << test.description << ":\n";
        std::cout << "  Root: " << res.root
            << ", residual: " << res.residual
            << ", iterations: " << res.iterations
            << ", status: " << res.status << "\n";
    }
    std::cout << "\n";
}

// a) x^3 + 3x^2 - 1 = 0
namespace EqA 
{
    double f(double x) 
    {
        return x * x * x + 3.0 * x * x - 1.0;
    }

    double psi_left(double x)
    {
        return x - 0.2 * f(x);
    }

    double psi_mid(double x) 
    {
        return x + 0.5 * f(x);
    }

    double psi_right(double x) {
        return x - 0.2 * f(x);
    }
}

// b) x^4 - x^3 = 0 = x^3 (x - 1)
namespace EqB 
{
    double f(double x) 
    {
        return x * x * x * x - x * x * x;
    }

    double psi0(double x)
    {
        return x * x;
    }

    double psi1(double x)
    {
        return x - f(x); 
    }
}

// c) x^2 - 3x + 2 = 0, roots 1 and 2
namespace EqC 
{
    double f(double x) 
    {
        return x * x - 3.0 * x + 2.0;
    }

    double psi_root1(double x)
    {
        return x * x - 2.0 * x + 2.0;
    }

    double psi_root2(double x)
    {
        return -x * x + 4.0 * x - 2.0;
    }
}

int main() {
    std::cout << std::fixed << std::setprecision(10);

    std::vector<TestCase> tests_a = 
    {
        { -2.5, EqA::psi_left,  EqA::f,
          "psi_left  = x - 0.2*f(x)      (left root = -2.879)"   },
        { -0.7, EqA::psi_mid,   EqA::f,
          "psi_mid   = x + 0.5*f(x)      (middle root = -0.653)" },
        {  0.5, EqA::psi_right, EqA::f,
          "psi_right = x - 0.2*f(x)      (right root = 0.532)"   }
    };
    process_equation(tests_a, "Equation (a): x^3 + 3x^2 - 1 = 0");

    std::vector<TestCase> tests_b = 
    {
        { 0.5,  EqB::psi0, EqB::f,
          "psi0 = x^2                     for root x = 0 (multiple)" },
        { 1.2,  EqB::psi1, EqB::f,
          "psi1 = x - f(x)                 for root x = 1" }
    };
    process_equation(tests_b, "Equation (b): x^4 - x^3 = 0", 1e-8, 2000);


    std::vector<TestCase> tests_c = 
    {
        { 1.2, EqC::psi_root1, EqC::f,
          "psi_root1 = x^2 - 2x + 2       for root x = 1" },
        { 2.3, EqC::psi_root2, EqC::f,
          "psi_root2 = -x^2 + 4x - 2      for root x = 2" }
    };
    process_equation(tests_c, "Equation (c): x^2 - 3x + 2 = 0");

    return 0;
}