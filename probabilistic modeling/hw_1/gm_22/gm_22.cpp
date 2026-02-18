//22. В «секретном» замке на общей оси четыре диска,
//каждый из которых разделен на пять секторов, 
//на которых написаны различные цифры.Замок открывается
//только в том случае, если диски установлены так, что
//цифры на них составляют определенное четырехзначное
//число.Найти вероятность того, что при произвольной
// установке дисков замок будет открыт.

#include <iostream>
#include <random>

int main() 
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 4); 

    const int trials = 10'000'000;
    const int target[4] = { 0, 0, 0, 0 }; 
    long long success = 0;

    for (int i = 0; i < trials; ++i) 
    {
        int match = 0;
        for (int j = 0; j < 4; ++j) 
        {
            if (dist(gen) == target[j]) 
                ++match;
        }
        if (match == 4) ++success;
    }

    std::cout << "result of " << trials << " test:" << std::endl
        << double(success) / trials << std::endl;

    return 0;
}