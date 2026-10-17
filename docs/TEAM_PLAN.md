# Team plan (4-5 members)

| # | Role | Owns |
|---|------|------|
| 1 | Core / integration | GeneticAlgorithm.h, GAConfig, infeasibility handling, CMake, README |
| 2 | Selection / crossover / replacement | Tournament, Roulette, MultiPoint, Order, Generational, Elitist + tests |
| 3 | Chromosomes / mutation | Binary, Integer, Float specs; BitFlip, Swap, RandomReset (+Gaussian) + tests |
| 4 | CVRP model | CVRPProblem, Parser, Decoder, Evaluator + tests |
| 5 (optional) | CVRP solver / output | CVRPSolver, Printer, tuning, report |

Branches: feature/ga-core, feature/operators, feature/chromosomes-mutation,
feature/cvrp-model, feature/cvrp-solver. Open pull requests; keep main buildable.

Rules
- Interfaces in include/ga/operators are FROZEN after the first merge.
- Rotate code reviews so every member can explain the full GA loop and CVRP decoding.
- Implement GeneticAlgorithm::run early so operators can be tested inside it.
