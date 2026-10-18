#pragma once
#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
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

// Problem independent genetic algorithm. All problem knowledge arrives through
// GAProblem (fitness, feasibility, initializer, optional repair); all search
// behaviour arrives through the four operator objects. Higher fitness = better.
//
// Infeasible chromosomes (see GAConfig::infeasibleStrategy):
//   Reject  - a child that is infeasible is discarded and another one is bred
//             (at most maxOffspringAttempts * populationSize failures per
//             generation, then parents are cloned so the loop always ends).
//   Repair  - the user's repair function is applied; if the result is still
//             infeasible it is rejected as above.
//   Penalty - infeasible chromosomes stay in the population but their fitness
//             is reduced (penaltyFactor) for selection and replacement.
// The returned best solution is always the best FEASIBLE one when one exists.
//
// The run stops when ANY of these happens: `generations` are done, the best has not
// improved for `stagnationLimit` generations (0 = off), or `timeLimitSeconds` elapsed (0 = off).
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

        GAResult<Gene> run() {
            const std::size_t size = config_.populationSize;

            PopulationType population = initializePopulation();
            Evaluation current = evaluate(population);

            GAResult<Gene> result;
            bool haveBest = false;
            updateBest(population, current, result, haveBest);

            const auto start = std::chrono::steady_clock::now();
            std::size_t stagnant = 0;
            for (std::size_t generation = 0; generation < config_.generations; ++generation) {
                PopulationType offspring = breed(population, current.effective);
                Evaluation offspringEval = evaluate(offspring);

                population = replacement_->replace(population, offspring, current.effective,
                                                   offspringEval.effective, size,
                                                   config_.eliteCount, rng_);
                if (population.size() != size)
                    throw std::logic_error("Replacement returned a population of the wrong size");
                current = evaluate(population);

                bool improved = updateBest(offspring, offspringEval, result, haveBest);
                improved = updateBest(population, current, result, haveBest) || improved;

                result.generationsRun = generation + 1;
                result.bestFitnessHistory.push_back(result.bestFitness);
                result.averageFitnessHistory.push_back(average(current.raw));

                if (config_.verbose && (generation % 10 == 0 || generation + 1 == config_.generations))
                    std::cout << "generation " << (generation + 1) << " best "
                              << result.bestFitness << " avg "
                              << result.averageFitnessHistory.back() << '\n';

                stagnant = improved ? 0 : stagnant + 1;
                if (config_.stagnationLimit > 0 && stagnant >= config_.stagnationLimit) break;
                if (config_.timeLimitSeconds > 0 &&
                    std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() >=
                    config_.timeLimitSeconds)
                    break;
            }
            return result;
        }

        const GAConfig& config() const { return config_; }

    private:
        struct Evaluation {
            std::vector<double> raw;        // user fitness
            std::vector<double> effective;  // fitness after the infeasibility penalty
            std::vector<char> feasible;
        };

        double penalize(double fitness) const {
            return fitness >= 0 ? fitness * config_.penaltyFactor
                                : fitness / config_.penaltyFactor;
        }

        Evaluation evaluate(const PopulationType& population) const {
            Evaluation eval;
            eval.raw.reserve(population.size());
            eval.effective.reserve(population.size());
            eval.feasible.reserve(population.size());
            for (const ChromosomeType& chromosome : population) {
                const double fitness = problem_.fitness(chromosome);
                if (std::isnan(fitness))
                    throw std::runtime_error("fitness function returned NaN");
                const bool ok = problem_.feasible(chromosome);
                eval.raw.push_back(fitness);
                eval.effective.push_back(ok ? fitness : penalize(fitness));
                eval.feasible.push_back(ok ? 1 : 0);
            }
            return eval;
        }

        static double average(const std::vector<double>& values) {
            if (values.empty()) return 0.0;
            double sum = 0.0;
            for (double v : values) sum += v;
            return sum / static_cast<double>(values.size());
        }

        // Feasible beats infeasible; otherwise higher raw fitness wins.
        static bool better(bool feasibleA, double fitnessA, bool feasibleB, double fitnessB) {
            if (feasibleA != feasibleB) return feasibleA;
            return fitnessA > fitnessB + 1e-12;
        }

        bool updateBest(const PopulationType& population, const Evaluation& eval,
                        GAResult<Gene>& result, bool& haveBest) const {
            bool improved = false;
            for (std::size_t i = 0; i < population.size(); ++i) {
                const bool ok = eval.feasible[i] != 0;
                if (!haveBest || better(ok, eval.raw[i], result.bestFeasible, result.bestFitness)) {
                    result.best = population[i];
                    result.bestFitness = eval.raw[i];
                    result.bestFeasible = ok;
                    haveBest = true;
                    improved = true;
                }
            }
            return improved;
        }

        // Applies the infeasibility strategy. Returns nothing if the chromosome is rejected.
        std::optional<ChromosomeType> finalize(ChromosomeType chromosome) {
            if (problem_.feasible(chromosome)) return chromosome;
            switch (config_.infeasibleStrategy) {
                case InfeasibleStrategy::Penalty:
                    return chromosome;
                case InfeasibleStrategy::Repair: {
                    ChromosomeType repaired = problem_.repair(chromosome, rng_);
                    if (problem_.feasible(repaired)) return repaired;
                    return std::nullopt;
                }
                case InfeasibleStrategy::Reject:
                    return std::nullopt;
            }
            return std::nullopt;
        }

        PopulationType initializePopulation() {
            PopulationType population;
            population.reserve(config_.populationSize);
            for (std::size_t i = 0; i < config_.populationSize; ++i) {
                bool placed = false;
                for (std::size_t attempt = 0; attempt < config_.maxInitializationAttempts; ++attempt) {
                    std::optional<ChromosomeType> accepted = finalize(problem_.initializer(rng_));
                    if (accepted) {
                        population.push_back(std::move(*accepted));
                        placed = true;
                        break;
                    }
                }
                if (!placed)
                    throw std::runtime_error(
                            "Could not create a feasible initial chromosome within "
                            "maxInitializationAttempts; use Repair or Penalty, or raise the limit");
            }
            return population;
        }

        PopulationType breed(const PopulationType& population, const std::vector<double>& fitness) {
            const std::size_t size = config_.populationSize;
            std::uniform_real_distribution<double> unit(0.0, 1.0);
            PopulationType offspring;
            offspring.reserve(size);

            std::size_t failures = 0;
            const std::size_t maxFailures = config_.maxOffspringAttempts * size;

            while (offspring.size() < size) {
                ChromosomeType parentA = selection_->select(population, fitness, rng_);
                ChromosomeType parentB = selection_->select(population, fitness, rng_);

                std::pair<ChromosomeType, ChromosomeType> children;
                if (unit(rng_) < config_.crossoverRate)
                    children = crossover_->cross(parentA, parentB, rng_);
                else
                    children = {parentA, parentB};

                for (ChromosomeType* child : {&children.first, &children.second}) {
                    if (offspring.size() >= size) break;
                    if (unit(rng_) < config_.mutationRate) mutation_->mutate(*child, rng_);
                    std::optional<ChromosomeType> accepted = finalize(std::move(*child));
                    if (accepted) offspring.push_back(std::move(*accepted));
                    else ++failures;
                }

                if (failures > maxFailures) {
                    // Too many rejections: fill the rest with selected parents so the
                    // generation always completes.
                    while (offspring.size() < size)
                        offspring.push_back(selection_->select(population, fitness, rng_));
                }
            }
            return offspring;
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