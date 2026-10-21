#include "cvrp/CVRPPrinter.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include "cvrp/CVRPEvaluator.h"

namespace cvrp {
namespace {
// Integers print without decimals (6208), other values with 2 decimals (1203.47).
std::string fmt(double v) {
    std::ostringstream s;
    if (std::fabs(v - std::round(v)) < 1e-6) s << static_cast<long long>(std::llround(v));
    else s << std::fixed << std::setprecision(2) << v;
    return s.str();
}
}  

void printRoutes(std::ostream& out, const CVRPProblem& problem, const Routes& routes) {
    const CVRPEvaluator evaluator(problem);

    // keep only real routes then add empty ones for unused vehicles of a fixed fleet
    Routes printed;
    for (const Route& r : routes)
        if (!r.empty()) printed.push_back(r);
    while (problem.vehicleCount() > 0 && printed.size() < static_cast<std::size_t>(problem.vehicleCount()))
        printed.push_back({});

    double totalDistance = 0.0, totalLoad = 0.0;
    for (std::size_t v = 0; v < printed.size(); ++v) {
        const Route& route = printed[v];
        double load = 0.0;
        out << "Route for vehicle " << v << ":\n";
        out << " 0 Load(0) -> ";
        for (int id : route) {
            out << ' ' << id << " Load(" << fmt(load) << ") -> ";
            load += problem.demand(id);
        }
        out << " 0 Load(" << fmt(load) << ")\n";
        const double distance = evaluator.routeDistance(route);
        out << "Distance of the route: " << fmt(distance) << "m\n";
        out << "Load of the route: " << fmt(load) << "\n\n";
        totalDistance += distance;
        totalLoad += load;
    }
    out << "Total distance of all routes: " << fmt(totalDistance) << "m\n";
    out << "Total load of all routes: " << fmt(totalLoad) << '\n';
}

void printSolution(std::ostream& out, const CVRPProblem& problem, const CVRPSolution& solution) {
    const CVRPEvaluator evaluator(problem, solution.feasible ? false : problem.vehicleCount() > 0);
    if (!solution.feasible) {
        out << "WARNING: the best solution found is INFEASIBLE\n";
        for (const std::string& v : evaluator.violations(solution.routes)) out << "  - " << v << '\n';
    }
    out << "Objective: " << fmt(evaluator.totalDistance(solution.routes)) << '\n';
    printRoutes(out, problem, solution.routes);
}
}
