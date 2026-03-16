#define _USE_MATH_DEFINES

#include <cmath>
#include <iomanip>
#include <iostream>

static const int    MAX_ITER = 1000;
static const double EPS = 1e-14;

static bool invertMat3(double A[3][3], double inv[3][3])
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            inv[i][j] = (i == j) ? 1.0 : 0.0;

    for (int col = 0; col < 3; ++col) {
        int    pivot = col;
        double maxVal = std::abs(A[col][col]);
        for (int row = col + 1; row < 3; ++row) {
            if (std::abs(A[row][col]) > maxVal) {
                maxVal = std::abs(A[row][col]);
                pivot = row;
            }
        }
        if (maxVal < EPS) return false;

        if (pivot != col) {
            for (int j = 0; j < 3; ++j) {
                double t;
                t = A[col][j];   A[col][j] = A[pivot][j];   A[pivot][j] = t;
                t = inv[col][j]; inv[col][j] = inv[pivot][j]; inv[pivot][j] = t;
            }
        }

        double d = A[col][col];
        for (int j = 0; j < 3; ++j) { A[col][j] /= d; inv[col][j] /= d; }

        for (int row = 0; row < 3; ++row) {
            if (row == col) continue;
            double f = A[row][col];
            for (int j = 0; j < 3; ++j) {
                A[row][j] -= f * A[col][j];
                inv[row][j] -= f * inv[col][j];
            }
        }
    }
    return true;
}

static void matvec3(const double M[3][3], const double v[3], double result[3])
{
    for (int i = 0; i < 3; ++i) {
        result[i] = 0.0;
        for (int j = 0; j < 3; ++j)
            result[i] += M[i][j] * v[j];
    }
}

static double normInf3(const double a[3], const double b[3])
{
    double mx = 0.0;
    for (int i = 0; i < 3; ++i) {
        double d = std::abs(a[i] - b[i]);
        if (d > mx) mx = d;
    }
    return mx;
}

static bool invertMat4(double A[4][4], double inv[4][4])
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            inv[i][j] = (i == j) ? 1.0 : 0.0;

    for (int col = 0; col < 4; ++col) {
        int    pivot = col;
        double maxVal = std::abs(A[col][col]);
        for (int row = col + 1; row < 4; ++row) {
            if (std::abs(A[row][col]) > maxVal) {
                maxVal = std::abs(A[row][col]);
                pivot = row;
            }
        }
        if (maxVal < EPS) return false;

        if (pivot != col) {
            for (int j = 0; j < 4; ++j) {
                double t;
                t = A[col][j];   A[col][j] = A[pivot][j];   A[pivot][j] = t;
                t = inv[col][j]; inv[col][j] = inv[pivot][j]; inv[pivot][j] = t;
            }
        }

        double d = A[col][col];
        for (int j = 0; j < 4; ++j) { A[col][j] /= d; inv[col][j] /= d; }

        for (int row = 0; row < 4; ++row) {
            if (row == col) continue;
            double f = A[row][col];
            for (int j = 0; j < 4; ++j) {
                A[row][j] -= f * A[col][j];
                inv[row][j] -= f * inv[col][j];
            }
        }
    }
    return true;
}

static void matvec4(const double M[4][4], const double v[4], double result[4])
{
    for (int i = 0; i < 4; ++i) {
        result[i] = 0.0;
        for (int j = 0; j < 4; ++j)
            result[i] += M[i][j] * v[j];
    }
}

static double normInf4(const double a[4], const double b[4])
{
    double mx = 0.0;
    for (int i = 0; i < 4; ++i) {
        double d = std::abs(a[i] - b[i]);
        if (d > mx) mx = d;
    }
    return mx;
}

static void F_a(const double x[3], double f[3])
{
    double x1 = x[0], x2 = x[1], x3 = x[2];
    f[0] = x1 * x1 * x1 + x1 * x1 * x2 - x1 * x3 + 6.0;
    f[1] = std::exp(x1) + std::exp(x2) - x3;
    f[2] = x2 * x2 - 2.0 * x1 * x3 - 4.0;
}

static void J_a(const double x[3], double J[3][3])
{
    double x1 = x[0], x2 = x[1], x3 = x[2];
    J[0][0] = 3.0 * x1 * x1 + 2.0 * x1 * x2 - x3; J[0][1] = x1 * x1;        J[0][2] = -x1;
    J[1][0] = std::exp(x1);                 J[1][1] = std::exp(x2); J[1][2] = -1.0;
    J[2][0] = -2.0 * x3;                      J[2][1] = 2.0 * x2;       J[2][2] = -2.0 * x1;
}

static void F_b(const double x[3], double f[3])
{
    double x1 = x[0], x2 = x[1], x3 = x[2];
    double arg = x1 * x1 + std::sin(x3) + 1.06;
    f[0] = 6.0 * x1 - 2.0 * std::cos(x2 * x3) - 1.0;
    f[1] = 9.0 * x2 + std::sqrt(arg) + 0.9;
    f[2] = 60.0 * x3 + 3.0 * std::exp(-x1 * x2) + 10.0 * M_PI - 3.0;
}

static void J_b(const double x[3], double J[3][3])
{
    double x1 = x[0], x2 = x[1], x3 = x[2];
    double arg = x1 * x1 + std::sin(x3) + 1.06;
    double inv2sqrt = 0.5 / std::sqrt(arg);
    double sinX2X3 = std::sin(x2 * x3);
    double eNeg = std::exp(-x1 * x2);

    J[0][0] = 6.0;               J[0][1] = 2.0 * x3 * sinX2X3;     J[0][2] = 2.0 * x2 * sinX2X3;
    J[1][0] = 2.0 * x1 * inv2sqrt;   J[1][1] = 9.0;                J[1][2] = std::cos(x3) * inv2sqrt;
    J[2][0] = -3.0 * x2 * eNeg;      J[2][1] = -3.0 * x1 * eNeg;       J[2][2] = 60.0;
}

static void F_c(const double x[4], double f[4])
{
    double x1 = x[0], x2 = x[1], x3 = x[2], x4 = x[3];
    f[0] = 4.0 * x1 - x2 + x3 - x1 * x4;
    f[1] = -x1 + 3.0 * x2 - 2.0 * x3 - x2 * x4;
    f[2] = x1 - 2.0 * x2 + 3.0 * x3 - x3 * x4;
    f[3] = x1 * x1 + x2 * x2 + x3 * x3 - 1.0;
}

static void J_c(const double x[4], double J[4][4])
{
    double x1 = x[0], x2 = x[1], x3 = x[2], x4 = x[3];
    J[0][0] = 4.0 - x4; J[0][1] = -1.0;    J[0][2] = 1.0;    J[0][3] = -x1;
    J[1][0] = -1.0;   J[1][1] = 3.0 - x4;  J[1][2] = -2.0;    J[1][3] = -x2;
    J[2][0] = 1.0;   J[2][1] = -2.0;    J[2][2] = 3.0 - x4;  J[2][3] = -x3;
    J[3][0] = 2.0 * x1; J[3][1] = 2.0 * x2;  J[3][2] = 2.0 * x3;  J[3][3] = 0.0;
}

static void newtonSolve3(void (*calcF)(const double[3], double[3]),
    void (*calcJ)(const double[3], double[3][3]),
    double x[3],
    double tol,
    int& iters)
{
    double f[3], Jmat[3][3], Jcopy[3][3], Jinv[3][3], dx[3], xNew[3];

    for (int k = 0; k < MAX_ITER; ++k) {
        calcF(x, f);
        calcJ(x, Jmat);

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                Jcopy[i][j] = Jmat[i][j];

        if (!invertMat3(Jcopy, Jinv)) { iters = -(k + 1); return; }

        matvec3(Jinv, f, dx);
        for (int i = 0; i < 3; ++i)
            xNew[i] = x[i] - dx[i];

        if (normInf3(xNew, x) < tol) {
            for (int i = 0; i < 3; ++i) x[i] = xNew[i];
            iters = k + 1;
            return;
        }
        for (int i = 0; i < 3; ++i) x[i] = xNew[i];
    }
    iters = -(MAX_ITER + 1);
}

static void newtonSolve4(void (*calcF)(const double[4], double[4]),
    void (*calcJ)(const double[4], double[4][4]),
    double x[4],
    double tol,
    int& iters)
{
    double f[4], Jmat[4][4], Jcopy[4][4], Jinv[4][4], dx[4], xNew[4];

    for (int k = 0; k < MAX_ITER; ++k) {
        calcF(x, f);
        calcJ(x, Jmat);

        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                Jcopy[i][j] = Jmat[i][j];

        if (!invertMat4(Jcopy, Jinv)) { iters = -(k + 1); return; }

        matvec4(Jinv, f, dx);
        for (int i = 0; i < 4; ++i)
            xNew[i] = x[i] - dx[i];

        if (normInf4(xNew, x) < tol) {
            for (int i = 0; i < 4; ++i) x[i] = xNew[i];
            iters = k + 1;
            return;
        }
        for (int i = 0; i < 4; ++i) x[i] = xNew[i];
    }
    iters = -(MAX_ITER + 1);
}

static void printResult3(const double x[3], int iters)
{
    std::cout << std::fixed << std::setprecision(15);
    if (iters > 0) {
        std::cout << "Solution found in " << iters << " iterations:\n"
            << "  X* = (" << x[0] << ", " << x[1] << ", " << x[2] << ")\n";
    }
    else if (iters == -(MAX_ITER + 1)) {
        std::cout << "Method did not converge after " << MAX_ITER << " iterations.\n"
            << "  Last approximation: ("
            << x[0] << ", " << x[1] << ", " << x[2] << ")\n";
    }
    else {
        std::cout << "Jacobian matrix is singular at iteration " << -iters << ".\n";
    }
}

static void printResult4(const double x[4], int iters)
{
    std::cout << std::fixed << std::setprecision(15);
    if (iters > 0) {
        std::cout << "Solution found in " << iters << " iterations:\n"
            << "  X* = (" << x[0] << ", " << x[1] << ", "
            << x[2] << ", " << x[3] << ")\n";
    }
    else if (iters == -(MAX_ITER + 1)) {
        std::cout << "Method did not converge after " << MAX_ITER << " iterations.\n"
            << "  Last approximation: ("
            << x[0] << ", " << x[1] << ", "
            << x[2] << ", " << x[3] << ")\n";
    }
    else {
        std::cout << "Jacobian matrix is singular at iteration " << -iters << ".\n";
    }
}

static void solveSystemA(int n)
{
    std::cout << "\n=== System (a), n = " << n << " ===\n";
    double x[3];
    std::cout << "Enter initial guess x1 x2 x3: ";
    std::cin >> x[0] >> x[1] >> x[2];
    int iters = 0;
    newtonSolve3(F_a, J_a, x, std::pow(10.0, -n), iters);
    printResult3(x, iters);
}

static void solveSystemB(int n)
{
    std::cout << "\n=== System (b), n = " << n << " ===\n";
    double x[3];
    std::cout << "Enter initial guess x1 x2 x3: ";
    std::cin >> x[0] >> x[1] >> x[2];
    int iters = 0;
    newtonSolve3(F_b, J_b, x, std::pow(10.0, -n), iters);
    printResult3(x, iters);
}

static void solveSystemC(int n)
{
    std::cout << "\n=== System (c), n = " << n << " ===\n";
    double x[4];
    std::cout << "Enter initial guess x1 x2 x3 x4: ";
    std::cin >> x[0] >> x[1] >> x[2] >> x[3];
    int iters = 0;
    newtonSolve4(F_c, J_c, x, std::pow(10.0, -n), iters);
    printResult4(x, iters);
}

int main()
{
    std::cout << "=== Newton's Method for Systems of Nonlinear Equations ===\n";

    char choice;
    std::cout << "Select system (a / b / c): ";
    std::cin >> choice;

    int n;
    std::cout << "Enter natural number n (precision 10^{-n}): ";
    std::cin >> n;
    if (n <= 0) {
        std::cerr << "Error: n must be a natural number.\n";
        return 1;
    }

    switch (choice) {
    case 'a': case 'A': solveSystemA(n); break;
    case 'b': case 'B': solveSystemB(n); break;
    case 'c': case 'C': solveSystemC(n); break;
    default:
        std::cerr << "Invalid system choice.\n";
        return 1;
    }
    return 0;
}
