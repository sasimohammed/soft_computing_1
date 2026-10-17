#include "TestUtils.h"
#include "ga/mutation/BitFlipMutation.h"
#include "ga/mutation/SwapMutation.h"
#include "ga/mutation/RandomResetMutation.h"
#include "ga/mutation/GaussianMutation.h"

int main() {
    ga::BitFlipMutation<int> flip(0.1);
    ga::SwapMutation<int> swap;
    ga::RandomResetMutation<int> reset(0, 10);
    ga::GaussianMutation<double> gauss(0.1, -1.0, 1.0);
    CHECK(flip.name() == "BitFlip");
    CHECK(swap.name() == "Swap");
    CHECK(reset.name() == "RandomReset");
    CHECK(gauss.name() == "Gaussian");
    CHECK_THROWS(ga::BitFlipMutation<int>(1.5));
    CHECK_THROWS(ga::RandomResetMutation<int>(5, 1));
    // TODO: bit flip stays in {0,1}; swap preserves the multiset of genes;
    //       reset stays within bounds; rate 0 changes nothing, rate 1 changes all.
    return finishTests("mutation_tests");
}
