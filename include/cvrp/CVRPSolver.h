#pragma once
#include "cvrp/CVRPDecoder.h"
#include "cvrp/CVRPProblem.h"
#include "ga/GAConfig.h"

namespace cvrp {
struct CVRPSolution {
    Routes routes;
    double totalDistance = 0.0;
    bool feasible = false;
};

class CVRPSolver {
public:
    explicit CVRPSolver(ga::GAConfig config = ga::GAConfig()) : config_(config) {}

    // TODO: build GAProblem<int> from CVRPEvaluator (fitness, feasibility,
    // permutation initializer, optional repair), pick OrderCrossover +
    // SwapMutation + Tournament + Elitist, run the GA, decode the best result.
    CVRPSolution solve(const CVRPProblem& problem);

private:
    ga::GAConfig config_;
};
}
