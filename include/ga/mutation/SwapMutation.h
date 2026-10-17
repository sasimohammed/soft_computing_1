#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Mutation.h"

namespace ga {
// Swap two random genes. Keeps permutations valid. Throw if size < 2.
template<class Gene>
class SwapMutation : public Mutation<Gene> {
public:
    SwapMutation() = default;

    // TODO: implement.
    void mutate(Chromosome<Gene>&, std::mt19937&) const override {
        throw std::logic_error("SwapMutation::mutate TODO");
    }

    std::string name() const override { return "Swap"; }

};
}
