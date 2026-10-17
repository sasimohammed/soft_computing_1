#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Crossover.h"

namespace ga {
// k-point crossover for binary / integer / float. NOT valid for permutations (use OrderCrossover). Parents must have equal length > points.
template<class Gene>
class MultiPointCrossover : public Crossover<Gene> {
public:
    explicit MultiPointCrossover(std::size_t points = 2) : points_(points) {
        if (points_ == 0) throw std::invalid_argument("points must be > 0");
    }

    // TODO: implement. Must return two VALID children.
    std::pair<Chromosome<Gene>, Chromosome<Gene>> cross(
        const Chromosome<Gene>&, const Chromosome<Gene>&,
        std::mt19937&) const override {
        throw std::logic_error("MultiPointCrossover::cross TODO");
    }

    std::string name() const override { return "MultiPoint"; }
private:
    std::size_t points_;
};
}
