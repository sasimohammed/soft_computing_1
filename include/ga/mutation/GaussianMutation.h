#pragma once
#include <algorithm>
#include <cstddef>
#include <random>
#include <stdexcept>
#include <string>
#include "ga/operators/Mutation.h"

namespace ga {
template<class Gene>
class GaussianMutation : public Mutation<Gene> {
public:
    GaussianMutation(double sigma, double lower, double upper, double perGeneRate = 0.1)
        : sigma_(sigma), lower_(lower), upper_(upper), perGeneRate_(perGeneRate) {
        if (sigma_ <= 0) throw std::invalid_argument("sigma must be > 0");
        if (lower_ >= upper_) throw std::invalid_argument("lower must be < upper");
        if (perGeneRate_ < 0 || perGeneRate_ > 1)
            throw std::invalid_argument("perGeneRate must be in [0,1]");
    }

    void mutate(Chromosome<Gene>& c, std::mt19937& rng) const override {
        std::bernoulli_distribution apply(perGeneRate_);
        std::normal_distribution<double> noise(0.0, sigma_);
        for (std::size_t i = 0; i < c.size(); ++i) {
            if (apply(rng)) {
                double v = static_cast<double>(c[i]) + noise(rng);
                c[i] = static_cast<Gene>(std::min(std::max(v, lower_), upper_));
            }
        }
    }

    std::string name() const override { return "Gaussian"; }
private:
    double sigma_, lower_, upper_, perGeneRate_;
};
}