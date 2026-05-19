#include <random.h>
#include <random>
#include <ctime>

// Random Number Generator
std::mt19937 gen32;

// Initialise random number generator
void seed() {
    gen32.seed(time(nullptr));
}

// Generate a uniform random number
float uniform(float minInclusive, float maxExclusive) {
    std::uniform_real_distribution<> dis(minInclusive, maxExclusive);
    return dis(gen32);
}