#include <type_traits>
#include <random>
#include "random_uint64.h"

struct random_generator {
	random_generator(std::random_device rd)
		: gen(rd()) { }
	std::mt19937_64 mt_gen;
};

struct random_generator *new_random_generator() {
	std::random_device rd;
	random_generator *gen = new random_generator(rd);
	return gen;
}

std::uint64_t random_uint64(struct random_generator *gen) {
    std::uniform_int_distribution<unsigned long long> dist(
        std::numeric_limits<std::uint64_t>::min(),
        std::numeric_limits<std::uint64_t>::max());
	
}