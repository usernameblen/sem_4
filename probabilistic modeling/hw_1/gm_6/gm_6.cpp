//куб разбит на 1000 кубиков (10x10x10):
//неокрашенные кубики: 8х8х8 = 512
//окрашены: 
//   (3) угловые: 8
//   (2) ребро( -угловые ): 12*8 = 96
//   (1) грани ( -ребра ): 6 * (8 * 8) = 384

#include<iostream>
#include<cmath>
#include <vector>
#include<random>
#include <chrono>

#define LEN 10
#define CUBES 1000U

int main()
{
	int count_unpainted = pow((LEN - 2), 3);
	int count3 = 8; //угловые
	int count2 = 12 * (LEN - 2); //ребра (- угловые)
	int count1 = 6 * (pow(LEN - 2, 2)); // грани (-ребра)

	std::cout << "number of painted and unpainted cubes:" << std::endl;
	std::cout << "unpainted: " << count_unpainted << std::endl //8x8x8
		<< "painted:" << std::endl
		<< "	(3 faces): " << count3 << std::endl //угловые
		<< "	(2 faces): " << count2 << std::endl //ребра - угловые
		<< "	(1 faces): " << count1 << std::endl << std::endl;

	if(CUBES == count_unpainted + count1 + count2 + count3)
		std::cout << "==> " << CUBES << " = " << count_unpainted + count1 + count2 + count3 << std::endl;



	std::vector<int> cubes(CUBES);
	int index = 0;
	for (int i = 0; i < count3; ++i) cubes[index++] = 3;
	for (int i = 0; i < count2; ++i) cubes[index++] = 2;
	for (int i = 0; i < count1; ++i) cubes[index++] = 1;
	for (int i = 0; i < count_unpainted; ++i) cubes[index++] = 0;

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> dist(0, CUBES - 1);

	long long const trials = 1'000'000;
	long long c1 = 0, c2 = 0, c3 = 0;

	for (long long i = 0; i < trials; ++i)
	{
		int cube = cubes[dist(rng)];
		if (cube == 1) ++c1;
		else if (cube == 2) ++c2;
		else if (cube == 3) ++c3;
	}

	// Теоретические вероятности
	double theor1 = static_cast<double>(count1) / CUBES;
	double theor2 = static_cast<double>(count2) / CUBES;
	double theor3 = static_cast<double>(count3) / CUBES;

	std::cout << "results (" << trials << " trials):\n";
	std::cout << "one painted border: " << (double)c1 / trials
		<< " (theoretically " << theor1 << ")" << std::endl;
	std::cout << "two painted border: " << (double)c2 / trials
		<< " (theoretically " << theor2 << ")" << std::endl;;
	std::cout << "three painted border: " << (double)c3 / trials
		<< " (theoretically " << theor3 << ")" << std::endl;;

	return 0;
}