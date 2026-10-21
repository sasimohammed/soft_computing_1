#pragma once
#include <ostream>
#include "cvrp/CVRPProblem.h"
#include "cvrp/CVRPSolver.h"

namespace cvrp {
// Prints the solution in the same format as the OR-Tools CVRP example output:
//
//   Objective: 6208
//   Route for vehicle 0:
//    0 Load(0) ->  4 Load(0) ->  3 Load(4) ->  1 Load(6) ->  7 Load(7) ->  0 Load(15)
//   Distance of the route: 1552m
//   Load of the route: 15
//   ...
//   Total distance of all routes: 6208m
//   Total load of all routes: 60
//
// Load(x) is the load in the vehicle when it ARRIVES at that node (before picking up there),
// exactly like OR-Tools. If the instance has a fixed vehicle count, unused vehicles are printed
// as empty routes (0 -> 0). If the solution is infeasible a warning lists the violations.
void printSolution(std::ostream& out, const CVRPProblem& problem,
                   const CVRPSolution& solution);

// Prints only the routes (same format, no objective/warning lines). Used by printSolution.
void printRoutes(std::ostream& out, const CVRPProblem& problem, const Routes& routes);
}
