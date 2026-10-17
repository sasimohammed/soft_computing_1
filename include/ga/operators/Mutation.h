#pragma once
#include <random>
#include <string>
#include "ga/Chromosome.h"

namespace ga {
template<class Gene>
class Mutation {
public:
    virtual ~Mutation() = default;
    // The GA decides WHETHER to mutate (mutationRate); this decides HOW.
    virtual void mutate(Chromosome<Gene>& chromosome, std::mt19937& rng) const = 0;
    virtual std::string name() const = 0;
};
}
