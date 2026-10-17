#pragma once
#include <cstddef>
#include <random>
#include <string>
#include <vector>
#include "ga/Chromosome.h"

namespace ga {
template<class Gene>
class Replacement {
public:
    virtual ~Replacement() = default;
    // Builds the next generation (size == targetSize).
    virtual Population<Gene> replace(
        const Population<Gene>& current,
        const Population<Gene>& offspring,
        const std::vector<double>& currentFitness,
        const std::vector<double>& offspringFitness,
        std::size_t targetSize,
        std::size_t eliteCount,
        std::mt19937& rng) const = 0;
    virtual std::string name() const = 0;
};
}
