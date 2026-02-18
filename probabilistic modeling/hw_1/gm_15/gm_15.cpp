//15. Устройство состоит из пяти элементов, из которых
//два изношены.При включении устройства включаются случайным 
// образом два элемента.Найти вероятность того,
//что включенными окажутся неизношенные элементы.

#include<vector>
#include<random>
#include<iostream>
#include<iomanip>

int main()
{
	int const total = 5;
	int const worn = 2;
	int const good = total - worn;
	int const choose = 2;
	long long const trials = 1'000'000;

	std::vector<int> elements(total, 1);
	for (int i = 0; i < worn; ++i)
		elements[i] = 0;

	std::random_device rd;
	std::mt19937 rng(rd());

	long long success = 0;

	for (long long i = 0; i < trials; ++i)
	{
		std::shuffle(elements.begin(), elements.end(), rng);
		if (elements[0] == 1 && elements[1] == 1)
			++success;
	}

	double empirical = static_cast<double>(success) / trials;

	std::cout << std::fixed << std::setprecision(6);
	std::cout << "result of " << trials << " test: " << empirical << std::endl;


	return 0;
}