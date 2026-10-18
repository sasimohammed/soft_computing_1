#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>
#include "TestUtils.h"
#include "ga/GeneticAlgorithm.h"
#include "ga/chromosomes/BinaryChromosome.h"
#include "ga/chromosomes/FloatChromosome.h"
#include "ga/chromosomes/IntegerChromosome.h"
#include "ga/mutation/GaussianMutation.h"
#include "ga/mutation/RandomResetMutation.h"
#include "ga/crossover/MultiPointCrossover.h"
#include "ga/mutation/BitFlipMutation.h"
#include "ga/replacement/ElitistReplacement.h"
#include "ga/replacement/GenerationalReplacement.h"
#include "ga/selection/RouletteSelection.h"
#include "ga/selection/TournamentSelection.h"

// A 0/1 knapsack: infeasible when the total weight exceeds the capacity.
namespace {
    const std::vector<int> kWeights = {12, 7, 11, 8, 9, 6, 14, 5, 10, 13, 4, 15, 3, 9, 8, 11, 7, 6, 12, 5};
    const std::vector<int> kValues  = {24, 13, 23, 15, 16, 11, 30, 9, 19, 26, 8, 31, 5, 17, 14, 22, 12, 10, 25, 9};
    const int kCapacity = 60;

    int weightOf(const ga::Chromosome<int>& c) {
        int w = 0;
        for (std::size_t i = 0; i < c.size(); ++i) w += c[i] * kWeights[i];
        return w;
    }
    int valueOf(const ga::Chromosome<int>& c) {
        int v = 0;
        for (std::size_t i = 0; i < c.size(); ++i) v += c[i] * kValues[i];
        return v;
    }
    int optimalValue() {  // dynamic programming
        std::vector<int> best(static_cast<std::size_t>(kCapacity) + 1, 0);
        for (std::size_t i = 0; i < kWeights.size(); ++i)
            for (int w = kCapacity; w >= kWeights[i]; --w)
                best[static_cast<std::size_t>(w)] = std::max(best[static_cast<std::size_t>(w)],
                                                             best[static_cast<std::size_t>(w - kWeights[i])] + kValues[i]);
        return best[static_cast<std::size_t>(kCapacity)];
    }

    ga::GAProblem<int> knapsack(bool withRepair) {
        ga::GAProblem<int> p;
        ga::BinaryChromosomeSpec spec(kWeights.size());
        p.fitness = [](const ga::Chromosome<int>& c) { return static_cast<double>(valueOf(c)); };
        p.feasible = [](const ga::Chromosome<int>& c) { return weightOf(c) <= kCapacity; };
        p.initializer = [spec](std::mt19937& rng) { return spec.random(rng); };
        if (withRepair)  // drop random selected items until the knapsack fits
            p.repair = [](const ga::Chromosome<int>& c, std::mt19937& rng) {
                ga::Chromosome<int> fixed = c;
                while (weightOf(fixed) > kCapacity) {
                    std::vector<std::size_t> taken;
                    for (std::size_t i = 0; i < fixed.size(); ++i) if (fixed[i] == 1) taken.push_back(i);
                    std::uniform_int_distribution<std::size_t> pick(0, taken.size() - 1);
                    fixed[taken[pick(rng)]] = 0;
                }
                return fixed;
            };
        return p;
    }

    ga::GeneticAlgorithm<int> makeEngine(ga::GAConfig config, ga::GAProblem<int> problem) {
        return ga::GeneticAlgorithm<int>(
                config, std::move(problem),
                std::make_shared<ga::TournamentSelection<int>>(config.tournamentSize),
                std::make_shared<ga::MultiPointCrossover<int>>(2),
                std::make_shared<ga::BitFlipMutation<int>>(0.05),
                std::make_shared<ga::ElitistReplacement<int>>());
    }
}  // namespace

int main() {
    // ---- configuration -------------------------------------------------------
    ga::GAConfig defaults;
    defaults.validate();
    CHECK(defaults.populationSize == 100 && defaults.generations == 200);  // documented defaults
    ga::GAConfig bad;
    bad.crossoverRate = 1.5;            CHECK_THROWS(bad.validate());
    bad = ga::GAConfig(); bad.mutationRate = -0.1;  CHECK_THROWS(bad.validate());
    bad = ga::GAConfig(); bad.populationSize = 0;   CHECK_THROWS(bad.validate());
    bad = ga::GAConfig(); bad.eliteCount = 101;     CHECK_THROWS(bad.validate());
    bad = ga::GAConfig(); bad.tournamentSize = 0;   CHECK_THROWS(bad.validate());
    bad = ga::GAConfig(); bad.penaltyFactor = 0.0;  CHECK_THROWS(bad.validate());

    // ---- construction checks -------------------------------------------------
    ga::GAProblem<int> empty;
    CHECK_THROWS(makeEngine(defaults, empty));                       // missing callbacks
    ga::GAConfig repairConfig;
    repairConfig.infeasibleStrategy = ga::InfeasibleStrategy::Repair;
    CHECK_THROWS(makeEngine(repairConfig, knapsack(false)));         // repair needs a function
    CHECK_THROWS(ga::GeneticAlgorithm<int>(defaults, knapsack(false), nullptr,
                                           std::make_shared<ga::MultiPointCrossover<int>>(), std::make_shared<ga::BitFlipMutation<int>>(),
                                           std::make_shared<ga::ElitistReplacement<int>>()));           // missing operator

    // ---- each infeasibility strategy finds a (near) optimal feasible solution ---
    const int optimum = optimalValue();
    const ga::InfeasibleStrategy strategies[] = {ga::InfeasibleStrategy::Reject,
                                                 ga::InfeasibleStrategy::Repair,
                                                 ga::InfeasibleStrategy::Penalty};
    for (ga::InfeasibleStrategy strategy : strategies) {
        ga::GAConfig config;
        config.populationSize = 60;
        config.generations = 120;
        config.infeasibleStrategy = strategy;
        auto result = makeEngine(config, knapsack(true)).run();
        CHECK(result.bestFeasible);
        CHECK(weightOf(result.best) <= kCapacity);                   // returned best is feasible
        CHECK(static_cast<int>(result.bestFitness) == valueOf(result.best));
        CHECK(result.bestFitness >= 0.9 * optimum);
        CHECK(result.generationsRun == config.generations);
        CHECK(result.bestFitnessHistory.size() == config.generations);
        CHECK(result.averageFitnessHistory.size() == config.generations);
        for (std::size_t g = 1; g < result.bestFitnessHistory.size(); ++g)
            CHECK(result.bestFitnessHistory[g] >= result.bestFitnessHistory[g - 1]);  // never worse
    }

    // ---- Reject: every individual of the initial population must be feasible ---
    {
        ga::GAConfig config;
        config.populationSize = 40;
        config.generations = 1;
        config.infeasibleStrategy = ga::InfeasibleStrategy::Reject;
        auto result = makeEngine(config, knapsack(false)).run();
        CHECK(result.bestFeasible && weightOf(result.best) <= kCapacity);
    }
    // Reject cannot start if feasible individuals are impossible to find
    {
        ga::GAProblem<int> impossible = knapsack(false);
        impossible.feasible = [](const ga::Chromosome<int>&) { return false; };
        ga::GAConfig config;
        config.maxInitializationAttempts = 20;
        CHECK_THROWS(makeEngine(config, impossible).run());
    }

    // ---- Penalty keeps infeasible individuals but never returns one when a feasible exists
    {
        ga::GAConfig config;
        config.infeasibleStrategy = ga::InfeasibleStrategy::Penalty;
        config.penaltyFactor = 0.1;
        config.populationSize = 50;
        config.generations = 60;
        auto result = makeEngine(config, knapsack(false)).run();
        CHECK(result.bestFeasible);
    }

    // ---- reproducibility -------------------------------------------------------
    {
        ga::GAConfig config;
        config.populationSize = 30;
        config.generations = 30;
        config.randomSeed = 123;
        auto a = makeEngine(config, knapsack(false)).run();
        auto b = makeEngine(config, knapsack(false)).run();
        CHECK(a.best == b.best);
        CHECK(a.bestFitnessHistory == b.bestFitnessHistory);
        config.randomSeed = 124;
        auto c = makeEngine(config, knapsack(false)).run();
        CHECK(c.averageFitnessHistory != a.averageFitnessHistory);  // other seed -> other run
    }

    // ---- stagnation stops the run early ---------------------------------------------
    {
        ga::GAProblem<int> flat = knapsack(false);
        flat.fitness = [](const ga::Chromosome<int>&) { return 1.0; };  // nothing ever improves
        ga::GAConfig config;
        config.generations = 500;
        config.stagnationLimit = 7;
        auto result = makeEngine(config, flat).run();
        CHECK(result.generationsRun == 7);
    }

    // ---- time limit stops the run early ---------------------------------------------------
    {
        ga::GAConfig config;
        config.generations = 1000000;
        config.timeLimitSeconds = 0.05;
        auto result = makeEngine(config, knapsack(false)).run();
        CHECK(result.generationsRun >= 1 && result.generationsRun < 1000000);
        ga::GAConfig negative;
        negative.timeLimitSeconds = -1.0;
        CHECK_THROWS(negative.validate());
    }

    // ---- other operators plug in without touching the engine --------------------------
    {
        ga::GAConfig config;
        config.populationSize = 40;
        config.generations = 40;
        ga::GeneticAlgorithm<int> engine(
                config, knapsack(false),
                std::make_shared<ga::RouletteSelection<int>>(),
                std::make_shared<ga::MultiPointCrossover<int>>(3),
                std::make_shared<ga::BitFlipMutation<int>>(0.1),
                std::make_shared<ga::GenerationalReplacement<int>>());
        auto result = engine.run();
        CHECK(result.bestFeasible);
        CHECK(result.bestFitness > 0);
    }

    // ---- float chromosomes end to end: minimize the sphere function -------------------------
    {
        ga::FloatChromosomeSpec spec(5, -5.0, 5.0);
        ga::GAProblem<double> p;
        p.fitness = [](const ga::Chromosome<double>& c) {
            double sum = 0.0;
            for (double x : c) sum += x * x;
            return 1.0 / (1.0 + sum);
        };
        p.feasible = [spec](const ga::Chromosome<double>& c) { return spec.isValid(c); };
        p.initializer = [spec](std::mt19937& rng) { return spec.random(rng); };
        ga::GAConfig config;
        config.populationSize = 80;
        config.generations = 150;
        ga::GeneticAlgorithm<double> engine(
                config, p, std::make_shared<ga::TournamentSelection<double>>(3),
                std::make_shared<ga::MultiPointCrossover<double>>(2),
                std::make_shared<ga::GaussianMutation<double>>(0.3, -5.0, 5.0, 0.2),
                std::make_shared<ga::ElitistReplacement<double>>());
        auto result = engine.run();
        CHECK(result.bestFeasible && spec.isValid(result.best));
        CHECK(result.bestFitness > result.bestFitnessHistory.front() - 1e-12);
        CHECK(result.bestFitness > 0.5);  // sum of squares below 1 (random start is about 40)
    }

    // ---- integer (range) chromosomes end to end: maximize the sum of genes in [0,9] ---------
    {
        ga::IntegerChromosomeSpec spec(8, 0, 9);
        ga::GAProblem<int> p;
        p.fitness = [](const ga::Chromosome<int>& c) {
            double sum = 0.0;
            for (int g : c) sum += g;
            return sum;
        };
        p.feasible = [spec](const ga::Chromosome<int>& c) { return spec.isValid(c); };
        p.initializer = [spec](std::mt19937& rng) { return spec.random(rng); };
        ga::GAConfig config;
        config.populationSize = 60;
        config.generations = 80;
        ga::GeneticAlgorithm<int> engine(
                config, p, std::make_shared<ga::TournamentSelection<int>>(3),
                std::make_shared<ga::MultiPointCrossover<int>>(2),
                std::make_shared<ga::RandomResetMutation<int>>(0, 9, 0.2),
                std::make_shared<ga::ElitistReplacement<int>>());
        auto result = engine.run();
        CHECK(result.bestFeasible && spec.isValid(result.best));
        CHECK(result.bestFitness >= 68.0);  // optimum is 72
    }

    // ---- NaN fitness is reported ------------------------------------------------------
    {
        ga::GAProblem<int> broken = knapsack(false);
        broken.fitness = [](const ga::Chromosome<int>&) { return std::nan(""); };
        CHECK_THROWS(makeEngine(ga::GAConfig(), broken).run());
    }

    return finishTests("ga_core_tests");
}