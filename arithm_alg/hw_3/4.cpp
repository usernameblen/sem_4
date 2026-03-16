#include <iostream>
#include <cmath>
#include <limits>
#include <iomanip>
#include <string>

//для а:
//0.3
//0.8
//1
//5 - сходится

//для b:
//0.5
//2
//0
//1 - сходится

static const int    MAX_ITER = 1000;
static const double TOLERANCE = 1e-10;

double phi_a(double x, double a, double b, double c)
{
    double s = std::sin(x);
    double cs = std::cos(x);
    return c + a * s * s + b * cs * cs;
}

double dphi_a(double x, double a, double b, double /*c*/)
{
    return (a - b) * std::sin(2.0 * x);
}

double phi_b(double x, double a, double b, double c)
{
    return c + a * std::exp(-b * x * x);
}

double dphi_b(double x, double a, double b, double /*c*/)
{
    return -2.0 * a * b * x * std::exp(-b * x * x);
}

typedef double (*PhiFunc)(double x, double a, double b, double c);
typedef double (*DPhiFunc)(double x, double a, double b, double c);

struct IterResult
{
    bool   converged;
    double root;
    int    iters;
    double deriv_at_root; //значение произвдной в корне
};

// Итерационный процесс
// phi      - итерационная функция
// dphi     - её производная
// x0       - начальное приближение
// a, b, c  - параметры функции
// tol      - точность (критерий останова |x_{n+1} - x_n| < tol)
// max_iter - максимальное число итераций
IterResult run_iteration(PhiFunc phi, DPhiFunc dphi,
    double x0,
    double a, double b, double c,
    double tol, int max_iter)
{
    IterResult res;
    res.converged = false;
    res.root = x0;
    res.iters = 0;
    res.deriv_at_root = 0.0;

    double x_cur = x0;

    std::cout << std::fixed << std::setprecision(10);
    std::cout << "\n  Iteration |  x_n\n";
    std::cout << "  ---------|------------------------------\n";
    std::cout << "  " << std::setw(8) << 0 << " | " << x_cur << "\n";

    for (int n = 1; n <= max_iter; ++n)
    {
        double x_next = phi(x_cur, a, b, c);

        // проверка на NaN или Inf
        if (!std::isfinite(x_next))
        {
            std::cout << "  " << std::setw(8) << n
                << " | [diverges: value went out of bounds]\n";
            res.iters = n;
            return res;
        }

        std::cout << "  " << std::setw(8) << n << " | " << x_next << "\n";

        if (std::fabs(x_next - x_cur) < tol)
        {
            res.converged = true;
            res.root = x_next;
            res.iters = n;
            res.deriv_at_root = dphi(x_next, a, b, c);
            return res;
        }

        x_cur = x_next;
    }

    res.iters = max_iter;
    res.root = x_cur;
    return res;
}


double read_double(const std::string& prompt)
{
    double val;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> val)
            return val;
        else
        {
            std::cout << "  [Error] Enter a real number.\n";
            std::cin.clear(); //сбрасывает состояние ошибки
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}


int read_int(const std::string& prompt)
{
    int val;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> val)
            return val;
        else
        {
            std::cout << "  [Error] Enter an integer number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}


void experiment_a()
{
    std::cout << "\n================================================\n";
    std::cout << "  FUNCTION (a): phi(x) = c + a*sin^2(x) + b*cos^2(x)\n";
    std::cout << "================================================\n";
    std::cout << "  Theory: phi'(x) = (a-b)*sin(2x)\n";
    std::cout << "  max|phi'(x)| = |a - b|\n";
    std::cout << "  Global convergence condition: |a - b| < 1\n\n";

    double a = read_double("  Enter a: ");
    double b = read_double("  Enter b: ");
    double c = read_double("  Enter c: ");
    double x0 = read_double("  Enter initial approximation x0: ");

    double q = std::fabs(a - b);
    std::cout << "\n  |a - b| = " << std::fixed << std::setprecision(6) << q << "\n";

    if (q < 1.0)
        std::cout << "  => Convergence condition SATISFIED (|a-b| < 1). "
        "Convergence guaranteed for any x0.\n";
    else
        std::cout << "  => Convergence condition NOT satisfied (|a-b| >= 1). "
        "Global convergence not guaranteed.\n";

    IterResult res = run_iteration(phi_a, dphi_a, x0, a, b, c,
        TOLERANCE, MAX_ITER);

    std::cout << "\n  ---- Result ----\n";
    if (res.converged)
    {
        std::cout << std::fixed << std::setprecision(10);
        std::cout << "  Process CONVERGED.\n";
        std::cout << "  Fixed point x* = " << res.root << "\n";
        std::cout << "  Number of iterations: " << res.iters << "\n";
        std::cout << "  phi'(x*) = " << res.deriv_at_root << "\n";
        if (std::fabs(res.deriv_at_root) < 1.0)
            std::cout << "  |phi'(x*)| < 1 - local convergence condition satisfied.\n";
        else
            std::cout << "  |phi'(x*)| >= 1 - local condition not satisfied "
            "(convergence might be accidental for this x0).\n";
    }
    else
        std::cout << "  Process DIVERGES (limit of " << MAX_ITER
        << " iterations exhausted or value went to infinity).\n";
}


void experiment_b()
{
    std::cout << "\n================================================\n";
    std::cout << "  FUNCTION (b): phi(x) = c + a*exp(-b*x^2),  b > 0\n";
    std::cout << "================================================\n";
    std::cout << "  Theory: phi'(x) = -2*a*b*x*exp(-b*x^2)\n";
    std::cout << "  max|phi'(x)| = |a| * sqrt(2b/e)\n";
    std::cout << "  Global convergence condition: |a| < sqrt(e / (2b))\n\n";

    double a = read_double("  Enter a: ");

    double b;
    while (true)
    {
        b = read_double("  Enter b (b > 0): ");
        if (b > 0.0)
            break;
        std::cout << "  [Warning] b must be > 0. Retry input.\n";
    }

    double c = read_double("  Enter c: ");
    double x0 = read_double("  Enter initial approximation x0: ");

    // критическое значение: |a| < sqrt(e / (2b))
    double e_val = std::exp(1.0);
    double a_crit = std::sqrt(e_val / (2.0 * b));
    double max_dphi = std::fabs(a) * std::sqrt(2.0 * b / e_val);

    std::cout << "\n  Critical value |a|_crit = sqrt(e/(2b)) = "
        << std::fixed << std::setprecision(6) << a_crit << "\n";
    std::cout << "  Your |a| = " << std::fabs(a) << "\n";
    std::cout << "  max|phi'(x)| = |a|*sqrt(2b/e) = " << max_dphi << "\n";

    if (max_dphi < 1.0)
        std::cout << "  => Convergence condition SATISFIED (max|phi'| < 1). "
        "Convergence guaranteed for any x0.\n";
    else
        std::cout << "  => Convergence condition NOT satisfied (max|phi'| >= 1). "
        "Global convergence not guaranteed.\n";

    IterResult res = run_iteration(phi_b, dphi_b, x0, a, b, c,
        TOLERANCE, MAX_ITER);

    std::cout << "\n  ---- Result ----\n";
    if (res.converged)
    {
        std::cout << std::fixed << std::setprecision(10);
        std::cout << "  Process CONVERGED.\n";
        std::cout << "  Fixed point x* = " << res.root << "\n";
        std::cout << "  Number of iterations: " << res.iters << "\n";
        std::cout << "  phi'(x*) = " << res.deriv_at_root << "\n";
        if (std::fabs(res.deriv_at_root) < 1.0)
            std::cout << "  |phi'(x*)| < 1 - local convergence condition satisfied.\n";
        else
            std::cout << "  |phi'(x*)| >= 1 - local condition not satisfied.\n";
    }
    else
        std::cout << "  Process DIVERGES (limit of " << MAX_ITER
        << " iterations exhausted or value went to infinity).\n";
}


void print_menu()
{
    std::cout << "   Simple Iteration Method: x = phi(x)        \n\n";
    std::cout << "  1. phi(x) = c + a*sin^2(x) + b*cos^2(x)    \n";
    std::cout << "  2. phi(x) = c + a*exp(-b*x^2)  (b > 0)     \n";
    std::cout << "  0. Exit                                     \n";
    std::cout << "  Your choice: ";
}


int main()
{
    std::cout << "Program for studying convergence of simple iteration method\n";
    std::cout << "Precision: " << TOLERANCE
        << "  |  Max iterations: " << MAX_ITER << "\n";

    int choice;
    while (true)
    {
        print_menu();
        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  [Error] Enter 0, 1 or 2.\n";
            continue;
        }

        if (choice == 0)
        {
            std::cout << "\nExiting program. \n";
            break;
        }
        else if (choice == 1)
            experiment_a();
        else if (choice == 2)
            experiment_b();
        else
            std::cout << "  [Error] Invalid choice. Enter 0, 1 or 2.\n";

        std::cout << "\n  [Press Enter to return to menu...]\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //игнорируем символы до \n
        std::cin.get(); //продолжаем
    }

    return 0;
}
