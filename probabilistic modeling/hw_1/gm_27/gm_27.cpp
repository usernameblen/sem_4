//27. На отрезок ОА длины 1 числовой оси Ох наудачу
//поставлена точка В(х).Найти вероятность того, что
//меньший из отрезков ОВ и ВА имеет длину, большую,
//чем L / 3. Предполагается, что вероятность попадания
//точки на отрезок пропорциональна длине отрезка и не
//зависит от его расположения на числовой оси.

#include<iostream>
#include<random>
#include<algorithm>

#define L 1.0

int main()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, L);

    double const threshold = L / 3.0;

    int const trials = 10'000'000;

    long long hits = 0;

    for (int i = 0; i < trials; ++i)
    {
        double point = dist(gen);
        if (std::min(L - point, point) > threshold)
            ++hits;
    }

    std::cout << "result of " << trials << " test:" << std::endl
        << static_cast<double>(hits) / trials << std::endl;

    return 0;
}