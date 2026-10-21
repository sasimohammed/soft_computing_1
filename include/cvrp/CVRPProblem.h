#pragma once
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace cvrp {

// One stop of the instance. Ids are internal: 0 = depot, customers are 1..n
// (the parser renumbers whatever labels the input file uses).
struct Location {
    int id = 0;          // 0 = depot, customers are 1..n
    double x = 0.0;
    double y = 0.0;
    double demand = 0.0; // depot demand is 0
};

// How the cost between two locations is measured.
//  Euclidean        - straight line sqrt(dx^2 + dy^2)
//  RoundedEuclidean - Euclidean rounded to the nearest integer (TSPLIB / CVRPLIB "EUC_2D",
//                     needed to compare against published optimal values)
//  Manhattan        - |dx| + |dy| (city blocks; the OR-Tools example uses this)
//  Explicit         - a full distance matrix given by the instance file
enum class DistanceType { Euclidean, RoundedEuclidean, Manhattan, Explicit };

// The CVRP instance: one depot, n customers with demands, a homogeneous fleet with a
// single capacity, and (optionally) a number of available vehicles.
//
// All pairwise distances are computed ONCE when the problem is built and stored in a
// (n+1) x (n+1) matrix, because the GA calls distance() millions of times.
class CVRPProblem {
public:
    CVRPProblem() = default;
    CVRPProblem(Location depot, std::vector<Location> customers, double capacity,
                int vehicleCount = 0, std::string name = "",
                DistanceType distanceType = DistanceType::Euclidean);

    const Location& depot() const { return depot_; }
    const std::vector<Location>& customers() const { return customers_; }
    double capacity() const { return capacity_; }
    int vehicleCount() const { return vehicleCount_; }  // 0 = unlimited / minimized
    std::size_t numCustomers() const { return customers_.size(); }
    const std::string& name() const { return name_; }
    DistanceType distanceType() const { return distanceType_; }

    // Location by internal id (0 = depot). Throws std::out_of_range for a bad id.
    const Location& location(int id) const;
    double demand(int id) const { return location(id).demand; }

    // Sum of all customer demands.
    double totalDemand() const;
    // ceil(totalDemand / capacity): no solution can use fewer vehicles than this.
    int minimumVehicles() const;

    // Cost from one location id to another (0 = depot, 1..n = customers).
    // Throws std::out_of_range for an id outside 0..n.
    double distance(int fromId, int toId) const;

    // Replaces the computed distances by an explicit matrix (row/column 0 = depot) and
    // switches the distance type to Explicit. Throws std::invalid_argument if the matrix is
    // not (n+1) x (n+1).
    void setDistanceMatrix(std::vector<std::vector<double>> matrix);

    // Throws std::invalid_argument if the instance cannot be solved or is malformed:
    // capacity <= 0, no customers, ids are not 1..n in order, depot id is not 0,
    // any demand < 0, any single demand > capacity, negative vehicle count,
    // or a distance matrix with negative / non-finite entries.
    // A vehicle count that is too small is NOT an error here (it is a soft limit unless the
    // solver enforces it); use vehicleLimitReachable() to check it.
    void validate() const;

    // true if vehicleCount() == 0 or vehicleCount() >= minimumVehicles().
    // (Necessary condition only: bin packing may still need more vehicles.)
    bool vehicleLimitReachable() const;

private:
    void buildDistanceMatrix();

    Location depot_;
    std::vector<Location> customers_;
    double capacity_ = 0.0;
    int vehicleCount_ = 0;
    std::string name_;
    DistanceType distanceType_ = DistanceType::Euclidean;
    std::vector<double> matrix_;  // row-major (n+1) x (n+1)
};

const char* toString(DistanceType type);
}
