#include "TestUtils.h"
#include "ga/selection/TournamentSelection.h"
#include "ga/selection/RouletteSelection.h"

int main() {
    ga::TournamentSelection<int> tournament(3);
    ga::RouletteSelection<int> roulette;
    CHECK(tournament.name() == "Tournament");
    CHECK(roulette.name() == "Roulette");
    CHECK_THROWS(ga::TournamentSelection<int>(0));
    // TODO: fittest wins with large tournament; roulette favors fitter
    //       individuals statistically; negative/zero fitness; empty population.
    return finishTests("selection_tests");
}
