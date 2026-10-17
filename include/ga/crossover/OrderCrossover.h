#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Crossover.h"

namespace ga {
// Order crossover (OX) for permutations only; children must stay valid permutations.
template<class Gene>
class OrderCrossover : public Crossover<Gene> {
public:
    OrderCrossover() = default;

    // TODO: implement. Must return two VALID children.
    std::pair<Chromosome<Gene>, Chromosome<Gene>> cross(
        const Chromosome<Gene>&, const Chromosome<Gene>&,
        std::mt19937&) const override {
        throw std::logic_error("OrderCrossover::cross TODO");
    }

    std::string name() const override { return "Order (OX)"; }

};
}
