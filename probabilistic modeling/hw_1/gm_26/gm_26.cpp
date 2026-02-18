//26. На отрезке 1 длины 20 см помещен меньший отрезок / длины 10 см. 
// Найти вероятность того, что точка,
//наудачу поставленная на больший отрезок, попадет
//также и на меньший отрезок.Предполагается, 
//что вероятность попадания точки на отрезок пропорциональна
//длине отрезка и не зависит от его расположения.

#include <iostream>
#include <random>

int main() 
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 20.0);

    const int trials = 10'000'000;

    //меньший отрезок
    const double left = 0.0;
    const double right = 10.0;

    long long hits = 0;

    for (int i = 0; i < trials; ++i) 
    {
        double point = dist(gen);
        if (point >= left && point <= right) 
            ++hits;
    }

    std::cout << "result of " << trials << " test:" << std::endl
        << static_cast<double>(hits) / trials << std::endl;

    return 0;
}