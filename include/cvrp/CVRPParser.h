#pragma once
#include <istream>
#include <string>
#include "cvrp/CVRPProblem.h"

namespace cvrp {
// Reads a CVRP instance in the TSPLIB / CVRPLIB ".vrp" format (see docs/CVRP_FORMAT.md):
//
//   NAME : ortools_example
//   TYPE : CVRP
//   DIMENSION : 17              (depot + customers)
//   CAPACITY : 15
//   VEHICLES : 4                (optional; else taken from a "-kN" suffix of NAME, else 0)
//   EDGE_WEIGHT_TYPE : MAN_2D   (EUC_2D | EXACT_2D | MAN_2D | EXPLICIT)
//   EDGE_WEIGHT_FORMAT : FULL_MATRIX   (EXPLICIT only)
//   NODE_COORD_SECTION / EDGE_WEIGHT_SECTION / DEMAND_SECTION / DEPOT_SECTION
//   EOF
//
// Node labels in the file can be any integers. They are renumbered: the depot becomes id 0
// and the other nodes become 1..n in ascending label order.
// Throws std::runtime_error (with file name and line number) on bad input.
CVRPProblem loadProblem(const std::string& path);

// Same, reading from an already opened stream. `sourceName` is only used in error messages.
CVRPProblem parseProblem(std::istream& in, const std::string& sourceName = "<input>");
}
