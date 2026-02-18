//45. Ќаудачу вз€ты два положительных числа х и у,
// каждое из которых не превышает единицы. 
// Ќайти веро€тность того, что сумма х+у 
// не превышает единицы,
//а произведение ху не меньше 0, 09.

#include <iostream>
#include <random>
#include <cmath>

int main() {
    const double threshold_sum = 1.0;
    const double threshold_prod = 0.09;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    const long long trials = 10'000'000;
    long long hits = 0;

    for (long long i = 0; i < trials; ++i) {
        double x = dist(gen);
        double y = dist(gen);
        if (x + y <= threshold_sum && x * y >= threshold_prod) {
            ++hits;
        }
    }

    std::cout << "result of " << trials << " test:" << std::endl
        << static_cast<double>(hits) / trials << std::endl;

    return 0;
}