//28. В круг радиуса R помещен меньший круг
//радиуса г.Найти вероятность того, что точка, на удачу
//брошенная в большой круг, попадет также и в малый
//круг.Предполагается, что вероятность попадания точки
//в круг пропорциональна площади круга и не зависит
//от его расположения.

#define _USE_MATH_DEFINES
#include<iostream>
#include<random>
#include<cmath>

#define R 10.0
#define r 5.0

int main()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist_radius(0.0, 1.0);

    const long long trials = 10'000'000;
    long long hits = 0;

    for (long long i = 0; i < trials; ++i) {
        double u = dist_radius(gen);
        double radius = R * std::sqrt(u);
        if (radius <= r) {
            ++hits;
        }
    }

    std::cout << "result of " << trials << " test:" << std::endl
        << static_cast<double>(hits) / trials << std::endl;

    return 0;
}