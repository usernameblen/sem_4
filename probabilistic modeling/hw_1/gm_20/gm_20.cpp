//20. В группе 12 студентов, среди которых 8 отличников. 
// По списку наудачу отобраны 9 студентов. Найти
//вероятность того, что среди отобранных студентов пять
//Отличников.

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

int main() {
    const int total = 12;
    const int excellent = 8;
    const int needed = 9;
    const int target = 5;

    std::random_device rd;
    std::mt19937 gen(rd());

    long long trials = 1'000'000;
    long long success = 0;

    std::vector<int> students(total);
    for (int i = 0; i < total; ++i) students[i] = i;

    for (long long i = 0; i < trials; ++i) 
    {
        std::shuffle(students.begin(), students.end(), gen);
        int count = 0;
        for (int j = 0; j < needed; ++j) {
            if (students[j] < excellent) ++count;
        }
        if (count == target) ++success;
    }

    std::cout << "result of " << trials << " test:" << std::endl 
        << double(success) / trials << std::endl;

    return 0;
}