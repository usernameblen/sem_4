//¬ коробке шесть одинаковых, занумерованных кубиков. 
// Ќаудачу по одному извлекают все кубики. 
// Ќайти веро€тность того, что номера извлеченных кубиков 
// по€в€тс€ в возрастающем пор€дке.

#include<vector>
#include<iostream>
#include<random>
#include<algorithm>
#include <iomanip>

int main()
{
	int n = 6;
	std::vector<int> cubes(n);
	for (int i = 0; i < n; ++i) cubes[i] = i + 1; //нумераци€

	std::random_device rd;
	std::mt19937 rng(rd());

	long long const trials = 1'000'000;
	long long success = 0;

	for (long long i = 0; i < trials; ++i)
	{
		std::shuffle(cubes.begin(), cubes.end(), rng);

		bool ascending = true;
		for (int j = 1; j < n; ++j)
		{
			if (cubes[j] < cubes[j - 1])
			{
				ascending = false;
				break;
			}
		}
		if (ascending) ++success;
	}

	double empirical = static_cast<double>(success) / trials;
	double theoretical = 1.0 / (1 * 2 * 3 * 4 * 5 * 6); 

	std::cout << std::fixed << std::setprecision(7);
	std::cout << "theoretically: " << theoretical << std::endl;
	std::cout << "result: " << empirical << '\n';
	std::cout << "count of success: " << success << " out of " << trials << '\n';

	return 0;
}