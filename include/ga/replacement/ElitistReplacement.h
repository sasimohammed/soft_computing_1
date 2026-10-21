#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include "ga/operators/Replacement.h"

namespace ga {

    template<class Gene>
    class ElitistReplacement : public Replacement<Gene> {
    public:

        Population<Gene> replace(
            const Population<Gene>& currentPopulation,
            const Population<Gene>& offspring,
            const std::vector<double>& currentFitness,
            const std::vector<double>& offspringFitness,
            std::size_t targetSize,
            std::size_t eliteCount,
            std::mt19937& rng) const override
        {
      

            if (eliteCount > targetSize)
                throw std::invalid_argument("Elite count exceeds target size");

            if (eliteCount > currentPopulation.size())
                throw std::invalid_argument("Not enough individuals for elitism");

            std::size_t remaining = targetSize - eliteCount;

            if (remaining > offspring.size())
                throw std::invalid_argument("Not enough offspring");

            Population<Gene> newPopulation;

           
            std::vector<std::size_t> indices(currentPopulation.size());

            std::iota(indices.begin(), indices.end(), 0);

            std::sort(indices.begin(), indices.end(),
                [&](std::size_t a, std::size_t b)
                {
                    return currentFitness[a] > currentFitness[b];
                });

        
            for (std::size_t i = 0; i < eliteCount; i++)
            {
                newPopulation.push_back(currentPopulation[indices[i]]);
            }

           
            std::vector<std::size_t> offspringIndices(offspring.size());

            std::iota(offspringIndices.begin(), offspringIndices.end(), 0);

            std::sort(offspringIndices.begin(), offspringIndices.end(),
                [&](std::size_t a, std::size_t b)
                {
                    return offspringFitness[a] > offspringFitness[b];
                });

            for (std::size_t i = 0; i < remaining; i++)
            {
                newPopulation.push_back(offspring[offspringIndices[i]]);
            }

            return newPopulation;
        }

        std::string name() const override {
            return "Elitist";
        }
    };

}
