#pragma once
#include <cstddef>
#include <vector>
#include "ga/Chromosome.h"

namespace ga {
    template<class Gene>
    struct GAResult {
        Chromosome<Gene> best;
        double bestFitness = 0.0;
        bool bestFeasible = false;
        std::size_t generationsRun = 0;
        std::vector<double> bestFitnessHistory;     // one entry per generation
        std::vector<double> averageFitnessHistory;  // one entry per generation
    };
}