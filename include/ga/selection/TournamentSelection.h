#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Selection.h"

namespace ga {
    // Pick tournamentSize random individuals, return the fittest. Works on any representation.
    template<class Gene>
    class TournamentSelection : public Selection<Gene> {
    public:
        explicit TournamentSelection(std::size_t tournamentSize = 3)
            : tournamentSize_(tournamentSize) {
            if (tournamentSize_ == 0) throw std::invalid_argument("tournamentSize must be > 0");
        }

        // TODO: implement.
        Chromosome<Gene> select(const Population<Gene>&populations, const std::vector<double>& fitness,
            std::mt19937& rng) const override {
            if (populations.empty()) std::invalid_argument("population is empty");
            if (population.size() != fitness.size()) std::invalid_argument("fitness size must match population size");
            std::uniform_int_distribution<std::size_t>dist(0, populations.size() - 1);
                std::size_t best_index = dist(rng);
            for (int i = 1;i < tournamentSize_;i++) {
                std::size_t current_index = dist(rng);
                if (fitness[current_index] > fitness[best_index]) best_index = current_index;

            }
            return populations[best_index];
        }

        std::string name() const override { return "Tournament"; }
    private:
        std::size_t tournamentSize_;
    };
}
