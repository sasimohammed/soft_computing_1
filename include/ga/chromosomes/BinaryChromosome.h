#pragma once
#include <cstddef>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "ga/Chromosome.h"

namespace ga {
using BinaryChromosome = Chromosome<int>;

class BinaryChromosomeSpec {
public:
    explicit BinaryChromosomeSpec(std::size_t length) : length_(length) {
        if (length_ == 0) throw std::invalid_argument("length must be > 0");
    }
    std::size_t length() const { return length_; }

    BinaryChromosome random(std::mt19937& rng) const {
        std::bernoulli_distribution d(0.5);
        std::vector<int> genes(length_);
        for (auto& g : genes) g = d(rng) ? 1 : 0;
        return BinaryChromosome(std::move(genes));
    }

    bool isValid(const BinaryChromosome& c) const {
        if (c.size() != length_) return false;
        for (std::size_t i = 0; i < c.size(); ++i) {
            if (c[i] != 0 && c[i] != 1) return false;
        }
        return true;
    }

private:
    std::size_t length_;
};
}