#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Replacement.h"

namespace ga {
// Keep the best eliteCount of the current population, fill the rest with the best offspring.
template<class Gene>
class ElitistReplacement : public Replacement<Gene> {
public:


    // TODO: implement. Result size must equal targetSize.
    Population<Gene> replace(
        const Population<Gene>&, const Population<Gene>&,
        const std::vector<double>&, const std::vector<double>&,
        std::size_t, std::size_t, std::mt19937&) const override {
        throw std::logic_error("ElitistReplacement::replace TODO");
    }

    std::string name() const override { return "Elitist"; }

};
}
