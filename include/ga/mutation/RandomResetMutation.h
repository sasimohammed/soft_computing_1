#pragma once
#include <cstddef>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include "ga/operators/Mutation.h"

namespace ga {
template<class Gene>
class RandomResetMutation : public Mutation<Gene> {
public:
    RandomResetMutation(Gene minValue, Gene maxValue, double perGeneRate = 0.05)
        : min_(minValue), max_(maxValue), perGeneRate_(perGeneRate) {
        if (min_ > max_) throw std::invalid_argument("minValue > maxValue");
        if (perGeneRate_ < 0 || perGeneRate_ > 1)
            throw std::invalid_argument("perGeneRate must be in [0,1]");
    }

    void mutate(Chromosome<Gene>& c, std::mt19937& rng) const override {
        std::bernoulli_distribution apply(perGeneRate_);
        if constexpr (std::is_integral<Gene>::value) {
            std::uniform_int_distribution<Gene> value(min_, max_);
            for (std::size_t i = 0; i < c.size(); ++i) {
                if (apply(rng)) c[i] = value(rng);
            }
        } else {
            std::uniform_real_distribution<Gene> value(min_, max_);
            for (std::size_t i = 0; i < c.size(); ++i) {
                if (apply(rng)) c[i] = value(rng);
            }
        }
    }

    std::string name() const override { return "RandomReset"; }
private:
    Gene min_, max_;
    double perGeneRate_;
};
}