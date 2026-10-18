#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include "cvrp/CVRPParser.h"
#include "cvrp/CVRPPrinter.h"
#include "cvrp/CVRPSolver.h"
#include "ga/GAConfig.h"

namespace {
    void usage() {
        std::cerr <<
                  "Usage: ga_toolbox <instance_file> [options]\n"
                  "  --seed N          random seed\n"
                  "  --pop N           population size\n"
                  "  --gens N          generations\n"
                  "  --crossover R     crossover rate in [0,1]\n"
                  "  --mutation R      mutation rate in [0,1]\n"
                  "  --tournament N    tournament size\n"
                  "  --elite N         elite count\n"
                  "  --stagnation N    stop after N generations without improvement (0 = off)\n"
                  "  --time S          stop after S seconds (0 = off)\n"
                  "  --strategy S      reject | repair | penalty (infeasible handling)\n"
                  "  --limit-vehicles  treat the vehicle count of the instance as a hard limit\n"
                  "  --verbose         print progress\n";
    }
}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        return 0;
    }
    try {
        ga::GAConfig config = cvrp::CVRPSolver::recommendedConfig();
        bool limitVehicles = false;
        const std::string path = argv[1];
        for (int i = 2; i < argc; ++i) {
            const std::string option = argv[i];
            auto value = [&]() -> std::string {
                if (i + 1 >= argc) throw std::invalid_argument("missing value for " + option);
                return argv[++i];
            };
            if (option == "--seed") config.randomSeed = static_cast<std::uint32_t>(std::stoul(value()));
            else if (option == "--pop") config.populationSize = std::stoul(value());
            else if (option == "--gens") config.generations = std::stoul(value());
            else if (option == "--crossover") config.crossoverRate = std::stod(value());
            else if (option == "--mutation") config.mutationRate = std::stod(value());
            else if (option == "--tournament") config.tournamentSize = std::stoul(value());
            else if (option == "--elite") config.eliteCount = std::stoul(value());
            else if (option == "--stagnation") config.stagnationLimit = std::stoul(value());
            else if (option == "--time") config.timeLimitSeconds = std::stod(value());
            else if (option == "--strategy") {
                const std::string s = value();
                if (s == "reject") config.infeasibleStrategy = ga::InfeasibleStrategy::Reject;
                else if (s == "repair") config.infeasibleStrategy = ga::InfeasibleStrategy::Repair;
                else if (s == "penalty") config.infeasibleStrategy = ga::InfeasibleStrategy::Penalty;
                else throw std::invalid_argument("unknown strategy: " + s);
            }
            else if (option == "--limit-vehicles") limitVehicles = true;
            else if (option == "--verbose") config.verbose = true;
            else { usage(); throw std::invalid_argument("unknown option: " + option); }
        }

        const cvrp::CVRPProblem problem = cvrp::loadProblem(path);
        cvrp::CVRPSolver solver(config, limitVehicles);
        const auto start = std::chrono::steady_clock::now();
        const cvrp::CVRPSolution solution = solver.solve(problem);
        const double seconds =
                std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();

        cvrp::printSolution(std::cout, problem, solution);
        std::cerr << "customers: " << problem.numCustomers() << ", routes: " << solution.routes.size()
                  << ", feasible: " << (solution.feasible ? "yes" : "NO")
                  << ", generations: " << solution.gaResult.generationsRun
                  << ", time: " << seconds << " s\n";
        return solution.feasible ? 0 : 2;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}