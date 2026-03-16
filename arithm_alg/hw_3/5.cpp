
  //Исследование сходимости метода простой итерации
  //Уравнения:
  //  a) x_{n+1} = 2^x_n - 1
  //  b) x_{n+1} = e^(2x_n) - 1
  //  c) x_{n+1} = A - ln(x_n)
  //  d) x_{n+1} = alpha * e^(-x_n) + beta * x_n


#include <iostream>
#include <iomanip>
#include <functional>
#include <cmath>
#include <limits>
#include <string>


constexpr double EPS = 1e-10;
constexpr int MAX_ITER = 1000;


struct IterationResult 
{
    bool   converged;     // true — сошёлся, false — расходится
    double finalValue;
    int    iterations;
    bool   domainError;   // true — ошибка области определения
};


IterationResult runIteration(
    const std::function<double(double)>& phi,
    double x0)
{
    IterationResult res{ false, x0, 0, false };

    double xCur = x0;
    std::cout << std::fixed << std::setprecision(10);

    auto printRow = [&](int n, double x) {
        std::cout << "  n=" << std::setw(4) << n << ":  x = " << x << "\n";
        };

    printRow(0, xCur);

    for (int n = 1; n <= MAX_ITER; ++n) {
        double xNext;

        // вычисляем phi(xCur) и сбрасываем errno перед вычислением.
        errno = 0;
        xNext = phi(xCur);

        // проверка на ошибку области определения (NaN после math-функции)
        if (std::isnan(xNext)) {
            std::cout << "\n  [!] Error: value out of domain (NaN) at iteration " << n << ".\n";
            res.domainError = true;
            res.iterations = n;
            res.finalValue = xCur;
            return res;
        }


        if (std::isinf(xNext)) {
            std::cout << "\n  [!] Value went to infinity at iteration " << n << ".\n";
            res.iterations = n;
            res.finalValue = xNext;
            return res;
        }

        printRow(n, xNext);

        // Условие сходимости
        if (std::abs(xNext - xCur) < EPS) {
            res.converged = true;
            res.finalValue = xNext;
            res.iterations = n;
            return res;
        }

        xCur = xNext;
        res.iterations = n;
        res.finalValue = xCur;
    }

    // Достигнут лимит итераций
    return res;
}


double readDouble(const std::string& prompt) 
{
    double v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) return v;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [!] Invalid input. Try again.\n";
    }
}

int readInt(const std::string& prompt) 
{
    int v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) return v;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [!] Invalid input. Try again.\n";
    }
}


double numericalDerivative(const std::function<double(double)>& phi, double x) {
    const double h = 1e-7;
    return (phi(x + h) - phi(x - h)) / (2.0 * h);
}


void runExperiment(const std::function<double(double)>& phi, const std::string& eqName) 
{
    std::cout << "\n--- Equation: " << eqName << " ---\n";

    double x0 = readDouble("Enter initial approximation x0: ");

    std::cout << "\nIterations (eps = " << std::scientific << EPS << ", max_iter = " << MAX_ITER << "):\n";
    IterationResult res = runIteration(phi, x0);


    std::cout << "\n--- Result ---\n";

    if (res.domainError) {
        std::cout << "Process terminated due to domain error.\n"
            << "Try different initial approximation.\n";
    }
    else if (res.converged) {
        std::cout << std::fixed << std::setprecision(10);
        std::cout << " Converged to x* = " << res.finalValue
            << " in " << res.iterations << " iterations.\n";

        // Производная в неподвижной точке
        double deriv = numericalDerivative(phi, res.finalValue);
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "phi'(x*) ≈ " << deriv
            << "  (condition |phi'|<1: " << (std::abs(deriv) < 1.0 ? "satisfied" : "NOT satisfied") << ")\n";
    }
    else {
        std::cout << " Diverged: reached limit of " << MAX_ITER << " iterations.\n"
            << "Last value: " << res.finalValue << "\n";
    }
}


int main() 
{
        std::cout << "eps = " << std::scientific << EPS << ", max_iter = " << MAX_ITER << "\n"
        << "All iterations will be shown until convergence or limit reached.\n";

    while (true) {
        std::cout << "\nSelect equation:\n"
            << "  1) x_{n+1} = 2^x_n - 1\n"
            << "  2) x_{n+1} = e^(2*x_n) - 1\n"
            << "  3) x_{n+1} = A - ln(x_n)       [parameter A]\n"
            << "  4) x_{n+1} = a*e^(-x_n) + b*x_n [parameters a, b]\n"
            << "  0) Exit\n"
            << "Your choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  [!] Invalid input.\n";
            continue;
        }

        if (choice == 0) 
            break;
       

        // ── Уравнение a) ─────────────────────────────────────
        if (choice == 1) {
           // phi(x) = 2^(x-1)
           // Неподвижная точка: x* = 2^(x*-1) => x* = 1 и x* = 2
           // phi'(x) = ln(2) * 2^(x-1), при x*=1: phi'(1) = ln2 ≈ 0.693 < 1 => сходимость к 1.
            auto phi = [](double x) -> double {
                return std::pow(2.0, x - 1.0);
                };
            runExperiment(phi, "x_{n+1} = 2^(x_n - 1)");
        }

        // ── Уравнение b) ─────────────────────────────────────
        else if (choice == 2) {
            // phi(x) = e^(2x) - 1
            // Неподвижная точка: x* = 0
            // phi'(x) = 2*e^(2x), при x*=0: phi'(0) = 2 > 1 => расходится
            auto phi = [](double x) -> double {
                return std::exp(2.0 * x) - 1.0;
                };
            runExperiment(phi, "x_{n+1} = e^(2*x_n) - 1");
        }

        // ── Уравнение c) ─────────────────────────────────────
        else if (choice == 3) {
            double A = readDouble("Enter parameter A: ");

            // phi(x) = A - ln(x), x > 0
            // Неподвижная точка: x* = A - ln(x*) => решение зависит от A
            // phi'(x) = -1/x; при x*: |phi'(x*)| = 1/x* < 1 => x* > 1
            auto phi = [A](double x) -> double {
                if (x <= 0.0) 
                    return std::numeric_limits<double>::quiet_NaN();
                
                return A - std::log(x);
                };
            runExperiment(phi, "x_{n+1} = A - ln(x_n), A=" + std::to_string(A));
        }

        // ── Уравнение d) ─────────────────────────────────────
        else if (choice == 4) {
            double alpha = readDouble("Enter parameter a: ");
            double beta = readDouble("Enter parameter b: ");

            // phi(x) = a * e^(-x) + b * x
            // phi'(x) = -a * e^(-x) + b
            // Сходимость зависит от выбора a, b и начального приближения
            auto phi = [alpha, beta](double x) -> double {
                double ex = std::exp(-x);
                if (std::isinf(ex)) {
                    return std::numeric_limits<double>::quiet_NaN();
                }
                return alpha * ex + beta * x;
                };
            runExperiment(phi, "x_{n+1} = a*e^(-x_n)+b*x_n, a=" +
                std::to_string(alpha) + ", b=" + std::to_string(beta));
        }

        else {
            std::cout << "  [!] Invalid choice. Try again.\n";
        }
    }

    return 0;
}