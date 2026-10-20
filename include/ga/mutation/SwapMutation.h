#pragma once
#include <cstddef>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include "ga/operators/Mutation.h"

namespace ga {
template<class Gene>
class SwapMutation : public Mutation<Gene> {
public:
    SwapMutation() = default;

    void mutate(Chromosome<Gene>& c, std::mt19937& rng) const override {
        if (c.size() < 2) throw std::invalid_argument("SwapMutation needs size >= 2");
        std::uniform_int_distribution<std::size_t> pos(0, c.size() - 1);
        std::size_t i = pos(rng);
        std::size_t j = pos(rng);
        while (j == i) j = pos(rng);
        std::swap(c[i], c[j]);
    }

    std::string name() const override { return "Swap"; }
};
}