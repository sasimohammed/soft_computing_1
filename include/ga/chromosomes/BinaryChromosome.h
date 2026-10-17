#pragma once
#include <cstddef>
#include <random>
#include <stdexcept>
#include "ga/Chromosome.h"

namespace ga {
using BinaryChromosome = Chromosome<int>;

// Describes the representation: fixed length, every gene is 0 or 1.
class BinaryChromosomeSpec {
public:
    explicit BinaryChromosomeSpec(std::size_t length) : length_(length) {
        if (length_ == 0) throw std::invalid_argument("length must be > 0");
    }
    std::size_t length() const { return length_; }

    // TODO: random 0/1 chromosome of the given length.
    BinaryChromosome random(std::mt19937&) const {
        throw std::logic_error("BinaryChromosomeSpec::random TODO");
    }
    // TODO: size == length and every gene in {0,1}.
    bool isValid(const BinaryChromosome&) const {
        throw std::logic_error("BinaryChromosomeSpec::isValid TODO");
    }
private:
    std::size_t length_;
};
}
