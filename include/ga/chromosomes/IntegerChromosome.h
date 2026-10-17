#pragma once
#include <cstddef>
#include <random>
#include <stdexcept>
#include "ga/Chromosome.h"

namespace ga {
using IntegerChromosome = Chromosome<int>;

// Genes in [minValue, maxValue]. If permutation == true the chromosome must be
// a permutation of minValue..maxValue (used for CVRP customer order).
class IntegerChromosomeSpec {
public:
    IntegerChromosomeSpec(std::size_t length, int minValue, int maxValue,
                          bool permutation = false)
        : length_(length), min_(minValue), max_(maxValue), permutation_(permutation) {
        if (length_ == 0) throw std::invalid_argument("length must be > 0");
        if (min_ > max_) throw std::invalid_argument("minValue > maxValue");
        if (permutation_ && static_cast<std::size_t>(max_ - min_ + 1) != length_)
            throw std::invalid_argument("permutation needs length == max-min+1");
    }
    std::size_t length() const { return length_; }
    int minValue() const { return min_; }
    int maxValue() const { return max_; }
    bool isPermutation() const { return permutation_; }

    // TODO: random chromosome (shuffle if permutation, else uniform in range).
    IntegerChromosome random(std::mt19937&) const {
        throw std::logic_error("IntegerChromosomeSpec::random TODO");
    }
    // TODO: length, bounds and (if permutation) uniqueness checks.
    bool isValid(const IntegerChromosome&) const {
        throw std::logic_error("IntegerChromosomeSpec::isValid TODO");
    }
private:
    std::size_t length_;
    int min_, max_;
    bool permutation_;
};
}
