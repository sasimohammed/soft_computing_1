#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Mutation.h"

namespace ga {
// OPTIONAL extra for float chromosomes: add N(0, sigma) noise, clamp to bounds.
template<class Gene>
class GaussianMutation : public Mutation<Gene> {
public:
    GaussianMutation(double sigma, double lower, double upper, double perGeneRate = 0.1)
        : sigma_(sigma), lower_(lower), upper_(upper), perGeneRate_(perGeneRate) {
        if (sigma_ <= 0) throw std::invalid_argument("sigma must be > 0");
        if (lower_ >= upper_) throw std::invalid_argument("lower must be < upper");
    }

    // TODO: implement.
    void mutate(Chromosome<Gene>&, std::mt19937&) const override {
        throw std::logic_error("GaussianMutation::mutate TODO");
    }

    std::string name() const override { return "Gaussian"; }
private:
    double sigma_, lower_, upper_, perGeneRate_;
};
}
