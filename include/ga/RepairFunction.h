#pragma once
#include <functional>
#include <random>

namespace ga {
// Optional, used by InfeasibleStrategy::Repair. Returns a feasible version.
    template<class ChromosomeType>
    using RepairFunction =
            std::function<ChromosomeType(const ChromosomeType&, std::mt19937&)>;
}