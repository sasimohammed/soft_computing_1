#pragma once
#include <cstddef>
#include <random>
#include <stdexcept>
#include "ga/Chromosome.h"

namespace ga {
using FloatChromosome = Chromosome<double>;

// Genes in [lower, upper].
class FloatChromosomeSpec {
public:
    FloatChromosomeSpec(std::size_t length, double lower, double upper)
        : length_(length), lower_(lower), upper_(upper) {
        if (length_ == 0) throw std::invalid_argument("length must be > 0");
        if (lower_ >= upper_) throw std::invalid_argument("lower must be < upper");
    }
    std::size_t length() const { return length_; }
    double lower() const { return lower_; }
    double upper() const { return upper_; }

    // TODO: uniform random genes in [lower, upper].
    FloatChromosome random(std::mt19937&) const {
        throw std::logic_error("FloatChromosomeSpec::random TODO");
    }
    // TODO: length and bounds check (reject NaN).
    bool isValid(const FloatChromosome&) const {
        throw std::logic_error("FloatChromosomeSpec::isValid TODO");
    }
private:
    std::size_t length_;
    double lower_, upper_;
};
}
