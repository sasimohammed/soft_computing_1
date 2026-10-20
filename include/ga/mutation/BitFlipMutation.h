#pragma once
#include <cstddef>
#include <random>
#include <stdexcept>
#include <string>
#include "ga/operators/Mutation.h"

namespace ga {
template<class Gene>
class BitFlipMutation : public Mutation<Gene> {
public:
    explicit BitFlipMutation(double perGeneRate = 0.05) : perGeneRate_(perGeneRate) {
        if (perGeneRate_ < 0 || perGeneRate_ > 1)
            throw std::invalid_argument("perGeneRate must be in [0,1]");
    }

    void mutate(Chromosome<Gene>& c, std::mt19937& rng) const override {
        std::bernoulli_distribution flip(perGeneRate_);
        for (std::size_t i = 0; i < c.size(); ++i) {
            if (flip(rng)) c[i] = static_cast<Gene>(c[i] == 0 ? 1 : 0);
        }
    }

    std::string name() const override { return "BitFlip"; }
private:
    double perGeneRate_;
};
}