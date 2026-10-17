#include <memory>
#include "TestUtils.h"
#include "ga/GeneticAlgorithm.h"
#include "ga/selection/TournamentSelection.h"
#include "ga/crossover/OrderCrossover.h"
#include "ga/mutation/SwapMutation.h"
#include "ga/replacement/ElitistReplacement.h"

static ga::GAProblem<int> dummyProblem() {
    ga::GAProblem<int> p;
    p.fitness = [](const ga::Chromosome<int>& c) { return static_cast<double>(c.size()); };
    p.feasible = [](const ga::Chromosome<int>&) { return true; };
    p.initializer = [](std::mt19937&) { return ga::Chromosome<int>{1, 2, 3}; };
    return p;
}

int main() {
    ga::GAConfig defaults;
    defaults.validate();

    ga::GAConfig bad;
    bad.crossoverRate = 1.5;
    CHECK_THROWS(bad.validate());

    // Wiring: the engine accepts a problem + the four operators.
    ga::GeneticAlgorithm<int> engine(
        defaults, dummyProblem(),
        std::make_shared<ga::TournamentSelection<int>>(3),
        std::make_shared<ga::OrderCrossover<int>>(),
        std::make_shared<ga::SwapMutation<int>>(),
        std::make_shared<ga::ElitistReplacement<int>>());
    CHECK(engine.config().populationSize == 100);

    // Missing callbacks must be rejected.
    ga::GAProblem<int> empty;
    CHECK_THROWS(ga::GeneticAlgorithm<int>(
        defaults, empty,
        std::make_shared<ga::TournamentSelection<int>>(3),
        std::make_shared<ga::OrderCrossover<int>>(),
        std::make_shared<ga::SwapMutation<int>>(),
        std::make_shared<ga::ElitistReplacement<int>>()));

    // TODO: once run() exists - same seed gives same result; best fitness is
    //       non-decreasing with elitism; infeasible strategies behave as documented.
    return finishTests("ga_core_tests");
}
