#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <cmath>
#include <stdexcept>


struct probabilities {
    double first_even;
    double second_even;
    double both_even;
};


probabilities analyticalA(int N) {
    double count_of_even = N / 2;
    probabilities p;
    p.first_even = count_of_even / N;
    p.second_even = count_of_even / N;
    p.both_even = count_of_even * (count_of_even - 1) / (static_cast<double>(N) * (N - 1));
    return p;
}


probabilities analyticalB(int N) 
{
    int count_of_even = N / 2;
    probabilities p;
    p.first_even = static_cast<double>(count_of_even) / N;

    if (N % 2 == 0) 
    {
        p.second_even = static_cast<double>(N - 2) / (2.0 * (N - 1)); //(N/2 - 1) / (N - 1)
        p.both_even = static_cast<double>(N - 2) / (4.0 * (N - 1));//  ((N/2) * (N/2 - 1))/N*(N-1)
    }
    else 
    {
        p.second_even = static_cast<double>(count_of_even) / (N - 1);
        p.both_even = static_cast<double>(N - 1) / (4.0 * N); // ((N - 1)/2) * ((N - 1)/2)/(N * (N - 1))
    }
    return p;
}


void printTable(const probabilities& theory,
    const probabilities& empiric)
{
    
    std::cout << "First even: theor. " << theory.first_even
        << ", empiric. " << empiric.first_even
        << ", difference " << (empiric.first_even - theory.first_even) << std::endl;
    std::cout << "Second even: theor. " << theory.second_even
        << ", empiric. " << empiric.second_even
        << ", difference " << (empiric.second_even - theory.second_even) << std::endl;
    std::cout << "Both are even: theoretical. " << theory.both_even
        << ", empiric. " << empiric.both_even
        << ", difference " << (empiric.both_even - theory.both_even) << std::endl << std::endl;
}


int main(int argc, char* argv[]) 
{
    if (argc != 3) {
        std::cout << "Not enough args: " << std::endl
        << "  N - alphabet power (integer, N >= 2)" << std::endl
            << "  C - number of experiments (integer, C > 0)" << std::endl;
        return 1;
    }

    int N, C;
    try {
        N = std::stoi(argv[1]);
        C = std::stoi(argv[2]);
    }
    catch (const std::exception&) {
        std::cerr << "Error: N and C must be integers" << std::endl;
        return 1;
    }

    if (N < 2) {
        std::cerr << "Error: N must be >= 2" << std::endl;
        return 1;
    }
    if (C <= 0) {
        std::cerr << "Error: C must be > 0" << std::endl;
        return 1;
    }


    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distN(1, N);
    std::uniform_int_distribution<int> distN1(1, N - 1);

    long long A_first = 0, A_second = 0, A_both = 0; // вар А
    long long B_first = 0, B_second = 0, B_both = 0; // вар Б


    for (int i = 0; i < C; ++i) 
    {
        int first = distN(rng);
        int raw = distN1(rng);
        int second = (raw >= first) ? raw + 1 : raw;

        //вар А: индексы не меняются
        bool a_first_even = (first % 2 == 0);
        bool a_second_even = (second % 2 == 0);

        if (a_first_even)              
            A_first++;

        if (a_second_even)             
            A_second++;

        if (a_first_even && a_second_even) 
            A_both++;

        //вар Б: перенумерация после первого выбора
        int new_second = (second < first) ? second : second - 1;

        bool b_first_even = (first % 2 == 0);
        bool b_second_even = (new_second % 2 == 0);

        if (b_first_even)              
            B_first++;

        if (b_second_even)             
            B_second++;

        if (b_first_even && b_second_even) 
            B_both++;
    }

    double dC = static_cast<double>(C);
    probabilities empA = { A_first / dC, A_second / dC, A_both / dC };
    probabilities empB = { B_first / dC, B_second / dC, B_both / dC };

    probabilities thA = analyticalA(N);
    probabilities thB = analyticalB(N);

    std::cout << "\nN = " << N << ", C = " << C << std::endl << std::endl;
    std::cout << "Option A (indices do not change): "<< std::endl;
    printTable(thA, empA);
    std::cout << "Option B (indices change):" << std::endl;
    printTable(thB, empB);

    return 0;
}