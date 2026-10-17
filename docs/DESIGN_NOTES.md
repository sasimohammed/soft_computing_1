# Design decisions (fill in and justify - you will be asked in the discussion)

- Higher fitness is better.
- CVRP chromosome: permutation of customer ids 1..n; depot is not a gene.
- Decoder: greedy split into routes by capacity.
- Infeasible handling: choose Reject / Repair / Penalty and explain why.
  For CVRP the decoder already guarantees capacity feasibility, so the toolbox
  strategy mainly matters for other problems.
- Operator compatibility:
  | Operator | Binary | Integer | Float | Permutation |
  |----------|--------|---------|-------|-------------|
  | MultiPointCrossover | yes | yes | yes | NO |
  | OrderCrossover | no | no | no | yes |
  | BitFlipMutation | yes | no | no | no |
  | SwapMutation | yes* | yes* | yes* | yes |
  | RandomResetMutation | yes | yes | yes | NO |
  | GaussianMutation | no | no | yes | no |
  (* swap is valid but weak for non-permutations)
- All operators use the shared std::mt19937 passed by the GA.
- Roulette: document how negative / near-equal fitness is handled.
- Test boundary cases, invalid inputs and solution feasibility.
