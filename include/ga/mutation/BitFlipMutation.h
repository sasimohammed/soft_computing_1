#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Mutation.h"

namespace ga {
// Binary chromosomes only: flip each gene 0 <-> 1 with probability perGeneRate.
template<class Gene>
class BitFlipMutation : public Mutation<Gene> {
public:
    explicit BitFlipMutation(double perGeneRate = 0.05) : perGeneRate_(perGeneRate) {
        if (perGeneRate_ < 0 || perGeneRate_ > 1)
            throw std::invalid_argument("perGeneRate must be in [0,1]");
    }

    // TODO: implement.
    void mutate(Chromosome<Gene>&, std::mt19937&) const override {
        throw std::logic_error("BitFlipMutation::mutate TODO");
    }

    std::string name() const override { return "BitFlip"; }
private:
    double perGeneRate_;
};
}
