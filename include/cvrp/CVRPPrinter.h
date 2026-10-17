#pragma once
#include <ostream>
#include "cvrp/CVRPProblem.h"
#include "cvrp/CVRPSolver.h"

namespace cvrp {
// TODO: print the solution in the SAME format as the assignment's example output
// (routes, per-route load/cost if required, total cost).
void printSolution(std::ostream& out, const CVRPProblem& problem,
                   const CVRPSolution& solution);
}
