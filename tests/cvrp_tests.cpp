#include "TestUtils.h"
#include "cvrp/CVRPEvaluator.h"
#include "cvrp/CVRPProblem.h"

int main() {
    cvrp::Location depot{0, 0.0, 0.0, 0.0};
    std::vector<cvrp::Location> customers = {
        {1, 3.0, 4.0, 5.0}, {2, 6.0, 8.0, 7.0}, {3, -3.0, 4.0, 4.0}};
    cvrp::CVRPProblem problem(depot, customers, 10.0);
    CHECK(problem.numCustomers() == 3);
    CHECK(problem.capacity() == 10.0);
    cvrp::CVRPEvaluator evaluator(problem);
    CHECK(&evaluator.problem() == &problem);
    // TODO: distance(0,1) == 5; decoder splits 1,2,3 into capacity-feasible routes
    //       and visits every customer exactly once; infeasible instances
    //       (demand > capacity) are rejected; fitness is higher for shorter routes;
    //       full GA run on a small instance returns a feasible solution.
    return finishTests("cvrp_tests");
}
