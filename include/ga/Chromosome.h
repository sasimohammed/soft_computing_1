#pragma once
#include <vector>

namespace ga {
template<class Gene>
using Chromosome = std::vector<Gene>;

template<class Gene>
using Population = std::vector<Chromosome<Gene>>;
}
