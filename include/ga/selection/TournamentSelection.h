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
    Chromosome<Gene> select(const Population<Gene>&, const std::vector<double>&,
                            std::mt19937&) const override {
        throw std::logic_error("TournamentSelection::select TODO");
    }

    std::string name() const override { return "Tournament"; }
private:
    std::size_t tournamentSize_;
};
}
