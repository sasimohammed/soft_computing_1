# Genetic Algorithm Toolbox + CVRP

C++17 base project for Assignment 1 (no external GA libraries).

## Build and test
    cmake -S . -B build
    cmake --build build
    ctest --test-dir build --output-on-failure

## Run
    ./build/ga_toolbox <instance_file>

## Layout
- include/ga/            reusable GA toolbox (problem independent)
  - operators/           abstract interfaces: Selection, Crossover, Mutation, Replacement
  - chromosomes/         binary, integer, float representations (+ validation)
  - selection/ crossover/ mutation/ replacement/   concrete operators
  - GeneticAlgorithm.h   the GA engine (takes problem callbacks + operators)
- include/cvrp/, src/cvrp/   CVRP model, parser, decoder, evaluator, solver, printer
- tests/                 one test executable per module
- docs/                  design notes, team plan, assignment checklist

## Conventions
- Higher fitness = better. For CVRP: fitness = 1 / (1 + totalDistance).
- Every operator receives the shared std::mt19937 (reproducible runs).
- Files marked TODO are scaffolding. They compile but throw std::logic_error.
