#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "PrimeGenerator.h"
#include <chrono>

constexpr int MAX_BOUND = 100'000'000;

TEST_CASE("PrimeGenerator handles small bounds", "[basic]")
{
	PrimeGenerator gen5(5);
	const auto& primes5 = gen5.GetPrimes();
	REQUIRE(primes5.size() == 3);
	CHECK(primes5.count(2));
	CHECK(primes5.count(3));
	CHECK(primes5.count(5));
	CHECK(!primes5.count(4));
}

TEST_CASE("PrimeGenerator edge cases", "[edge]")
{
    CHECK(PrimeGenerator(1).GetPrimes().empty());
    CHECK(PrimeGenerator(2).GetPrimes() == std::set<int>{2});
    CHECK(PrimeGenerator(0).GetPrimes().empty());
    CHECK_THROWS_AS(PrimeGenerator(-1), std::invalid_argument);
    CHECK_THROWS_AS(PrimeGenerator(100'000'001), std::overflow_error);
}

// todo: выполнять тест только в релизе
TEST_CASE("PrimeGenerator correct count up to 100M", "[slow]")
{
	const auto start = std::chrono::high_resolution_clock::now();

    PrimeGenerator generator(MAX_BOUND);
	const auto& primes = generator.GetPrimes();

	const auto end = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<double> diff = end - start;

	CHECK(primes.size() == 5761455);
	CAPTURE(diff.count());
	CHECK(diff.count() < 15.0);
}