# CVRP instance format and model (Member 4)

## 1. Input file format

Instances use the standard **TSPLIB / CVRPLIB `.vrp` format**, so any instance from
CVRPLIB (e.g. the Augerat A/B/P sets) can be loaded without changes.

```
NAME : ortools_example            optional, a "-kN" suffix gives the vehicle count (A-n32-k5 -> 5)
COMMENT : free text               optional, ignored
TYPE : CVRP                       optional, must be CVRP if present
DIMENSION : 17                    optional, number of nodes INCLUDING the depot (checked)
CAPACITY : 15                     required, capacity of every vehicle (homogeneous fleet)
VEHICLES : 4                      optional extension, number of available vehicles (0/absent = unlimited)
EDGE_WEIGHT_TYPE : EXPLICIT       EUC_2D | EXACT_2D | MAN_2D | EXPLICIT  (default EXACT_2D)
EDGE_WEIGHT_FORMAT : FULL_MATRIX  EXPLICIT only: FULL_MATRIX | UPPER_ROW | LOWER_ROW |
                                                 UPPER_DIAG_ROW | LOWER_DIAG_ROW
NODE_COORD_SECTION                "label x y" per line   (required unless EXPLICIT)
EDGE_WEIGHT_SECTION               the matrix numbers     (EXPLICIT only)
DEMAND_SECTION                    "label demand" per line (required, depot demand = 0)
DEPOT_SECTION                     depot label, terminated by -1 (optional, default = first node)
EOF
```

| EDGE_WEIGHT_TYPE | Distance | Note |
|---|---|---|
| `EXACT_2D` | sqrt(dx² + dy²) | real Euclidean distance |
| `EUC_2D` | round(sqrt(dx² + dy²)) | TSPLIB rule, needed to compare with CVRPLIB optimal values |
| `MAN_2D` | \|dx\| + \|dy\| | city blocks (the OR-Tools example) |
| `EXPLICIT` | given matrix | copied as-is |

**Renumbering.** Node labels in the file can be any integers (0-based or 1-based). Internally
the depot becomes id `0` and the other nodes become `1..n` in ascending label order. In the
OR-Tools example the depot is node 0, so the printed ids are exactly the OR-Tools ids.

**Errors.** Every problem is reported as `std::runtime_error("file:line: message")`:
missing capacity, wrong dimension, negative demand, a demand larger than the capacity (no
vehicle could ever serve that customer), a node without coordinates or demand, duplicate
nodes, more than one depot, an unsupported metric, or a matrix with the wrong number of values.

All distances are computed once when the instance is loaded and kept in an (n+1)×(n+1)
matrix, because the GA asks for distances millions of times.

## 2. Files in `data/`

| File | Content |
|---|---|
| `ortools_example.vrp` | The assignment's example (OR-Tools CVRP): 16 customers, 4 vehicles × 15, the same distance matrix. OR-Tools objective: **6208**. |
| `ortools_example_coords.vrp` | Same instance given as coordinates with `MAN_2D` (gives the same matrix). |
| `random_n31_k6.vrp` | Generated: 30 customers, capacity 40, total demand 203 (≥ 6 vehicles), `EUC_2D`. |
| `tiny.vrp` | 3 customers, can be checked by hand (optimal = 36). |

## 3. Model: representation and decoding

**Chromosome = permutation of customer ids 1..n** (integer chromosome, permutation mode).
The depot is not a gene and there are no route separators in the chromosome.

**Decoder (CVRPDecoder)** cuts the permutation into consecutive routes:

* `Optimal` (default): Prins' *Split*. Builds a graph where node j means "the first j customers
  of the order are served" and arc i→j means "one vehicle serves customers i+1..j". An arc
  only exists if those customers fit in the capacity, and its weight is the route cost
  depot→…→depot. The shortest path 0→n is the cheapest way to cut this order. Complexity is
  O(n·L), where L is the most customers that fit in one vehicle.
* `Greedy`: fill a vehicle until the next customer does not fit. This uses the fewest
  vehicles for the given order but may cost more distance. It is kept for comparison.
* With a vehicle limit K the split becomes a layered shortest path (at most K arcs). If this
  order cannot be cut into ≤ K routes, the decoder returns the greedy cut (fewest vehicles)
  and the evaluator marks it infeasible.

Why this design:
1. Every permutation decodes to routes that respect capacity. Crossover (OX) and swap or
   inversion mutation keep a permutation a permutation, so the GA never produces a
   capacity-infeasible solution and no fitness is wasted on infeasible solutions.
2. The GA only has to search the visiting order, while the split finds the best route
   boundaries for that order exactly. There is always at least one permutation whose optimal
   split is an optimal CVRP solution, so no solution is out of reach.
3. A brute-force test confirms that the split equals the best of all 2^(n-1) possible
   cuts (`tests/cvrp_tests.cpp`).

## 4. Constraint handling (feasibility check)

`CVRPEvaluator::isFeasible(routes)` checks every constraint itself and does not rely on the
decoder having done it:

| | Constraint |
|---|---|
| C1 | each customer 1..n is visited exactly once (no duplicates, none missing) |
| C2 | routes contain only valid customer ids (the depot is implied, never listed) |
| C3 | the load of every route ≤ capacity |
| C4 | number of used vehicles ≤ VEHICLES, only if the vehicle limit is enforced (`--limit-vehicles`) |

`isFeasible(order)` is the function given to the GA: it returns false for a chromosome that
is not a permutation, and otherwise decodes the order and checks C1 to C4.
`violations(routes)` returns readable messages, which the printer shows if the final solution
is infeasible.

## 5. Fitness

`fitness(order) = 1 / (1 + cost(order))`, with values in (0, 1], and higher is better (the
toolbox convention). It is a strictly decreasing function of distance, so it ranks solutions
exactly like total distance. A non-permutation gets 0, the worst value.

`cost = total distance + penalty`. The penalty is 0 for feasible solutions. It only applies
when C4 is enforced and broken:
`penalty = W × (extra vehicles + demand on the extra routes / capacity)`, where W is twice the
distance to the farthest customer (roughly the cost of one extra trip). The second term
rewards orders that are almost feasible, which gives the GA a direction to move in. On the
OR-Tools instance this matters because total demand is exactly 4 × 15, so the limit can only
be met by perfectly packed routes.

## 6. Output format

Same layout as the OR-Tools example. `Load(x)` is the load when the vehicle arrives at the
node. Unused vehicles of a fixed fleet are printed as `0 -> 0`:

```
Objective: 6208
Route for vehicle 0:
 0 Load(0) ->  7 Load(0) ->  4 Load(8) ->  3 Load(12) ->  1 Load(14) ->  0 Load(15)
Distance of the route: 1552m
Load of the route: 15
...
Total distance of all routes: 6208m
Total load of all routes: 60
```
