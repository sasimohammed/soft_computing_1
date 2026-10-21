#pragma once
#include <vector>
#include "cvrp/CVRPProblem.h"

namespace cvrp {
using CustomerOrder = std::vector<int>;  // permutation of customer ids 1..n (depot is NOT a gene)
using Route = std::vector<int>;          // customer ids visited in order (depot implied at both ends)
using Routes = std::vector<Route>;

// How a customer order (the chromosome) is cut into vehicle routes.
//  Optimal - Prins' "Split" procedure: shortest-path dynamic programming over all ways of
//            cutting the order into consecutive capacity-feasible routes. Returns the cheapest
//            cutting of THIS order. O(n * L) where L = max customers that fit in one vehicle.
//  Greedy  - fill the current vehicle until the next customer does not fit, then start a new
//            one. Fast and simple, uses the minimum number of vehicles for this order, but can
//            cost more distance than Optimal.
enum class SplitMethod { Optimal, Greedy };

// Turns a chromosome (an order of all customers, depot not included) into routes.
//
// The chromosome itself never contains route separators, so crossover and mutation can never
// break the capacity constraint: the decoder only ever builds routes whose load <= capacity.
// When maxRoutes > 0 the decoder also tries to stay within that many vehicles; if this order
// cannot be cut into <= maxRoutes routes it falls back to the greedy cut (fewest vehicles) and
// the evaluator reports the solution as infeasible.
class CVRPDecoder {
public:
    explicit CVRPDecoder(SplitMethod method = SplitMethod::Optimal, int maxRoutes = 0)
        : method_(method), maxRoutes_(maxRoutes < 0 ? 0 : maxRoutes) {}

    // Throws std::invalid_argument if `order` is not a permutation of 1..n.
    Routes decode(const CVRPProblem& problem, const CustomerOrder& order) const;

    // true if `order` contains every customer id 1..n exactly once and nothing else.
    static bool isPermutation(const CVRPProblem& problem, const CustomerOrder& order);

    // The individual cutting procedures (order must be a valid permutation).
    static Routes greedySplit(const CVRPProblem& problem, const CustomerOrder& order);
    static Routes optimalSplit(const CVRPProblem& problem, const CustomerOrder& order);
    // Cheapest cutting using at most maxRoutes routes. Returns an EMPTY Routes if impossible.
    static Routes optimalSplitLimited(const CVRPProblem& problem, const CustomerOrder& order,
                                      int maxRoutes);

    // Routes -> chromosome (concatenation). Useful for seeding and tests.
    static CustomerOrder flatten(const Routes& routes);

    SplitMethod method() const { return method_; }
    int maxRoutes() const { return maxRoutes_; }

private:
    SplitMethod method_;
    int maxRoutes_;
};
}
