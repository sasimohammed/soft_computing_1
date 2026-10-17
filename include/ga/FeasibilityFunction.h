#pragma once
#include <functional>

namespace ga {
// User supplied. Returns true if the chromosome satisfies all constraints.
template<class ChromosomeType>
using FeasibilityFunction = std::function<bool(const ChromosomeType&)>;
}
