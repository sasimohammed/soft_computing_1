#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include<vector>
#include "ga/operators/Replacement.h"

namespace ga {
    // Offspring fully replace the current population (eliteCount may be ignored or honored; document it).
    template<class Gene>
    class GenerationalReplacement : public Replacement<Gene> {
    public:


        // TODO: implement. Result size must equal targetSize.
        Population<Gene> replace(
            const Population<Gene>& currentpopulation, const Population<Gene>&offspring,
            const std::vector<double>&currentfitness, const std::vector<double>&offspringfitness,
            std::size_t targetsize, std::size_t elitecount, std::mt19937&rng) const override {
            if (offspring.size() < targetsize) throw std::invalid_argument("num of spring less target size");
            Population<Gene>newpopulation;
            for (int i = 0;i < targetsize;i++) newpopulation.push_back(offspring[i]);
            return newpopulation;
        }

        std::string name() const override { return "Generational"; }

    };
}
