#pragma once
#include <random>
#include <string>
#include <vector>
#include "ga/Chromosome.h"

namespace ga {
template<class Gene>
class Selection {
public:
    virtual ~Selection() = default;
    // Returns a COPY of the selected parent. fitness[i] belongs to population[i].
    virtual Chromosome<Gene> select(
        const Population<Gene>& population,
        const std::vector<double>& fitness,
        std::mt19937& rng) const = 0;
    virtual std::string name() const = 0;
};
}
