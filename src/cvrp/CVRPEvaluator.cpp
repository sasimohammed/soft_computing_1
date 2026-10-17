#include "cvrp/CVRPEvaluator.h"
#include <stdexcept>

namespace cvrp {
double CVRPEvaluator::routeDistance(const Route&) const {
    throw std::logic_error("CVRPEvaluator::routeDistance TODO");
}
double CVRPEvaluator::totalDistance(const Routes&) const {
    throw std::logic_error("CVRPEvaluator::totalDistance TODO");
}
bool CVRPEvaluator::isFeasible(const Routes&) const {
    throw std::logic_error("CVRPEvaluator::isFeasible(Routes) TODO");
}
bool CVRPEvaluator::isFeasible(const CustomerOrder&) const {
    throw std::logic_error("CVRPEvaluator::isFeasible(order) TODO");
}
double CVRPEvaluator::fitness(const CustomerOrder&) const {
    throw std::logic_error("CVRPEvaluator::fitness TODO");
}
}
