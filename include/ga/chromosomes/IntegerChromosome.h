#pragma once
#include <algorithm>
#include <cstddef>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "ga/Chromosome.h"

namespace ga {
using IntegerChromosome = Chromosome<int>;

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

    IntegerChromosome random(std::mt19937& rng) const {
        std::vector<int> genes(length_);
        if (permutation_) {
            std::iota(genes.begin(), genes.end(), min_);
            std::shuffle(genes.begin(), genes.end(), rng);
        } else {
            std::uniform_int_distribution<int> d(min_, max_);
            for (auto& g : genes) g = d(rng);
        }
        return IntegerChromosome(std::move(genes));
    }

    bool isValid(const IntegerChromosome& c) const {
        if (c.size() != length_) return false;
        std::vector<char> seen(permutation_ ? length_ : 0, 0);
        for (std::size_t i = 0; i < c.size(); ++i) {
            int g = c[i];
            if (g < min_ || g > max_) return false;
            if (permutation_) {
                std::size_t idx = static_cast<std::size_t>(g - min_);
                if (seen[idx]) return false;
                seen[idx] = 1;
            }
        }
        return true;
    }

private:
    std::size_t length_;
    int min_, max_;
    bool permutation_;
};
}