#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Selection.h"
#include<vector>
namespace ga {
    // Fitness-proportionate. TODO: handle negative/zero fitness (shift or rank-based). Weak pressure if fitness values are all similar (e.g. 1/(1+distance)).
    template<class Gene>
    class RouletteSelection : public Selection<Gene> {
    public:
        RouletteSelection() = default;

        // TODO: implement.
        Chromosome<Gene> select(const Population<Gene>& populations, const std::vector<double>& fitness,
            std::mt19937&  rng) const override {
            if (populations.empty()) throw std::invalid_argument("population is empty");
             if (populations.size() != fitness.size()) throw std::invalid_argument("fitness size must match population size");
             if (populations.size() == 1) return populations[0];
             double sum_fitness = 0;
             for (double fitness_value:  fitness)
             {
                 sum_fitness += fitness_value;
             }
             std::vector<double>probabiltes;

             for (int i = 0;i < fitness.size();i++) {
                 probabiltes.push_back( (  fitness[i] / sum_fitness)  );
             }
             std::uniform_real_distribution<double>dist(0.0, 1.0);
             double random_value = dist(rng);
             double end_rang = 0.0;
             for (int i = 0;i < probabiltes.size();i++)
             {
                     end_rang += probabiltes[i];
                     if (i == probabiltes.size() - 1) end_rang = 1.0;
                     if (random_value < end_rang) return(populations[i]);
             }
             throw std::runtime_error("roulette failed");
        }

        std::string name() const override { return "Roulette"; }

    };
}

