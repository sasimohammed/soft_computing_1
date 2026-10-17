#pragma once
#include <functional>

namespace ga {
// User supplied. Higher return value = better solution.
template<class ChromosomeType>
using FitnessFunction = std::function<double(const ChromosomeType&)>;
}
