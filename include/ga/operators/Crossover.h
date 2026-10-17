#pragma once
#include <random>
#include <string>
#include <utility>
#include "ga/Chromosome.h"

namespace ga {
template<class Gene>
class Crossover {
public:
    virtual ~Crossover() = default;
    virtual std::pair<Chromosome<Gene>, Chromosome<Gene>> cross(
        const Chromosome<Gene>& first,
        const Chromosome<Gene>& second,
        std::mt19937& rng) const = 0;
    virtual std::string name() const = 0;
};
}
