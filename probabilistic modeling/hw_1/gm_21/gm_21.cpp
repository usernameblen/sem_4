//21. В коробке пять одинаковых изделий, причем три
//из них окрашены.Наудачу извлечены два изделия.
//Найти вероятность того, что среди двух извлеченных
//изделий окажутся : а) одно окрашенное изделие; б) два
//окрашенных изделия; в) хотя бы одно окрашенноеизделие.

#include<iostream>
#include<random>

int main()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 4);

    const int total = 5; // 0, 1, 2 - окрашены
    const int trials = 1000000;

    long long count0 = 0, count1 = 0, count2 = 0; //a, б, в

    for (int i = 0; i < trials; ++i)
    {
        int idx1 = dist(gen);
        int idx2;
        do {
            idx2 = dist(gen);
        } while (idx2 == idx1);

        if ((idx1 <= 2 && idx2 > 2) || (idx2 <= 2 && idx1 > 2))
        {
            ++count0;
            ++count2;
        }

        else if (idx1 <= 2 && idx2 <= 2)
        {
            ++count1;
            ++count2;
        }
    }

    std::cout << "result of " << trials << " test:" << std::endl;
    std::cout << "a) one colored: " << double(count0) / trials << "\n";
    std::cout << "b) two colored:  " << double(count1) / trials << "\n";
    std::cout << "c) at least one:    " << double(count2) / trials << "\n";


    return 0;
}