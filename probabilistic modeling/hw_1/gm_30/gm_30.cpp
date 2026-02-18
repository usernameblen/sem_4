//30. На плоскость с нанесенной сеткой квадратов со
//стороной а наудачу брошена монета радиуса < a / 2.
//	Найти вероятность того, что монета не пересечет ни одной
//	из сторон квадрата.Предполагается, что вероятность 
//	попадания точки в плоскую фигуру пропорциональна 
//	площади фигуры и не зависит от ее расположения.

#include <iostream>
#include <random>

int main() {
    const double a = 10.0;
    const double r = 3.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, a);

    const long long trials = 10'000'000;
    long long hits = 0;

    for (long long i = 0; i < trials; ++i) {
        double x = dist(gen);
        double y = dist(gen);
        //центр должен быть не ближе r к каждой стороне
        if (x >= r && x <= a - r && y >= r && y <= a - r) {
            ++hits;
        }
    }

    std::cout << "result of " << trials << " test:" << std::endl
        << static_cast<double>(hits) / trials << std::endl;

    return 0;
}