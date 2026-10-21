#include "cvrp/CVRPProblem.h"
#include <cmath>
#include <stdexcept>
#include <string>

namespace cvrp {

CVRPProblem::CVRPProblem(Location depot, std::vector<Location> customers, double capacity,
                         int vehicleCount, std::string name, DistanceType distanceType)
    : depot_(depot), customers_(std::move(customers)), capacity_(capacity),
      vehicleCount_(vehicleCount), name_(std::move(name)), distanceType_(distanceType) {
    if (distanceType_ == DistanceType::Explicit)
        throw std::invalid_argument("use setDistanceMatrix() for an explicit distance matrix");
    buildDistanceMatrix();
}

const Location& CVRPProblem::location(int id) const {
    if (id == 0) return depot_;
    if (id < 0 || static_cast<std::size_t>(id) > customers_.size())
        throw std::out_of_range("location id " + std::to_string(id) + " is out of range");
    return customers_[static_cast<std::size_t>(id) - 1];
}

double CVRPProblem::totalDemand() const {
    double sum = 0.0;
    for (const Location& c : customers_) sum += c.demand;
    return sum;
}

int CVRPProblem::minimumVehicles() const {
    if (capacity_ <= 0) return 0;
    // small epsilon so 60 / 15 gives exactly 4 and not 5 because of rounding noise
    return static_cast<int>(std::ceil(totalDemand() / capacity_ - 1e-9));
}

double CVRPProblem::distance(int fromId, int toId) const {
    const std::size_t size = customers_.size() + 1;
    if (fromId < 0 || toId < 0 || static_cast<std::size_t>(fromId) >= size ||
        static_cast<std::size_t>(toId) >= size)
        throw std::out_of_range("distance: location id out of range (" + std::to_string(fromId) +
                                ", " + std::to_string(toId) + ")");
    return matrix_[static_cast<std::size_t>(fromId) * size + static_cast<std::size_t>(toId)];
}

void CVRPProblem::buildDistanceMatrix() {
    const std::size_t size = customers_.size() + 1;
    matrix_.assign(size * size, 0.0);
    auto at = [&](std::size_t i) -> const Location& { return i == 0 ? depot_ : customers_[i - 1]; };
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            const double dx = at(i).x - at(j).x;
            const double dy = at(i).y - at(j).y;
            double d = 0.0;
            switch (distanceType_) {
                case DistanceType::Euclidean: d = std::sqrt(dx * dx + dy * dy); break;
                case DistanceType::RoundedEuclidean: d = std::round(std::sqrt(dx * dx + dy * dy)); break;
                case DistanceType::Manhattan: d = std::fabs(dx) + std::fabs(dy); break;
                case DistanceType::Explicit: d = 0.0; break;  // filled by setDistanceMatrix
            }
            matrix_[i * size + j] = d;
        }
    }
}

void CVRPProblem::setDistanceMatrix(std::vector<std::vector<double>> matrix) {
    const std::size_t size = customers_.size() + 1;
    if (matrix.size() != size)
        throw std::invalid_argument("distance matrix must have " + std::to_string(size) + " rows");
    matrix_.assign(size * size, 0.0);
    for (std::size_t i = 0; i < size; ++i) {
        if (matrix[i].size() != size)
            throw std::invalid_argument("distance matrix row " + std::to_string(i) + " must have " +
                                        std::to_string(size) + " columns");
        for (std::size_t j = 0; j < size; ++j) matrix_[i * size + j] = matrix[i][j];
    }
    distanceType_ = DistanceType::Explicit;
}

void CVRPProblem::validate() const {
    if (!(capacity_ > 0) || !std::isfinite(capacity_))
        throw std::invalid_argument("vehicle capacity must be a positive number");
    if (customers_.empty())
        throw std::invalid_argument("the instance has no customers");
    if (depot_.id != 0)
        throw std::invalid_argument("depot id must be 0");
    if (depot_.demand != 0)
        throw std::invalid_argument("depot demand must be 0");
    if (vehicleCount_ < 0)
        throw std::invalid_argument("vehicle count must be >= 0 (0 = unlimited)");
    for (std::size_t i = 0; i < customers_.size(); ++i) {
        const Location& c = customers_[i];
        const std::string label = "customer " + std::to_string(c.id);
        if (c.id != static_cast<int>(i) + 1)
            throw std::invalid_argument("customer ids must be 1..n in order (found " +
                                        std::to_string(c.id) + " at position " +
                                        std::to_string(i + 1) + ")");
        if (!std::isfinite(c.demand) || c.demand < 0)
            throw std::invalid_argument(label + " has a negative or invalid demand");
        if (c.demand > capacity_ + 1e-9)
            throw std::invalid_argument(label + " demand exceeds the vehicle capacity, "
                                        "no vehicle can ever serve it");
    }
    const std::size_t size = customers_.size() + 1;
    if (matrix_.size() != size * size)
        throw std::invalid_argument("distance matrix size does not match the number of locations");
    for (double d : matrix_)
        if (!std::isfinite(d) || d < 0)
            throw std::invalid_argument("distances must be finite and non-negative");
}

bool CVRPProblem::vehicleLimitReachable() const {
    return vehicleCount_ == 0 || vehicleCount_ >= minimumVehicles();
}

const char* toString(DistanceType type) {
    switch (type) {
        case DistanceType::Euclidean: return "Euclidean";
        case DistanceType::RoundedEuclidean: return "RoundedEuclidean";
        case DistanceType::Manhattan: return "Manhattan";
        case DistanceType::Explicit: return "Explicit";
    }
    return "?";
}
}
