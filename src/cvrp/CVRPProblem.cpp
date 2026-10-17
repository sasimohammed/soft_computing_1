#include "cvrp/CVRPProblem.h"
#include <stdexcept>

namespace cvrp {
double CVRPProblem::distance(int, int) const {
    throw std::logic_error("CVRPProblem::distance TODO");
}

void CVRPProblem::validate() const {
    throw std::logic_error("CVRPProblem::validate TODO");
}
}
