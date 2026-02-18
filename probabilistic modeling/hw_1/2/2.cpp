#include <iostream>
#include <iomanip>
#include <vector>
#include <numeric> 
#include <algorithm> 
#include <random>
#include <string>
#include <stdexcept>

int main(int argc, char* argv[]) 
{
    if (argc != 3) 
    {
        std::cout << "Not enough args: " << std::endl
            << "  N - set size (integer, N >= 2)" << std::endl
            << "  C - number of experiments (integer, C > 0)" << std::endl;
        return 1;
    }

    int N, C;
    try 
    {
        N = std::stoi(argv[1]);
        C = std::stoi(argv[2]);
    }
    catch (const std::exception&) 
    {
        std::cout << "Error: N and C must be integers" << std::endl;
        return 1;
    }

    if (N < 2)
    {
        std::cout << "Error: N must be >= 2" << std::endl;
        return 1;
    }
    if (C <= 0) 
    { 
        std::cout << "Error: C must be > 0" << std::endl;
        return 1; 
    }

    const int idx_i = 4;
    const int idx_j = 1;

    if (idx_i >= N || idx_j >= N)
    {
        std::cout << "Error: i and j must ba < N" << std::endl;
        return 1;
    }

    // P(A_i) = P(A_j) = (N-1)!/N!
    // P(A_i * A_j) = (N-2)!/N!
    // P(A_i + A_j) = P(A_i) + P(A_j) - P(A_i * A_j)  Ч теорема сложени€
    double th_i = 1.0 / N;
    double th_j = 1.0 / N;
    double th_ij = 1.0 / (static_cast<double>(N) * (N - 1));
    double th_iORj = th_i + th_j - th_ij;

    std::random_device rd;
    std::mt19937 rng(rd());

    std::vector<int> perm(N);
    std::iota(perm.begin(), perm.end(), 0);


    long long cnt_i = 0;
    long long cnt_j = 0;
    long long cnt_ij = 0;
    long long cnt_iORj = 0;

    for (int exp = 0; exp < C; ++exp) 
    {
        std::shuffle(perm.begin(), perm.end(), rng);

        bool ai = (perm[idx_i] == idx_i);
        bool aj = (perm[idx_j] == idx_j);

        if (ai)
            cnt_i++;
        if (aj)
            cnt_j++;
        if (ai && aj) 
            cnt_ij++;
        if (ai || aj) 
            cnt_iORj++;
    }

    double dC = static_cast<double>(C);
    double em_i = cnt_i / dC;
    double em_j = cnt_j / dC;
    double em_ij = cnt_ij / dC;
    double em_iORj = cnt_iORj / dC;
    double em_rhs = em_i + em_j - em_ij; //права€ часть т

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "N = " << N << ", C = " << C << ", i =  "<< idx_i + 1 <<", j = " << idx_j + 1 << std::endl;

    std::cout << "\nTheoretical probabilities:" << std::endl;
    std::cout << "  P(A1) = " << th_i << std::endl;
    std::cout << "  P(A2) = " << th_j << std::endl;
    std::cout << "  P(A1 n A2) = " << th_ij << std::endl;
    std::cout << "  P(A1 u A2) = " << th_iORj
        << "  [= P(A1) + P(A2) - P(A1 n A2)]" << std::endl;

    std::cout << "\nEmpirical probabilities:" << std::endl;
    std::cout << "  P(A1) = " << em_i << std::endl;
    std::cout << "  P(A2) = " << em_j << std::endl;
    std::cout << "  P(A1 n A2) = " << em_ij << std::endl;
    std::cout << "  P(A1 u A2) = " << em_iORj
        << "  [= P(A1) + P(A2) - P(A1 n A2)]" << std::endl;

    std::cout << "\nCheck addition rule  P(A1 u A2) = P(A1) + P(A2) - P(A1 n A2):" << std::endl;
    std::cout << "  Left  side  [P(A1 u A2)] = "
        << em_iORj << std::endl;
    std::cout << "  Right side  [P(A1) + P(A2) - P(A1 n A2)] = "
        << em_rhs << std::endl;
    std::cout << "  Difference = "
        << (em_iORj - em_rhs) << std::noshowpos << std::endl;

    return 0;
}