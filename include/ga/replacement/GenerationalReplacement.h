#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Replacement.h"

namespace ga {
// Offspring fully replace the current population (eliteCount may be ignored or honored; document it).
template<class Gene>
class GenerationalReplacement : public Replacement<Gene> {
public:


    // TODO: implement. Result size must equal targetSize.
    Population<Gene> replace(
        const Population<Gene>&, const Population<Gene>&,
        const std::vector<double>&, const std::vector<double>&,
        std::size_t, std::size_t, std::mt19937&) const override {
        throw std::logic_error("GenerationalReplacement::replace TODO");
    }

    std::string name() const override { return "Generational"; }

};
}
