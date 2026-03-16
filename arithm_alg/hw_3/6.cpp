
#include <iostream>
#include <cmath>
#include <functional>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <stdexcept>


struct Interval { double a, b; };

struct RootResult
{
    double root;
    int    iterations;
    bool   converged;
};


RootResult bisection(std::function<double(double)> f, 
    double a, 
    double b, 
    double eps, 
    bool verbose) //вывод промежуточных резов
{
    RootResult res{ 0.0, 0, false };

    double fa = f(a), fb = f(b);
    if (fa * fb > 0.0)
    {
        std::cout << "  [Bisection] No sign change on interval ["
            << a << ", " << b << "]\n";
        return res;
    }

    if (std::fabs(fa) < eps) 
    { 
        res.root = a; 
        res.iterations = 0; 
        res.converged = true; 
        return res; 
    }

    if (std::fabs(fb) < eps) 
    { 
        res.root = b;
        res.iterations = 0; 
        res.converged = true; 
        return res; 
    }

    const int MAX_ITER = 100000;
    double mid = a;

    for (int iter = 1; iter <= MAX_ITER; ++iter)
    {
        mid = (a + b) * 0.5;
        double fm = f(mid);

        if (verbose)
            std::cout << "  Iteration " << std::setw(5) << iter
            << ": x = " << std::setprecision(10) << std::fixed << mid << "\n";

        if ((b - a) * 0.5 < eps || std::fabs(fm) < eps)
        {
            res.root = mid;
            res.iterations = iter;
            res.converged = true;
            return res;
        }

        if (fa * fm < 0.0) 
        { 
            b = mid; 
            fb = fm; 
        }
        else 
        {
            a = mid; 
            fa = fm; 
        }
    }

    res.root = mid;
    res.iterations = MAX_ITER;
    res.converged = false;
    std::cout << "  [Bisection] Maximum number of iterations exceeded.\n";
    return res;
}


RootResult newton(std::function<double(double)> f, 
    std::function<double(double)> df,
    double x0,
    double eps,
    bool verbose)
{
    RootResult res{ x0, 0, false };
    const int MAX_ITER = 1000;
    double x = x0;

    for (int iter = 1; iter <= MAX_ITER; ++iter)
    {
        double fx = f(x);
        double dfx = df(x);

        if (std::fabs(dfx) < 1e-15)
        {
            std::cout << "  [Newton] Derivative is approximately zero at iteration "
                << iter << ". Method aborted.\n";
            res.root = x;
            res.iterations = iter;
            res.converged = false;
            return res;
        }

        double x_new = x - fx / dfx;

        if (verbose)
            std::cout << "  Iteration " << std::setw(5) << iter
            << ": x = " << std::setprecision(15) << std::fixed << x_new << "\n";

        if (std::fabs(x_new - x) < eps || std::fabs(f(x_new)) < eps)
        {
            res.root = x_new;
            res.iterations = iter;
            res.converged = true;
            return res;
        }

        x = x_new;
    }

    std::cout << "  [Newton] Maximum number of iterations exceeded.\n";
    res.root = x;
    res.iterations = MAX_ITER;
    res.converged = false;
    return res;
}


std::vector<Interval> localize(std::function<double(double)> f,
    double lo,
    double hi,
    int N = 50000)
{
    std::vector<Interval> result;
    double step = (hi - lo) / N;
    double xprev = lo;
    double fprev = f(xprev);

    for (int i = 1; i <= N; ++i)
    {
        double xcur = lo + i * step;
        double fcur = f(xcur);

        if (std::isfinite(fprev) && std::isfinite(fcur) && fprev * fcur < 0.0)
            result.push_back({ xprev, xcur });

        xprev = xcur;
        fprev = fcur;
    }
    return result;
}


void printComparison(const RootResult& resB, const RootResult& resN)
{
    std::cout << std::string(65, '-') << "\n";
    std::cout << "  Result:\n";
    if (resB.converged)
        std::cout << "  Bisection : root = " << std::setprecision(15) << std::fixed
        << resB.root << ",  iterations = " << resB.iterations << "\n";
    else
        std::cout << "  Bisection : did NOT converge\n";

    if (resN.converged)
        std::cout << "  Newton    : root = " << std::setprecision(15) << std::fixed
        << resN.root << ",  iterations = " << resN.iterations << "\n";
    else
        std::cout << "  Newton    : did NOT converge\n";

    if (resB.converged && resN.converged)
    {
        int diff = resB.iterations - resN.iterations;
        if (diff > 0)
            std::cout << "  => Newton's method is faster by " << diff << " iterations.\n";
        else if (diff < 0)
            std::cout << "  => Bisection method is faster by " << (-diff) << " iterations.\n";
        else
            std::cout << "  => Both methods converged in the same number of iterations.\n";
    }
}


void solveOneRoot(int rootNum, Interval interval,
    std::function<double(double)> f, std::function<double(double)> df,
    double eps, double newtonX0)
{
    std::cout << std::string(65, '-') << "\n";
    std::cout << "Root #" << rootNum
        << "  (interval [" << interval.a << ", " << interval.b << "])\n";

    std::cout << "\n  Bisection method:\n";
    RootResult resB = bisection(f, interval.a, interval.b, eps, true);

    std::cout << "\n  Newton's method  (x0 = " << newtonX0 << "):\n";
    RootResult resN = newton(f, df, newtonX0, eps, true);

    printComparison(resB, resN);
}

//  (a): sin(x) - 2x^2 + 0.5 = 0
//  Корни: около -0.27 и около 0.55
void solveA(double eps)
{
    std::cout << std::string(65, '=') << "\n";
    std::cout << "Equation (a): sin(x) - 2*x^2 + 0.5 = 0\n";
    std::cout << std::string(65, '=') << "\n";

    std::function<double(double)> f = [](double x) { return std::sin(x) - 2.0 * x * x + 0.5; };
    std::function<double(double)> df = [](double x) { return std::cos(x) - 4.0 * x; };

    // локализация: ищем на [-2, 2]
    auto intervals = localize(f, -2.0, 2.0);

    if (intervals.empty())
    {
        std::cout << "No roots found on interval [-2, 2].\n";
        return;
    }

    for (int i = 0; i < (int)intervals.size(); ++i)
    {
        double x0 = (intervals[i].a + intervals[i].b) * 0.5;
        solveOneRoot(i + 1, intervals[i], f, df, eps, x0);
    }
}

//  b): x^powN = powA  =>  f(x) = x^powN - powA
//  При нечётном powN: один положительный корень
//  При чётном  powN: два корня: +a^(1/n) и -a^(1/n)
void solveB(double eps, int powN, double powA)
{
    std::cout << std::string(65, '=') << "\n";
    std::cout << "Equation (b): x^" << powN << " = " << powA << "\n";
    std::cout << std::string(65, '=') << "\n";

    // f(x) = x^n - a
    std::function<double(double)> f = [powN, powA](double x)
        {
            // std::pow для целых степеней с возможностью отрицательного x
            double val = 1.0;
            double ax = std::fabs(x);
            for (int i = 0; i < powN; ++i) val *= ax;
            if (powN % 2 == 1 && x < 0) val = -val;
            return val - powA;
        };

    // f'(x) = n * x^(n-1)
    std::function<double(double)> df = [powN](double x)
        {
            double val = 1.0;
            double ax = std::fabs(x);
            for (int i = 0; i < powN - 1; ++i) val *= ax;
            if ((powN - 1) % 2 == 1 && x < 0) val = -val;
            return (double)powN * val;
        };

    double root_approx = std::pow(powA, 1.0 / powN);  // примерный корень

    if (powN % 2 == 1)
    {
        double lo = 0.0, hi = powA + 2.0;
        if (hi < root_approx + 1.0) hi = root_approx + 1.0;
        solveOneRoot(1, { lo, hi }, f, df, eps, root_approx * 0.5 + 0.1);
    }
    else
    {
        double bound = root_approx + 1.0;
        solveOneRoot(1, { 0.0,  bound }, f, df, eps, root_approx * 0.5 + 0.1);
        solveOneRoot(2, { -bound, 0.0 }, f, df, eps, -root_approx * 0.5 - 0.1);
    }
}

//  c): sqrt(1 - x^2) - e^x + 0.1 = 0
void solveC(double eps)
{
    std::cout << std::string(65, '=') << "\n";
    std::cout << "Equation (c): sqrt(1 - x^2) - e^x + 0.1 = 0,  |x| <= 1\n";
    std::cout << std::string(65, '=') << "\n";

    std::function<double(double)> f = [](double x)
        {
            if (std::fabs(x) > 1.0) return std::numeric_limits<double>::quiet_NaN();
            return std::sqrt(1.0 - x * x) - std::exp(x) + 0.1;
        };

    // f'(x) = -x / sqrt(1-x^2) - e^x
    std::function<double(double)> df = [](double x)
        {
            double s = 1.0 - x * x;
            if (s <= 0.0) return std::numeric_limits<double>::quiet_NaN();
            return -x / std::sqrt(s) - std::exp(x);
        };

    auto intervals = localize(f, -0.9999, 0.9999, 100000);

    if (intervals.empty())
    {
        std::cout << "No roots found.\n";
        return;
    }

    for (int i = 0; i < (int)intervals.size(); ++i)
    {
        double x0 = (intervals[i].a + intervals[i].b) * 0.5;
        solveOneRoot(i + 1, intervals[i], f, df, eps, x0);
    }
}

//  d): x^6 - 5x^3 - 2 = 0
//  Действительные корни: около -0.68 и около 1.88
void solveD(double eps)
{
    std::cout << std::string(65, '=') << "\n";
    std::cout << "Equation (d): x^6 - 5*x^3 - 2 = 0\n";
    std::cout << std::string(65, '=') << "\n";

    std::function<double(double)> f = [](double x)
        {
            double x3 = x * x * x;
            double x6 = x3 * x3;
            return x6 - 5.0 * x3 - 2.0;
        };

    std::function<double(double)> df = [](double x)
        {
            double x2 = x * x;
            double x5 = x2 * x2 * x;
            return 6.0 * x5 - 15.0 * x2;
        };

    auto intervals = localize(f, -5.0, 5.0);

    if (intervals.empty())
    {
        std::cout << "No roots found on interval [-5, 5].\n";
        return;
    }

    for (int i = 0; i < (int)intervals.size(); ++i)
    {
        double x0 = (intervals[i].a + intervals[i].b) * 0.5;
        solveOneRoot(i + 1, intervals[i], f, df, eps, x0);
    }
}

//  e): log2(x) - 1/(1+x^2) = 0,  x > 0
//  Один корень около x ≈ 1.30
void solveE(double eps)
{
    std::cout << std::string(65, '=') << "\n";
    std::cout << "Equation (e): log2(x) = 1/(1 + x^2),  x > 0\n";
    std::cout << std::string(65, '=') << "\n";

    std::function<double(double)> f = [](double x)
        {
            if (x <= 0.0) return std::numeric_limits<double>::quiet_NaN();
            return std::log2(x) - 1.0 / (1.0 + x * x);
        };

    // f'(x) = 1/(x*ln2) + 2x/(1+x^2)^2
    std::function<double(double)> df = [](double x)
        {
            if (x <= 0.0) return std::numeric_limits<double>::quiet_NaN();
            double d1 = 1.0 / (x * std::log(2.0));
            double d2 = 2.0 * x / ((1.0 + x * x) * (1.0 + x * x));
            return d1 + d2;
        };

    auto intervals = localize(f, 0.001, 10.0, 100000);

    if (intervals.empty())
    {
        std::cout << "No roots found on interval (0, 10].\n";
        return;
    }

    for (int i = 0; i < (int)intervals.size(); ++i)
    {
        double x0 = (intervals[i].a + intervals[i].b) * 0.5;
        solveOneRoot(i + 1, intervals[i], f, df, eps, x0);
    }
}

//  f): sin(x/2) = 1  =>  sin(x/2) - 1 = 0
//  Бесконечно много решений: x = pi + 4*pi*k
//  Первые три наименьших по модулю:
//    k= 0: x =   pi  ≈  3.14159
//    k=-1: x = -3*pi ≈ -9.42478
//    k= 1: x =  5*pi ≈ 15.70796
void solveF(double eps)
{
    std::cout << std::string(65, '=') << "\n";
    std::cout << "Equation (f): sin(x/2) = 1  =>  sin(x/2) - 1 = 0\n";
    std::cout << "Infinitely many solutions. We search for 3 smallest by absolute value.\n";
    std::cout << "NOTE:\n"
        << "  The root is at the maximum of sin(x/2)=1, so there is NO sign change.\n"
        << "  Bisection method is applied to h(x)=x/2 - (pi/2 + 2*pi*k)=0\n"
        << "  (equivalent equation with explicit sign change).\n"
        << "  Newton's method is applied to the original equation.\n";
    std::cout << std::string(65, '=') << "\n";

    const double PI = std::acos(-1.0);

    // Для Ньютона — исходное уравнение
    std::function<double(double)> fOrig = [](double x) { return std::sin(x * 0.5) - 1.0; };
    std::function<double(double)> dfOrig = [](double x) { return 0.5 * std::cos(x * 0.5); };

    struct RootInfo { int k; double exact; };
    std::vector<RootInfo> roots =
    {
        {  0,  PI        },
        { -1, -3.0 * PI  },
        {  1,  5.0 * PI  }
    };

    for (int i = 0; i < (int)roots.size(); ++i)
    {
        double xc = roots[i].exact;
        int    k = roots[i].k;
        double cst = PI * (0.5 + 2.0 * k); 

        // h(x) = x/2 - cst;  h'(x) = 1/2
        std::function<double(double)> fBis = [cst](double x) { return x * 0.5 - cst; };

        std::cout << std::string(65, '-') << "\n";
        std::cout << "Root #" << (i + 1)
            << "  (k=" << k << ", exact value x = "
            << std::setprecision(10) << xc << ")\n";

        std::cout << "\n  Bisection method (for h(x)=x/2 - " << cst
            << ", interval [" << (xc - 2.0) << ", " << (xc + 2.0) << "]):\n";
        RootResult resB = bisection(fBis, xc - 2.0, xc + 2.0, eps, true);

        double x0 = xc - 0.3;  // чуть в стороне, чтобы производная не была 0
        std::cout << "\n  Newton's method  (x0 = " << x0 << "):\n";
        RootResult resN = newton(fOrig, dfOrig, x0, eps, true);

        printComparison(resB, resN);
    }
}

//  g): ln(x) - 1 = 0
//  Единственный корень: x = e ≈ 2.71828
void solveG(double eps)
{
    std::cout << std::string(65, '=') << "\n";
    std::cout << "Equation (g): ln(x) = 1  =>  ln(x) - 1 = 0\n";
    std::cout << std::string(65, '=') << "\n";

    std::function<double(double)> f = [](double x)
        {
            if (x <= 0.0) return std::numeric_limits<double>::quiet_NaN();
            return std::log(x) - 1.0;
        };
    std::function<double(double)> df = [](double x)
        {
            if (x <= 0.0) return std::numeric_limits<double>::quiet_NaN();
            return 1.0 / x;
        };

    solveOneRoot(1, { 1.0, 4.0 }, f, df, eps, 2.0);
}


void printMenu()
{
    std::cout << "\n";
    std::cout << std::string(65, '=') << "\n";
    std::cout << "  ROOT-FINDING EQUATIONS MENU\n";
    std::cout << std::string(65, '=') << "\n";
    std::cout << "  a) sin(x) - 2*x^2 + 0.5 = 0\n";
    std::cout << "  b) x^n = a  (parameters n, a are entered by user)\n";
    std::cout << "  c) sqrt(1 - x^2) - e^x + 0.1 = 0\n";
    std::cout << "  d) x^6 - 5*x^3 - 2 = 0\n";
    std::cout << "  e) log2(x) = 1/(1 + x^2)\n";
    std::cout << "  f) sin(x/2) = 1  (infinitely many roots; 3 with smallest |x|)\n";
    std::cout << "  g) ln(x) = 1\n";
    std::cout << "  q) Quit\n";
    std::cout << std::string(65, '=') << "\n";
    std::cout << "Your choice: ";
}


int main()
{
    std::cout << std::fixed;

    int n_prec = 0;
    std::cout << "Enter n (accuracy eps = 10^(-n), n >= 1): ";
    while (!(std::cin >> n_prec) || n_prec < 1)
    {
        std::cout << "Invalid input. Please enter an integer n >= 1: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    double eps = std::pow(10.0, -(double)n_prec);
    std::cout << "Accuracy eps = 10^(-" << n_prec << ") = " << eps << "\n";

    char choice = 0;
    while (true)
    {
        printMenu();
        std::cin >> choice;
        choice = (char)std::tolower((unsigned char)choice);

        if (choice == 'q') 
            break;

        switch (choice)
        {
        case 'a':
            solveA(eps);
            break;

        case 'b':
        {
            int    powN = 0;
            double powA = 0.0;
            std::cout << "Enter n (integer, >= 1): ";
            while (!(std::cin >> powN) || powN < 1)
            {
                std::cout << "Invalid input. Please enter an integer n >= 1: ";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
            std::cout << "Enter a (real number, > 0): ";
            while (!(std::cin >> powA) || powA <= 0.0)
            {
                std::cout << "Invalid input. Please enter a > 0: ";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
            solveB(eps, powN, powA);
            break;
        }

        case 'c':
            solveC(eps);
            break;

        case 'd':
            solveD(eps);
            break;

        case 'e':
            solveE(eps);
            break;

        case 'f':
            solveF(eps);
            break;

        case 'g':
            solveG(eps);
            break;

        default:
            std::cout << "Unknown command. Please try again.\n";
            break;
        }
    }

    return 0;
}