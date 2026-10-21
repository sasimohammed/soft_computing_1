# data/

CVRP instances in TSPLIB / CVRPLIB `.vrp` format. The format is described in
`docs/CVRP_FORMAT.md`.

| File | Content |
|---|---|
| `ortools_example.vrp` | The assignment's OR-Tools example: explicit distance matrix, 16 customers, 4 vehicles × capacity 15. OR-Tools objective 6208. |
| `ortools_example_coords.vrp` | Same instance as coordinates (`MAN_2D`). |
| `random_n31_k6.vrp` | Generated: 30 customers, capacity 40, rounded Euclidean. |
| `tiny.vrp` | 3 customers, can be checked by hand (optimal 36). |

Run: `./build/ga_toolbox data/ortools_example.vrp --limit-vehicles --strategy penalty`
