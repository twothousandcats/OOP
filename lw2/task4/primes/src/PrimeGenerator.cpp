#include "PrimeGenerator.h"
#include <algorithm>
#include <iostream>
constexpr int MAX_BOUND = 100'000'000;

PrimeGenerator::PrimeGenerator(const int upperBound)
	: m_upperBound(upperBound)
{
	if (upperBound < 0)
	{
		throw std::invalid_argument("Upper bound cannot be negative.");
	}

	if (upperBound > MAX_BOUND)
	{
		throw std::overflow_error("Upper bound exceeds maximum allowed value (100,000,000).");
	}
}

void PrimeGenerator::Generate()
{
	std::set<int> primes;
	if (m_upperBound < 2)
	{
		m_primesCache = primes;
		m_isPrimeVec.clear();
		return;
	}
	primes.insert(2);

	if (const int maxVal = m_upperBound; maxVal > 2)
	{
		const int limitIdx = (maxVal - 3) / 2;
		m_isPrimeVec.assign(limitIdx + 1, true);

		const int sqrtLimit = static_cast<int>(std::sqrt(maxVal));
		for (int i = 0; ; ++i)
		{
			const int p = 2 * i + 3;
			if (p > sqrtLimit)
			{
				break;
			}

			if (m_isPrimeVec[i])
			{
				const long long p_sq = 1LL * p * p;
				if (p_sq > maxVal)
				{
					continue;
				}

				const int startIdx = static_cast<int>((p_sq - 3) / 2);
				for (int j = startIdx; j <= limitIdx; j += p)
				{
					m_isPrimeVec[j] = false;
				}
			}
		}
	}
	for (size_t i = 0; i < m_isPrimeVec.size(); ++i)
	{
		if (m_isPrimeVec[i])
		{
			primes.insert(2 * static_cast<int>(i) + 3);
		}
	}

	m_primesCache = std::move(primes);
}

std::set<int>& PrimeGenerator::GetPrimes()
{
	if (m_primesCache.has_value())
	{
		return *m_primesCache;
	}

	Generate();
	return *m_primesCache;
}