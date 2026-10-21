#include "cvrp/CVRPDecoder.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace cvrp {
namespace {
constexpr double kInf = std::numeric_limits<double>::infinity();
constexpr double kEps = 1e-9;  // tolerance for floating-point demands

// Rebuilds the routes from the predecessor array of a split: pred[j] = i means the last
// route serves order[i..j-1].
Routes buildRoutes(const CustomerOrder& order, const std::vector<std::size_t>& pred) {
    Routes routes;
    std::size_t j = order.size();
    while (j > 0) {
        const std::size_t i = pred[j];
        routes.emplace_back(order.begin() + static_cast<std::ptrdiff_t>(i),
                            order.begin() + static_cast<std::ptrdiff_t>(j));
        j = i;
    }
    std::reverse(routes.begin(), routes.end());
    return routes;
}
}  // namespace

bool CVRPDecoder::isPermutation(const CVRPProblem& problem, const CustomerOrder& order) {
    const std::size_t n = problem.numCustomers();
    if (order.size() != n) return false;
    std::vector<char> seen(n + 1, 0);
    for (int id : order) {
        if (id < 1 || static_cast<std::size_t>(id) > n) return false;
        if (seen[static_cast<std::size_t>(id)]) return false;
        seen[static_cast<std::size_t>(id)] = 1;
    }
    return true;
}

Routes CVRPDecoder::greedySplit(const CVRPProblem& problem, const CustomerOrder& order) {
    Routes routes;
    Route current;
    double load = 0.0;
    for (int id : order) {
        const double d = problem.demand(id);
        if (!current.empty() && load + d > problem.capacity() + kEps) {
            routes.push_back(std::move(current));
            current.clear();
            load = 0.0;
        }
        current.push_back(id);
        load += d;
    }
    if (!current.empty()) routes.push_back(std::move(current));
    return routes;
}

// Prins (2004) Split. Node j of an auxiliary graph = "first j customers of the order are
// served". An arc i -> j exists when customers order[i..j-1] fit in one vehicle, and its weight
// is the cost of that route: depot -> order[i] -> ... -> order[j-1] -> depot.
// The shortest path 0 -> n gives the cheapest way to cut this order into routes.
Routes CVRPDecoder::optimalSplit(const CVRPProblem& problem, const CustomerOrder& order) {
    const std::size_t n = order.size();
    std::vector<double> best(n + 1, kInf);
    std::vector<std::size_t> pred(n + 1, 0);
    best[0] = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        if (best[i] == kInf) continue;
        double load = 0.0, cost = 0.0;
        for (std::size_t j = i; j < n; ++j) {
            const int c = order[j];
            load += problem.demand(c);
            if (load > problem.capacity() + kEps) break;
            if (j == i)
                cost = problem.distance(0, c) + problem.distance(c, 0);
            else  // extend the route: remove old return leg, add prev -> c and c -> depot
                cost += -problem.distance(order[j - 1], 0) + problem.distance(order[j - 1], c) +
                        problem.distance(c, 0);
            if (best[i] + cost < best[j + 1] - 1e-12) {
                best[j + 1] = best[i] + cost;
                pred[j + 1] = i;
            }
        }
    }
    return buildRoutes(order, pred);
}

// Same graph, but the shortest path may use at most maxRoutes arcs (Bellman-Ford by layers):
// best[k][j] = cheapest way to serve the first j customers with exactly k routes.
Routes CVRPDecoder::optimalSplitLimited(const CVRPProblem& problem, const CustomerOrder& order,
                                        int maxRoutes) {
    const std::size_t n = order.size();
    if (maxRoutes <= 0) return optimalSplit(problem, order);
    const std::size_t K = std::min<std::size_t>(static_cast<std::size_t>(maxRoutes), n);
    std::vector<std::vector<double>> best(K + 1, std::vector<double>(n + 1, kInf));
    std::vector<std::vector<std::size_t>> pred(K + 1, std::vector<std::size_t>(n + 1, 0));
    best[0][0] = 0.0;
    for (std::size_t k = 0; k < K; ++k) {
        for (std::size_t i = 0; i < n; ++i) {
            if (best[k][i] == kInf) continue;
            double load = 0.0, cost = 0.0;
            for (std::size_t j = i; j < n; ++j) {
                const int c = order[j];
                load += problem.demand(c);
                if (load > problem.capacity() + kEps) break;
                if (j == i)
                    cost = problem.distance(0, c) + problem.distance(c, 0);
                else
                    cost += -problem.distance(order[j - 1], 0) + problem.distance(order[j - 1], c) +
                            problem.distance(c, 0);
                if (best[k][i] + cost < best[k + 1][j + 1] - 1e-12) {
                    best[k + 1][j + 1] = best[k][i] + cost;
                    pred[k + 1][j + 1] = i;
                }
            }
        }
    }
    // cheapest number of routes k <= K (ties go to fewer vehicles)
    std::size_t bestK = 0;
    for (std::size_t k = 1; k <= K; ++k)
        if (best[k][n] < kInf && (bestK == 0 || best[k][n] < best[bestK][n] - 1e-12)) bestK = k;
    if (bestK == 0) return {};  // impossible within the limit

    Routes routes;
    std::size_t j = n;
    for (std::size_t k = bestK; k > 0; --k) {
        const std::size_t i = pred[k][j];
        routes.emplace_back(order.begin() + static_cast<std::ptrdiff_t>(i),
                            order.begin() + static_cast<std::ptrdiff_t>(j));
        j = i;
    }
    std::reverse(routes.begin(), routes.end());
    return routes;
}

Routes CVRPDecoder::decode(const CVRPProblem& problem, const CustomerOrder& order) const {
    if (!isPermutation(problem, order))
        throw std::invalid_argument("CVRPDecoder::decode: chromosome is not a permutation of 1..n");
    if (method_ == SplitMethod::Greedy) return greedySplit(problem, order);
    if (maxRoutes_ > 0) {
        Routes limited = optimalSplitLimited(problem, order, maxRoutes_);
        if (!limited.empty()) return limited;
        // this order cannot be cut into maxRoutes routes: return the cutting with the fewest
        // vehicles so the evaluator can measure (and penalise) how far it is from feasible
        return greedySplit(problem, order);
    }
    return optimalSplit(problem, order);
}

CustomerOrder CVRPDecoder::flatten(const Routes& routes) {
    CustomerOrder order;
    for (const Route& r : routes) order.insert(order.end(), r.begin(), r.end());
    return order;
}
}
