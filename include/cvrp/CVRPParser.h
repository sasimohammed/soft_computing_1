#pragma once
#include <string>
#include "cvrp/CVRPProblem.h"

namespace cvrp {
// TODO: read an instance file in the SAME format as the assignment example
// (depot, coordinates, demands, capacity). Throw std::runtime_error on bad input.
CVRPProblem loadProblem(const std::string& path);
}
