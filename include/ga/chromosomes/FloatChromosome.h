#pragma once
#include <cmath>
#include <cstddef>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "ga/Chromosome.h"

namespace ga {
using FloatChromosome = Chromosome<double>;

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

    FloatChromosome random(std::mt19937& rng) const {
        std::uniform_real_distribution<double> d(lower_, upper_);
        std::vector<double> genes(length_);
        for (auto& g : genes) g = d(rng);
        return FloatChromosome(std::move(genes));
    }

    bool isValid(const FloatChromosome& c) const {
        if (c.size() != length_) return false;
        for (std::size_t i = 0; i < c.size(); ++i) {
            double g = c[i];
            if (std::isnan(g) || g < lower_ || g > upper_) return false;
        }
        return true;
    }

private:
    std::size_t length_;
    double lower_, upper_;
};
}