#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Mutation.h"

namespace ga {
// Reset genes to a random value in [min,max]. Integer / float / binary (0,1). Do NOT use on permutations.
template<class Gene>
class RandomResetMutation : public Mutation<Gene> {
public:
    RandomResetMutation(Gene minValue, Gene maxValue, double perGeneRate = 0.05)
        : min_(minValue), max_(maxValue), perGeneRate_(perGeneRate) {
        if (min_ > max_) throw std::invalid_argument("minValue > maxValue");
        if (perGeneRate_ < 0 || perGeneRate_ > 1)
            throw std::invalid_argument("perGeneRate must be in [0,1]");
    }

    // TODO: implement.
    void mutate(Chromosome<Gene>&, std::mt19937&) const override {
        throw std::logic_error("RandomResetMutation::mutate TODO");
    }

    std::string name() const override { return "RandomReset"; }
private:
    Gene min_, max_;
    double perGeneRate_;
};
}
