//29. Плоскость разграфлена параллельными прямыми,
//находящимися друг от друга на расстоянии 2а.На
//плоскость наудачу брошена монета радиуса га.Найти
//вероятность того, что монета не пересечет ни одной из
//прямых.

#include <iostream>
#include <random>
#include <cmath>

#define A 10.0
#define R 5.0

int main() {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-A, A);

    const long long trials = 10'000'000;
    long long hits = 0;

    for (long long i = 0; i < trials; ++i) 
    {
        double y = dist(gen);
        if (std::abs(y) < A - R) 
            ++hits;
    }

    std::cout << "result of " << trials << " test:" << std::endl
        << static_cast<double>(hits) / trials << std::endl;

    return 0;
}