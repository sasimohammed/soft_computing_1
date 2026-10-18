// Integration tests for Member 1's part: CVRPSolver + GA toolbox + configuration.
#include <algorithm>
#include <numeric>
#include <random>
#include <vector>
#include "TestUtils.h"
#include "cvrp/CVRPEvaluator.h"
#include "cvrp/CVRPSolver.h"

using namespace cvrp;

static CVRPProblem makeProblem(int customers, double capacity, int vehicles, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> coord(0, 100), demand(5, 25);
    std::vector<Location> list;
    for (int i = 1; i <= customers; ++i)
        list.push_back({i, double(coord(rng)), double(coord(rng)), double(demand(rng))});
    return CVRPProblem(Location{0, 50, 50, 0}, list, capacity, vehicles);
}

static ga::GAConfig smallConfig() {
    ga::GAConfig config = CVRPSolver::recommendedConfig();
    config.populationSize = 40;
    config.generations = 60;
    config.stagnationLimit = 0;
    return config;
}

static bool coversEveryCustomerOnce(const CVRPSolution& solution, std::size_t n) {
    std::vector<int> all;
    for (const Route& route : solution.routes) all.insert(all.end(), route.begin(), route.end());
    std::sort(all.begin(), all.end());
    std::vector<int> expected(n);
    std::iota(expected.begin(), expected.end(), 1);
    return all == expected;
}

int main() {
    // ---- configuration: defaults exist, recommended values are valid and overridable ----------
    ga::GAConfig defaults;
    defaults.validate();
    CHECK(defaults.populationSize == 100 && defaults.generations == 200);
    CHECK(defaults.infeasibleStrategy == ga::InfeasibleStrategy::Reject);
    ga::GAConfig recommended = CVRPSolver::recommendedConfig();
    recommended.validate();
    CHECK(recommended.mutationRate == 0.3 && recommended.populationSize == 200);

    // ---- repair function ---------------------------------------------------------------------
    {
        CVRPProblem p = makeProblem(6, 100.0, 0, 1);
        std::mt19937 rng(1);
        CustomerOrder valid = {3, 1, 6, 2, 5, 4};
        CHECK(CVRPSolver::repairOrder(p, valid, rng) == valid);                    // valid stays unchanged
        CustomerOrder broken = {2, 2, 9, 5, 0, 2, 1};                              // duplicates, unknown ids, missing
        CustomerOrder fixed = CVRPSolver::repairOrder(p, broken, rng);
        CHECK(CVRPDecoder::isPermutation(p, fixed));
        CHECK(fixed[0] == 2 && fixed[1] == 5 && fixed[2] == 1);                    // first occurrences keep their order
        CHECK(CVRPDecoder::isPermutation(p, CVRPSolver::repairOrder(p, {}, rng)));  // empty input
    }

    // ---- every strategy works on CVRP --------------------------------------------------------
    {
        CVRPProblem p = makeProblem(15, 60.0, 0, 3);
        const ga::InfeasibleStrategy strategies[] = {ga::InfeasibleStrategy::Reject,
                                                     ga::InfeasibleStrategy::Repair,
                                                     ga::InfeasibleStrategy::Penalty};
        double first = -1.0;
        for (ga::InfeasibleStrategy strategy : strategies) {
            ga::GAConfig config = smallConfig();
            config.infeasibleStrategy = strategy;
            CVRPSolution solution = CVRPSolver(config).solve(p);
            CHECK(solution.feasible);
            CHECK(coversEveryCustomerOnce(solution, 15));
            CHECK(CVRPEvaluator(p).isFeasible(solution.routes));
            CHECK(solution.gaResult.generationsRun == config.generations);
            // with no vehicle limit nothing is ever infeasible, so the strategy does not change the run
            if (first < 0) first = solution.totalDistance;
            CHECK(solution.totalDistance == first);
        }
    }

    // ---- configuration changes the behaviour --------------------------------------------------
    {
        CVRPProblem p = makeProblem(20, 60.0, 0, 4);
        ga::GAConfig a = smallConfig(), b = smallConfig();
        b.randomSeed = a.randomSeed + 1;
        CVRPSolution one = CVRPSolver(a).solve(p), same = CVRPSolver(a).solve(p), other = CVRPSolver(b).solve(p);
        CHECK(one.totalDistance == same.totalDistance);                             // reproducible
        CHECK(one.gaResult.averageFitnessHistory != other.gaResult.averageFitnessHistory);
        ga::GAConfig stop = smallConfig();
        stop.generations = 1000;
        stop.stagnationLimit = 5;
        CVRPSolution early = CVRPSolver(stop).solve(p);
        CHECK(early.gaResult.generationsRun < 1000);                                // stagnation stops the run
        ga::GAConfig invalid = smallConfig();
        invalid.populationSize = 0;
        CHECK_THROWS(CVRPSolver(invalid).solve(p));                                 // invalid config is rejected
    }

    // ---- vehicle limit as an extra constraint --------------------------------------------------
    {
        // reachable limit: Penalty and Repair find a solution within the limit
        CVRPProblem easy = makeProblem(12, 100.0, 6, 5);
        for (ga::InfeasibleStrategy strategy : {ga::InfeasibleStrategy::Penalty, ga::InfeasibleStrategy::Reject}) {
            ga::GAConfig config = smallConfig();
            config.infeasibleStrategy = strategy;
            CVRPSolution solution = CVRPSolver(config, true).solve(easy);
            CHECK(solution.feasible);
            CHECK(solution.routes.size() <= 6);
            CHECK(coversEveryCustomerOnce(solution, 12));
        }
        // impossible limit (one vehicle, total demand far above capacity)
        CVRPProblem hard = makeProblem(12, 60.0, 1, 6);
        ga::GAConfig penalty = smallConfig();
        penalty.infeasibleStrategy = ga::InfeasibleStrategy::Penalty;
        CVRPSolution best = CVRPSolver(penalty, true).solve(hard);
        CHECK(!best.feasible);                                                      // reported honestly
        CHECK(best.routes.size() > 1);
        ga::GAConfig reject = smallConfig();
        reject.infeasibleStrategy = ga::InfeasibleStrategy::Reject;
        reject.maxInitializationAttempts = 20;
        CHECK_THROWS(CVRPSolver(reject, true).solve(hard));                         // cannot start, clear error
    }

    // ---- unusable instance is rejected before the GA starts -----------------------------------
    CHECK_THROWS(CVRPSolver(smallConfig()).solve(CVRPProblem(Location{0, 0, 0, 0}, {{1, 1, 1, 99}}, 10.0)));

    return finishTests("solver_tests");
}