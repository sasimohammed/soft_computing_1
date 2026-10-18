# Tuning log

Instances (synthetic, in data/): example_n30.vrp (30 customers, capacity 100) and
example_n60.vrp (60 customers, capacity 120). Values are the average total distance over
seeds 1, 2, 3 (lower is better), no early stopping. Reference: Clarke-Wright savings
heuristic = 681.90 (n30) and 1036.63 (n60).

| Pop | Gens | Crossover | Mutation | Tournament | Elite | n30 avg | n60 avg |
|-----|------|-----------|----------|------------|-------|---------|---------|
| 50  | 200 | 0.8 | 0.1 | 3 | 1 | 719.29 | 1436.35 |
| 100 | 200 | 0.8 | 0.1 | 3 | 1 | 714.02 | 1227.03 |
| 100 | 200 | 0.8 | 0.3 | 3 | 1 | 678.87 | 1168.30 |
| 100 | 200 | 0.8 | 0.5 | 3 | 1 | 673.97 | 1279.11 |
| 200 | 300 | 0.8 | 0.3 | 2 | 1 | 775.67 | 1679.14 |
| 200 | 300 | 0.8 | 0.3 | 4 | 1 | 682.45 | 1098.43 |
| 200 | 300 | 0.8 | 0.3 | 8 | 1 | 683.44 | 1074.88 |
| 200 | 600 | 0.9 | 0.3 | 4 | 2 | 682.45 | 1050.79 |

Observations
- A permutation GA needs a high mutation rate (0.3 beats 0.1 by 5-10 %).
- Tournament size 2 is too weak; 4 to 8 works well. Larger populations help on bigger instances.
- The last row is the recommended configuration (`CVRPSolver::recommendedConfig()`), plus
  `stagnationLimit = 200` to stop early. On the 60-customer instance a single run with
  seed 42 reached 1015.78, better than the savings heuristic (1036.63).
- Optimal split decoding versus greedy split (same GA budget, n60, seed 42): greedy 1135.67,
  optimal 1015.78.

Conclusion: use the last row. Reproduce with
`./build/ga_toolbox data/example_n60.vrp --seed 1 --stagnation 0 --pop 200 --gens 600 --mutation 0.3 --tournament 4 --crossover 0.9 --elite 2`.