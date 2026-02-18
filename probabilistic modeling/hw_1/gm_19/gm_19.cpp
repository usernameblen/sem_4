//19. На складе имеется 15 кинескопов, причем 10 из
//них изготовлены Львовским заводом.Найти вероятность
//того, что среди пяти взятых наудачу кинескопов 
// окажутся три кинескопа Львовского завода.

#include<vector>
#include<random>
#include<iostream>
#include<iomanip>

int main()
{
    int const total = 15;
    int const lvov = 10;
    int const other = total - lvov;
    int const take = 5;
    int const target = 3;
    long long const trials = 1'000'000; 


    std::vector<int> kinescopes(total, 1);
    for (int i = lvov; i < total; ++i) 
        kinescopes[i] = 0;


    std::random_device rd;
    std::mt19937 rng(rd());

    long long success = 0;

    for (long long i = 0; i < trials; ++i) 
    {
        std::shuffle(kinescopes.begin(), kinescopes.end(), rng);
        int count = 0;
        for (int j = 0; j < take; ++j) 
        {
            if (kinescopes[j] == 1) ++count;
        }
        if (count == target) 
            ++success;
    }

    double empirical = static_cast<double>(success) / trials;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "result of " << trials << " test:" << empirical << std::endl;


	return 0;
}