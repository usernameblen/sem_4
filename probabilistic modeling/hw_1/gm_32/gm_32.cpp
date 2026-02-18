//32. Ќа плоскости начерчены две концентрические
//окружности, радиусы которых 5 и 10 см соответственно.
//Ќайти веро€тность того, что точка, брошенна€ наудачу
//в большой круг, попадет также и в кольцо, 
//образованноепостроенными окружност€ми.
//ѕредполагаетс€, что веро€тность попадани€ точки 
//в плоскую фигуру пропорциональна площади этой фигуры 
//и не зависит от ее расположени€.

#include <iostream>
#include <random>
#include <cmath>

int main() {
    const double R_big = 10.0;
    const double R_small = 5.0; 

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist_u(0.0, 1.0);

    const long long trials = 10'000'000;
    long long hits = 0;

    for (long long i = 0; i < trials; ++i) {
        double u = dist_u(gen);
        double r = R_big * std::sqrt(u);
        if (r >= R_small && r <= R_big) {
            ++hits;
        }
    }

    std::cout << "result of " << trials << " test:" << std::endl
        << static_cast<double>(hits) / trials << std::endl;


    return 0;
}