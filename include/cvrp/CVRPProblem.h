#pragma once
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace cvrp {
struct Location {
    int id = 0;          // 0 = depot, customers are 1..n
    double x = 0.0;
    double y = 0.0;
    double demand = 0.0; // depot demand is 0
};

class CVRPProblem {
public:
    CVRPProblem() = default;
    CVRPProblem(Location depot, std::vector<Location> customers, double capacity,
                int vehicleCount = 0, std::string name = "")
        : depot_(depot), customers_(std::move(customers)), capacity_(capacity),
          vehicleCount_(vehicleCount), name_(std::move(name)) {}

    const Location& depot() const { return depot_; }
    const std::vector<Location>& customers() const { return customers_; }
    double capacity() const { return capacity_; }
    int vehicleCount() const { return vehicleCount_; }  // 0 = unlimited / minimized
    std::size_t numCustomers() const { return customers_.size(); }
    const std::string& name() const { return name_; }

    // TODO: Euclidean distance between two location ids (0 = depot, 1..n = customers).
    double distance(int fromId, int toId) const;

    // TODO: throw std::invalid_argument if capacity <= 0, no customers, ids are not 1..n,
    // any demand < 0, or any single demand > capacity.
    void validate() const;

private:
    Location depot_;
    std::vector<Location> customers_;
    double capacity_ = 0.0;
    int vehicleCount_ = 0;
    std::string name_;
};
}
