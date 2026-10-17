#pragma once
#include <vector>
#include "cvrp/CVRPProblem.h"

namespace cvrp {
using CustomerOrder = std::vector<int>;  // permutation of customer ids 1..n (depot is NOT a gene)
using Route = std::vector<int>;          // customer ids visited in order (depot implied at both ends)
using Routes = std::vector<Route>;

class CVRPDecoder {
public:
    // TODO: walk the permutation, start a new route whenever adding the next
    // customer would exceed capacity (greedy split). Every customer must appear once.
    Routes decode(const CVRPProblem& problem, const CustomerOrder& order) const;
};
}
