#pragma once
#include <set>
#include <vector>
#include <optional>

class PrimeGenerator {
public:
    explicit PrimeGenerator(int upperBound);

    std::set<int> &GetPrimes();

    // todo: public содержит публичные только методы
    // todo: придумать как протестиирвать функции6 используемые внутри GetPrimes
private:
    void Generate();

    int m_upperBound;

    // memory opt
    std::vector<bool> m_isPrimeVec; // fast checking IsPrime
    std::optional<std::set<int> > m_primesCache; // caching primes while obj is existed
};
