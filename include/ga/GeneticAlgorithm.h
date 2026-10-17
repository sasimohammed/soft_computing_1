#pragma once
#include <memory>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "ga/Chromosome.h"
#include "ga/GAConfig.h"
#include "ga/GAProblem.h"
#include "ga/GAResult.h"
#include "ga/operators/Selection.h"
#include "ga/operators/Crossover.h"
#include "ga/operators/Mutation.h"
#include "ga/operators/Replacement.h"

namespace ga {
template<class Gene>
class GeneticAlgorithm {
public:
    using ChromosomeType = Chromosome<Gene>;
    using PopulationType = Population<Gene>;

    GeneticAlgorithm(
        GAConfig config,
        GAProblem<Gene> problem,
        std::shared_ptr<Selection<Gene>> selection,
        std::shared_ptr<Crossover<Gene>> crossover,
        std::shared_ptr<Mutation<Gene>> mutation,
        std::shared_ptr<Replacement<Gene>> replacement)
        : config_(std::move(config)),
          problem_(std::move(problem)),
          selection_(std::move(selection)),
          crossover_(std::move(crossover)),
          mutation_(std::move(mutation)),
          replacement_(std::move(replacement)),
          rng_(config_.randomSeed) {
        config_.validate();
        if (!problem_.fitness || !problem_.feasible || !problem_.initializer)
            throw std::invalid_argument("fitness, feasible and initializer are required");
        if (config_.infeasibleStrategy == InfeasibleStrategy::Repair && !problem_.repair)
            throw std::invalid_argument("Repair strategy requires a repair function");
        if (!selection_ || !crossover_ || !mutation_ || !replacement_)
            throw std::invalid_argument("All four operators are required");
    }

    // TODO (core loop):
    //  1. population = initializePopulation()
    //  2. evaluate fitness (use effectiveFitness for the Penalty strategy)
    //  3. repeat for config_.generations:
    //       - select two parents, crossover with probability crossoverRate
    //       - mutate each child with probability mutationRate
    //       - handle infeasible children (config_.infeasibleStrategy)
    //       - evaluate offspring, build next generation with replacement_
    //       - record best / average fitness history, honor stagnationLimit
    //  4. return the best FEASIBLE chromosome found
    GAResult<Gene> run() {
        throw std::logic_error("GeneticAlgorithm::run is not implemented");
    }

    const GAConfig& config() const { return config_; }

private:
    // TODO: create populationSize feasible chromosomes (retry up to
    // maxInitializationAttempts per individual, then throw or repair).
    PopulationType initializePopulation() {
        throw std::logic_error("initializePopulation is not implemented");
    }

    // TODO: fitness used for selection/replacement (applies the penalty if needed).
    double effectiveFitness(const ChromosomeType&) const {
        throw std::logic_error("effectiveFitness is not implemented");
    }

    // TODO: apply Reject / Repair / Penalty to an infeasible chromosome.
    ChromosomeType handleInfeasible(const ChromosomeType&) {
        throw std::logic_error("handleInfeasible is not implemented");
    }

    GAConfig config_;
    GAProblem<Gene> problem_;
    std::shared_ptr<Selection<Gene>> selection_;
    std::shared_ptr<Crossover<Gene>> crossover_;
    std::shared_ptr<Mutation<Gene>> mutation_;
    std::shared_ptr<Replacement<Gene>> replacement_;
    std::mt19937 rng_;
};
}
