#include "cvrp/CVRPSolver.h"
#include <algorithm>
#include <memory>
#include <numeric>
#include <random>
#include "cvrp/CVRPEvaluator.h"
#include "ga/GeneticAlgorithm.h"
#include "ga/chromosomes/IntegerChromosome.h"
#include "ga/crossover/OrderCrossover.h"
#include "ga/mutation/InversionMutation.h"
#include "ga/mutation/SwapMutation.h"
#include "ga/replacement/ElitistReplacement.h"
#include "ga/selection/TournamentSelection.h"

namespace cvrp {
    namespace {
// Mixes two permutation mutations: the GA still sees a single Mutation object.
        class SwapOrInversionMutation : public ga::Mutation<int> {
        public:
            void mutate(ga::Chromosome<int>& chromosome, std::mt19937& rng) const override {
                if (std::uniform_int_distribution<int>(0, 1)(rng) == 0) swap_.mutate(chromosome, rng);
                else inversion_.mutate(chromosome, rng);
            }
            std::string name() const override { return "Swap+Inversion"; }

        private:
            ga::SwapMutation<int> swap_;
            ga::InversionMutation<int> inversion_;
        };
    }  // namespace

    CustomerOrder CVRPSolver::repairOrder(const CVRPProblem& problem, const CustomerOrder& order,
                                          std::mt19937& rng) {
        const std::size_t n = problem.numCustomers();
        std::vector<char> seen(n + 1, 0);
        CustomerOrder fixed;
        fixed.reserve(n);
        for (int id : order) {
            if (id < 1 || static_cast<std::size_t>(id) > n || seen[static_cast<std::size_t>(id)]) continue;
            seen[static_cast<std::size_t>(id)] = 1;
            fixed.push_back(id);
        }
        CustomerOrder missing;
        for (std::size_t id = 1; id <= n; ++id)
            if (!seen[id]) missing.push_back(static_cast<int>(id));
        std::shuffle(missing.begin(), missing.end(), rng);
        fixed.insert(fixed.end(), missing.begin(), missing.end());
        return fixed;
    }

    CVRPSolution CVRPSolver::solve(const CVRPProblem& problem) {
        problem.validate();
        const CVRPEvaluator evaluator(problem, enforceVehicleLimit_);
        const std::size_t n = problem.numCustomers();

        CVRPSolution solution;
        if (n < 2) {  // nothing to search: a single customer
            CustomerOrder order(n);
            std::iota(order.begin(), order.end(), 1);
            solution.routes = evaluator.decoder().decode(problem, order);
            solution.totalDistance = evaluator.totalDistance(solution.routes);
            solution.feasible = evaluator.isFeasible(solution.routes);
            return solution;
        }

        const ga::IntegerChromosomeSpec spec(n, 1, static_cast<int>(n), true);

        ga::GAProblem<int> gaProblem;
        gaProblem.fitness = [evaluator](const ga::Chromosome<int>& c) { return evaluator.fitness(c); };
        gaProblem.feasible = [evaluator](const ga::Chromosome<int>& c) { return evaluator.isFeasible(c); };
        gaProblem.initializer = [spec](std::mt19937& rng) { return spec.random(rng); };
        // Orders stay valid permutations with Order crossover and swap/inversion, and the decoder
        // guarantees capacity, so infeasibility only appears with the optional vehicle limit.
        // The repair function (used by the Repair strategy) restores a valid permutation; it cannot
        // reduce the number of routes, so use Penalty or Reject together with --limit-vehicles.
        gaProblem.repair = [&problem](const ga::Chromosome<int>& c, std::mt19937& rng) {
            return CVRPSolver::repairOrder(problem, c, rng);
        };

        ga::GeneticAlgorithm<int> engine(
                config_, gaProblem,
                std::make_shared<ga::TournamentSelection<int>>(config_.tournamentSize),
                std::make_shared<ga::OrderCrossover<int>>(),
                std::make_shared<SwapOrInversionMutation>(),
                std::make_shared<ga::ElitistReplacement<int>>());

        solution.gaResult = engine.run();
        solution.routes = evaluator.decoder().decode(problem, solution.gaResult.best);
        solution.totalDistance = evaluator.totalDistance(solution.routes);
        solution.feasible = evaluator.isFeasible(solution.routes);
        return solution;
    }
}