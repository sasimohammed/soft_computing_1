#pragma once
#include <random>
#include "cvrp/CVRPDecoder.h"
#include "cvrp/CVRPProblem.h"
#include "ga/GAConfig.h"
#include "ga/GAResult.h"

namespace cvrp {
    struct CVRPSolution {
        Routes routes;
        double totalDistance = 0.0;
        bool feasible = false;
        ga::GAResult<int> gaResult;  // fitness history etc.
    };

    class CVRPSolver {
    public:
        // Hyperparameters tuned for CVRP (see docs/TUNING.md). Any field can be overridden.
        static ga::GAConfig recommendedConfig() {
            ga::GAConfig config;
            config.populationSize = 200;
            config.generations = 600;
            config.crossoverRate = 0.9;
            config.mutationRate = 0.3;
            config.tournamentSize = 4;
            config.eliteCount = 2;
            config.stagnationLimit = 200;  // stop early when the best has not improved
            return config;
        }

        explicit CVRPSolver(ga::GAConfig config = recommendedConfig(), bool enforceVehicleLimit = false)
                : config_(config), enforceVehicleLimit_(enforceVehicleLimit) {}

        // Builds a GAProblem<int> (permutation of customers) from CVRPEvaluator and runs the
        // generic GA with Tournament + OrderCrossover + Swap/Inversion mutation + Elitist replacement.
        // The decoder cuts the evolved customer order into the cheapest capacity-feasible routes.
        // Throws std::invalid_argument for an unusable instance.
        CVRPSolution solve(const CVRPProblem& problem);

        // Repair function handed to the GA for InfeasibleStrategy::Repair. Turns any sequence of
        // ints into a valid permutation of 1..n: keeps the first occurrence of every valid
        // customer id, drops duplicates and unknown ids, and appends the missing customers in
        // random order. A valid order is returned unchanged.
        static CustomerOrder repairOrder(const CVRPProblem& problem, const CustomerOrder& order,
                                         std::mt19937& rng);

    private:
        ga::GAConfig config_;
        bool enforceVehicleLimit_;
    };
}