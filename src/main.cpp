#include <exception>
#include <iostream>
#include "cvrp/CVRPParser.h"
#include "cvrp/CVRPPrinter.h"
#include "cvrp/CVRPSolver.h"
#include "ga/GAConfig.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "GA Toolbox scaffold ready.\n"
                  << "Usage: ga_toolbox <instance_file>\n";
        return 0;
    }
    try {
        cvrp::CVRPProblem problem = cvrp::loadProblem(argv[1]);
        problem.validate();
        ga::GAConfig config;               // defaults; override here if desired
        cvrp::CVRPSolver solver(config);
        cvrp::CVRPSolution solution = solver.solve(problem);
        cvrp::printSolution(std::cout, problem, solution);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
