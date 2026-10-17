#include "TestUtils.h"
#include "ga/crossover/MultiPointCrossover.h"
#include "ga/crossover/OrderCrossover.h"

int main() {
    ga::MultiPointCrossover<int> multi(2);
    ga::OrderCrossover<int> order;
    CHECK(multi.name() == "MultiPoint");
    CHECK(order.name() == "Order (OX)");
    CHECK_THROWS(ga::MultiPointCrossover<int>(0));
    // TODO: children keep the parents' length; multi-point genes come from a
    //       parent; OX children are valid permutations; unequal lengths throw.
    return finishTests("crossover_tests");
}
