#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Selection.h"

namespace ga {
// Fitness-proportionate. TODO: handle negative/zero fitness (shift or rank-based). Weak pressure if fitness values are all similar (e.g. 1/(1+distance)).
template<class Gene>
class RouletteSelection : public Selection<Gene> {
public:
    RouletteSelection() = default;

    // TODO: implement.
    Chromosome<Gene> select(const Population<Gene>&, const std::vector<double>&,
                            std::mt19937&) const override {
        throw std::logic_error("RouletteSelection::select TODO");
    }

    std::string name() const override { return "Roulette"; }

};
}
