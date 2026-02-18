
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <stdexcept>
#include <cmath>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Not enough args: " << std::endl 
            << " k  (integer k >= 2)" << std::endl;
        return 1;
    }

    int k;
    try 
    {
        k = std::stoi(argv[1]);
    }
    catch (const std::exception&) 
    {
        std::cout << "Error: k must be an integer" << std::endl;
        return 1;
    }

    if (k < 2) {
        std::cerr << "Error: k must be >= 2" << std::endl;
        return 1;
    }

    // аналитическая вер
    // P(length < k) = 1 - 1/2^{k-2} считаем обратный случай тип чередуются opopopop..
    //=> 1 * 0,5 * 0,5 ... = 0,5^(k-2): -1: умножение на единицу в начале, 
                                      //-1: неважно что выпадет последним броском
    double th_before_k = 1.0 - std::pow(0.5, k - 2);

    //1/2 + 1/2^2 + 1/2^3 ... (2 одинак, 3 одинак и тд)
    //четные: 1/2 + 1/2^3 + 1/2^5 ... - геом прогрессия: (1/2)/(1 - (1/4)) = 2/3
    double th_even = 2.0 / 3.0;

    const int trials = 1'000'000;
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> coin(0, 1);

    long long cnt_before_k = 0;
    long long cnt_even = 0;

    for (int i = 0; i < trials; ++i) 
    {
        int prev = coin(rng);
        int len = 1;

        while (true) 
        {
            int cur = coin(rng);
            ++len;
            if (cur == prev) break;
            prev = cur;
        }
        if (len < k)
            ++cnt_before_k;
        if (len % 2 == 0)
            ++cnt_even;
    }

    double em_before_k = static_cast<double>(cnt_before_k) / trials;
    double em_even = static_cast<double>(cnt_even) / trials;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "k = " << k << ", experiments = " << trials << std::endl;

    std::cout << "Theoretical probabilities:" << std::endl;
    std::cout << "  P(end before k) = " << th_before_k << std::endl;
    std::cout << "  P(even length)   = " << th_even << std::endl;

    std::cout << "Empirical probabilities:\n";
    std::cout << "  P(end before k) = " << em_before_k << std::endl;
    std::cout << "  P(even length)   = " << em_even << std::endl;

    std::cout << "Differences:" << std::endl;
    std::cout << "  P(end before k): " << (em_before_k - th_before_k) << std::endl;
    std::cout << "  P(even length):   " << (em_even - th_even) << std::endl;

    return 0;
}