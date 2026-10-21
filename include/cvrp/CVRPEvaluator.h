#pragma once
#include <string>
#include <vector>
#include "cvrp/CVRPDecoder.h"
#include "cvrp/CVRPProblem.h"

namespace cvrp {
// Provides the callbacks handed to the GA toolbox (fitness + feasibility check).
// Keeps a reference to the problem, so the problem must outlive the evaluator.
// Cheap to copy (the solver captures it by value in lambdas).
//
// Constraints of a CVRP solution:
//   C1  every customer 1..n is visited exactly once (no duplicates, nothing missing)
//   C2  only valid customer ids appear in routes (the depot is implied, never listed)
//   C3  the load of every route is <= vehicle capacity
//   C4  (only when enforceVehicleLimit and the instance gives a vehicle count)
//       number of non-empty routes <= vehicle count
class CVRPEvaluator {
public:
    explicit CVRPEvaluator(const CVRPProblem& problem, bool enforceVehicleLimit = false,
                           SplitMethod split = SplitMethod::Optimal);

    // Distance of one route including depot -> first and last -> depot. Empty route = 0.
    double routeDistance(const Route& route) const;
    // Sum of customer demands on the route.
    double routeLoad(const Route& route) const;
    // Sum of routeDistance over all routes.
    double totalDistance(const Routes& routes) const;

    // Feasibility check of a decoded solution (checks C1..C4 independently of the decoder).
    bool isFeasible(const Routes& routes) const;
    // Same, but returns a human-readable description of every violated constraint
    // (empty vector = feasible). Used by the printer and in tests.
    std::vector<std::string> violations(const Routes& routes) const;
    // Feasibility check handed to the GA: not a permutation -> false; else decode, then check.
    bool isFeasible(const CustomerOrder& order) const;

    // Total distance + penalty for violated constraints. For a feasible order this is exactly
    // the total distance. Infeasible orders (vehicle limit exceeded) get a penalty that grows
    // with how much demand does not fit into the allowed vehicles, so the GA is guided back
    // towards feasibility when the Penalty strategy is used.
    double cost(const CustomerOrder& order) const;
    // Fitness handed to the GA, higher is better: 1 / (1 + cost). In (0, 1].
    // A chromosome that is not a permutation of 1..n gets 0 (the worst possible value).
    double fitness(const CustomerOrder& order) const;

    const CVRPProblem& problem() const { return problem_; }
    const CVRPDecoder& decoder() const { return decoder_; }
    bool enforcesVehicleLimit() const { return enforceVehicleLimit_; }
    // The vehicle limit in force (0 = no limit).
    int vehicleLimit() const { return enforceVehicleLimit_ ? problem_.vehicleCount() : 0; }

private:
    double penalty(const Routes& routes) const;

    const CVRPProblem& problem_;
    bool enforceVehicleLimit_;
    CVRPDecoder decoder_;
    double penaltyWeight_;  // cost of one "missing vehicle" (2 x farthest customer from the depot)
};
}
