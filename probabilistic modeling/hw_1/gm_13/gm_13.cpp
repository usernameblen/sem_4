//13. В конверте среди 100 фотокарточек находится
//одна разыскиваемая.Из конверта наудачу извлечены 10карточек.
// Найти вероятность того, что среди них окажется нужная.

#include<vector>
#include<iostream>
#include<algorithm>
#include<random>
#include<iomanip>

int main()
{
	int const total = 100;
	int const needed = 1;
	int const draw = 10;
	long long const trials = 1'000'000;

	std::vector<int> cards(total, 0);
	cards[0] = 1;

	std::random_device rd;
	std::mt19937 rng(rd());

	long long success = 0;

	for (long long i = 0; i < trials; ++i)
	{
		std::shuffle(cards.begin(), cards.end(), rng);
		for (int j = 0; j < draw; ++j)
		{
			if (cards[j] == 1)
			{
				++success;
				break;
			}
		}
	}

	double empirical = static_cast<double>(success) / trials;

	std::cout << std::fixed << std::setprecision(6);
	std::cout << "result of " << trials << " test:" << empirical << std::endl;

	return 0;
}