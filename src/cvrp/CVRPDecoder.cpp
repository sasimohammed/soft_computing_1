#include "cvrp/CVRPDecoder.h"
#include <stdexcept>

namespace cvrp {
Routes CVRPDecoder::decode(const CVRPProblem&, const CustomerOrder&) const {
    throw std::logic_error("CVRPDecoder::decode TODO");
}
}
