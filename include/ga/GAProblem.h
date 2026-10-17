#pragma once
#include <functional>
#include <random>
#include "ga/Chromosome.h"
#include "ga/FitnessFunction.h"
#include "ga/FeasibilityFunction.h"
#include "ga/RepairFunction.h"

namespace ga {
// Everything problem-specific the GA needs. The GA itself never changes
// when you switch to a different optimization problem.
template<class Gene>
struct GAProblem {
    using ChromosomeType = Chromosome<Gene>;
    using Initializer = std::function<ChromosomeType(std::mt19937&)>;

    FitnessFunction<ChromosomeType> fitness;      // required
    FeasibilityFunction<ChromosomeType> feasible; // required
    Initializer initializer;                      // required: random chromosome
    RepairFunction<ChromosomeType> repair;        // optional (needed for Repair strategy)
};
}
