#include "cvrp/CVRPEvaluator.h"
#include <algorithm>
#include <functional>
#include <string>

namespace cvrp {
namespace {
constexpr double kEps = 1e-9;

std::string number(double v) {
    std::string s = std::to_string(v);
    s.erase(s.find_last_not_of('0') + 1);
    if (!s.empty() && s.back() == '.') s.pop_back();
    return s;
}
}  

CVRPEvaluator::CVRPEvaluator(const CVRPProblem& problem, bool enforceVehicleLimit, SplitMethod split)
    : problem_(problem),
      enforceVehicleLimit_(enforceVehicleLimit && problem.vehicleCount() > 0),
      decoder_(split, enforceVehicleLimit_ ? problem.vehicleCount() : 0),
      penaltyWeight_(0.0) {
    double farthest = 0.0;
    for (std::size_t id = 1; id <= problem_.numCustomers(); ++id)
        farthest = std::max(farthest, problem_.distance(0, static_cast<int>(id)) +
                                          problem_.distance(static_cast<int>(id), 0));
    penaltyWeight_ = std::max(1.0, farthest);
}

double CVRPEvaluator::routeDistance(const Route& route) const {
    if (route.empty()) return 0.0;
    double d = problem_.distance(0, route.front());
    for (std::size_t i = 1; i < route.size(); ++i) d += problem_.distance(route[i - 1], route[i]);
    return d + problem_.distance(route.back(), 0);
}

double CVRPEvaluator::routeLoad(const Route& route) const {
    double load = 0.0;
    for (int id : route) load += problem_.demand(id);
    return load;
}

double CVRPEvaluator::totalDistance(const Routes& routes) const {
    double total = 0.0;
    for (const Route& r : routes) total += routeDistance(r);
    return total;
}

std::vector<std::string> CVRPEvaluator::violations(const Routes& routes) const {
    std::vector<std::string> problems;
    const std::size_t n = problem_.numCustomers();
    std::vector<int> visits(n + 1, 0);
    int usedVehicles = 0;

    for (std::size_t r = 0; r < routes.size(); ++r) {
        const Route& route = routes[r];
        if (route.empty()) continue;  // an unused vehicle is allowed
        ++usedVehicles;
        double load = 0.0;
        bool badId = false;
        for (int id : route) {
            if (id < 1 || static_cast<std::size_t>(id) > n) {  // C2
                problems.push_back("route " + std::to_string(r) + " contains invalid customer id " +
                                   std::to_string(id));
                badId = true;
                continue;
            }
            ++visits[static_cast<std::size_t>(id)];
            load += problem_.demand(id);
        }
        if (!badId && load > problem_.capacity() + kEps)  // C3
            problems.push_back("route " + std::to_string(r) + " load " + number(load) +
                               " exceeds capacity " + number(problem_.capacity()));
    }
    for (std::size_t id = 1; id <= n; ++id) {  // C1
        if (visits[id] == 0)
            problems.push_back("customer " + std::to_string(id) + " is not visited");
        else if (visits[id] > 1)
            problems.push_back("customer " + std::to_string(id) + " is visited " +
                               std::to_string(visits[id]) + " times");
    }
    if (enforceVehicleLimit_ && usedVehicles > problem_.vehicleCount())  // C4
        problems.push_back("uses " + std::to_string(usedVehicles) + " vehicles but only " +
                           std::to_string(problem_.vehicleCount()) + " are available");
    return problems;
}

bool CVRPEvaluator::isFeasible(const Routes& routes) const { return violations(routes).empty(); }

bool CVRPEvaluator::isFeasible(const CustomerOrder& order) const {
    if (!CVRPDecoder::isPermutation(problem_, order)) return false;
    return isFeasible(decoder_.decode(problem_, order));
}

// Only C4 can be violated by a decoded permutation (the decoder guarantees C1..C3).
// Penalty = penaltyWeight * (extra vehicles + share of a vehicle that the extra routes carry).
// The second term makes the landscape smooth: among orders that need one vehicle too many, the
// one whose extra route carries less demand is closer to feasible and gets a smaller penalty.
double CVRPEvaluator::penalty(const Routes& routes) const {
    if (!enforceVehicleLimit_) return 0.0;
    std::vector<double> loads;
    for (const Route& r : routes)
        if (!r.empty()) loads.push_back(routeLoad(r));
    const std::size_t limit = static_cast<std::size_t>(problem_.vehicleCount());
    if (loads.size() <= limit) return 0.0;
    std::sort(loads.begin(), loads.end(), std::greater<double>());
    double overflow = 0.0;  // demand carried by the routes that do not have a vehicle
    for (std::size_t i = limit; i < loads.size(); ++i) overflow += loads[i];
    const double extraVehicles = static_cast<double>(loads.size() - limit);
    return penaltyWeight_ * (extraVehicles + overflow / problem_.capacity());
}

double CVRPEvaluator::cost(const CustomerOrder& order) const {
    const Routes routes = decoder_.decode(problem_, order);
    return totalDistance(routes) + penalty(routes);
}

double CVRPEvaluator::fitness(const CustomerOrder& order) const {
    if (!CVRPDecoder::isPermutation(problem_, order)) return 0.0;
    return 1.0 / (1.0 + cost(order));
}
}
