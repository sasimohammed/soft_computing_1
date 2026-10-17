#pragma once
#include "cvrp/CVRPDecoder.h"
#include "cvrp/CVRPProblem.h"

namespace cvrp {
// Provides the callbacks handed to the GA toolbox. Keeps a reference to the
// problem, so the problem must outlive the evaluator.
class CVRPEvaluator {
public:
    explicit CVRPEvaluator(const CVRPProblem& problem) : problem_(problem) {}

    // TODO: distance of one route including depot -> first and last -> depot.
    double routeDistance(const Route& route) const;
    // TODO: sum of routeDistance over all routes.
    double totalDistance(const Routes& routes) const;
    // TODO: every customer exactly once, every route load <= capacity,
    // (and route count <= vehicleCount when it is fixed).
    bool isFeasible(const Routes& routes) const;
    bool isFeasible(const CustomerOrder& order) const;  // decode, then check
    // TODO: higher is better, e.g. 1 / (1 + totalDistance).
    double fitness(const CustomerOrder& order) const;

    const CVRPProblem& problem() const { return problem_; }

private:
    const CVRPProblem& problem_;
    CVRPDecoder decoder_;
};
}
