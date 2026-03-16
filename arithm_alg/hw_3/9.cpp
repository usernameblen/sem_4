#include <iostream>
#include <cmath>
#include <functional>
#include <optional>
#include <iomanip>
#include <string>
#include <vector>

struct Vec2 {
    double x{ 0 }, y{ 0 };

    double norm() const { return std::hypot(x, y); }

    Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
};

struct Mat2 {
    double a{ 0 }, b{ 0 };
    double c{ 0 }, d{ 0 };
};

Vec2 F_a(double x, double y, double A, double alpha2, double beta2)
{
    return {
        std::tan(x * y + A) - x * x,
        x * x / alpha2 + y * y / beta2 - 1.0
    };
}

Mat2 J_a(double x, double y, double A, double alpha2, double beta2)
{
    double cosv = std::cos(x * y + A);
    double sec2 = 1.0 / (cosv * cosv);
    return {
        y * sec2 - 2.0 * x,  x * sec2,
        2.0 * x / alpha2,    2.0 * y / beta2
    };
}

Vec2 F_b(double x1, double x2, double, double, double)
{
    return {
        x1 * x1 + x2 * x2 - 2.0,
        std::exp(x1 - 1.0) + x2 * x2 * x2 - 2.0
    };
}

Mat2 J_b(double x1, double x2, double, double, double)
{
    return {
        2.0 * x1,          2.0 * x2,
        std::exp(x1 - 1.0), 3.0 * x2 * x2
    };
}

bool solveLinear(const Mat2& J, const Vec2& rhs, Vec2& delta, double& det)
{
    double A[2][3] = {
        {J.a, J.b, rhs.x},
        {J.c, J.d, rhs.y}
    };

    for (int col = 0; col < 2; ++col) {
        int pivotRow = col;
        double maxVal = std::fabs(A[col][col]);
        for (int row = col + 1; row < 2; ++row) {
            if (std::fabs(A[row][col]) > maxVal) {
                maxVal = std::fabs(A[row][col]);
                pivotRow = row;
            }
        }

        if (pivotRow != col)
            std::swap(A[col], A[pivotRow]);

        if (std::fabs(A[col][col]) < 1e-15)
            return false;

        for (int row = col + 1; row < 2; ++row) {
            double factor = A[row][col] / A[col][col];
            for (int k = col; k < 3; ++k)
                A[row][k] -= factor * A[col][k];
        }
    }

    det = A[0][0] * A[1][1];

    if (std::fabs(det) < 1e-15)
        return false;

    delta.y = A[1][2] / A[1][1];
    delta.x = (A[0][2] - A[0][1] * delta.y) / A[0][0];

    return true;
}

using FFunc = std::function<Vec2(double, double, double, double, double)>;
using JFunc = std::function<Mat2(double, double, double, double, double)>;

struct NewtonResult {
    Vec2   solution;
    int    iterations;
    Vec2   residual;
    bool   converged;
    std::string message;
};

NewtonResult newton(Vec2 X0,
    double epsilon,
    int maxIter,
    const FFunc& Ffunc,
    const JFunc& Jfunc,
    double param1 = 0.0,
    double param2 = 1.0,
    double param3 = 1.0)
{
    Vec2 X = X0;
    int  iter = 0;

    for (; iter < maxIter; ++iter) {
        Vec2 F = Ffunc(X.x, X.y, param1, param2, param3);
        Mat2 J = Jfunc(X.x, X.y, param1, param2, param3);

        Vec2   rhs = { -F.x, -F.y };
        Vec2   delta = {};
        double det = 0.0;

        if (!solveLinear(J, rhs, delta, det))
            return { X, iter, Ffunc(X.x, X.y, param1, param2, param3),
                    false, "Singular Jacobian matrix at iteration " + std::to_string(iter) };

        Vec2 Xnew = { X.x + delta.x, X.y + delta.y };

        Vec2 Fnew = Ffunc(Xnew.x, Xnew.y, param1, param2, param3);
        double normF = Fnew.norm();
        double normD = (Xnew - X).norm();

        X = Xnew;

        if (normF < epsilon && normD < epsilon) {
            ++iter;
            return { X, iter, Fnew, true, "Converged" };
        }
    }

    Vec2 Ffinal = Ffunc(X.x, X.y, param1, param2, param3);
    return { X, iter, Ffinal, false, "Maximum iterations exceeded" };
}

void printResult(const std::string& label, const NewtonResult& r)
{
    std::cout << "\n=== " << label << " ===\n";
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "  x        = " << r.solution.x << "\n";
    std::cout << "  y        = " << r.solution.y << "\n";
    std::cout << "  Iterations: " << r.iterations << "\n";
    std::cout << "  ||F||_2   = " << std::scientific << r.residual.norm() << "\n";
    std::cout << std::fixed;
    std::cout << "  F1(x,y)  = " << r.residual.x << "\n";
    std::cout << "  F2(x,y)  = " << r.residual.y << "\n";
    std::cout << "  Status   : " << r.message << "\n";
}

int main()
{
    constexpr double eps = 1e-9;
    constexpr int    maxIter = 100;

    std::cout << std::setprecision(12);

    struct CaseA {
        std::string name;
        double A, alpha2, beta2;
        Vec2   X0;
    };

    std::vector<CaseA> cases_a = {
        {"a-i:   A=0.2, a^2=1/0.6, b^2=1/2",   0.2, 1.0 / 0.6, 1.0 / 2.0, {0.5,  0.5}},
        {"a-ii:  A=0.4, a^2=1/0.8, b^2=1/2",   0.4, 1.0 / 0.8, 1.0 / 2.0, {0.5,  0.5}},
        {"a-iii: A=0.3, a^2=1/0.2, b^2=1/3",   0.3, 1.0 / 0.2, 1.0 / 3.0, {0.5,  0.5}},
        {"a-iv:  A=0,   a^2=1/0.6, b^2=1/2",   0.0, 1.0 / 0.6, 1.0 / 2.0, {0.5,  0.5}},
    };

    std::cout << "\n SYSTEM (a): tan(xy+A)=x^2,  x^2/a^2+y^2/b^2=1    \n";

    for (auto& c : cases_a) {
        auto res = newton(c.X0, eps, maxIter, F_a, J_a, c.A, c.alpha2, c.beta2);
        printResult(c.name, res);

        if (!res.converged) {
            std::cout << "  -> Trying alternative guess (-0.5, -0.5)...\n";
            res = newton({ -0.5, -0.5 }, eps, maxIter, F_a, J_a, c.A, c.alpha2, c.beta2);
            printResult(c.name + " [alt]", res);
        }
    }

    std::cout << "\n SYSTEM (b): x1^2+x2^2=2,  e^(x1-1)+x2^3=2    \n";

    struct CaseB {
        std::string name;
        Vec2        X0;
    };

    std::vector<CaseB> cases_b = {
        {"b: initial (0.5, 0.5)",   {0.5,  0.5}},
        {"b: initial (-1.0, 1.2)",  {-1.0, 1.2}},
        {"b: initial (1.2, -1.0)",  {1.2, -1.0}},
    };

    for (auto& c : cases_b) {
        auto res = newton(c.X0, eps, maxIter, F_b, J_b, 0.0, 1.0, 1.0);
        printResult(c.name, res);
    }

    return 0;
}
