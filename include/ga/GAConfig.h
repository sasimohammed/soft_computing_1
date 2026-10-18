#pragma once
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace ga {

// How the GA treats infeasible chromosomes (justify your choice in the report):
//  Reject  - discard and regenerate (up to maxInitializationAttempts / maxOffspringAttempts)
//  Repair  - fix it with the user-supplied repair function
//  Penalty - keep it but reduce its fitness by penaltyFactor
    enum class InfeasibleStrategy { Reject, Repair, Penalty };

    struct GAConfig {
        // Every field has a default; the user may override any of them.
        std::size_t populationSize = 100;
        std::size_t generations = 200;
        double crossoverRate = 0.80;
        double mutationRate = 0.10;
        std::size_t tournamentSize = 3;
        std::size_t eliteCount = 1;
        InfeasibleStrategy infeasibleStrategy = InfeasibleStrategy::Reject;
        std::size_t maxInitializationAttempts = 1000;
        std::size_t maxOffspringAttempts = 50;
        double penaltyFactor = 0.5;       // in (0,1]. Infeasible fitness f becomes f*factor (f>=0) or f/factor (f<0)
        std::size_t stagnationLimit = 0;  // 0 = disabled; else stop after N generations with no improvement
        double timeLimitSeconds = 0.0;    // 0 = disabled; else stop when this wall-clock time is used up
        std::uint32_t randomSeed = 42;
        bool verbose = false;

        void validate() const {
            if (populationSize == 0)
                throw std::invalid_argument("populationSize must be > 0");
            if (generations == 0)
                throw std::invalid_argument("generations must be > 0");
            if (crossoverRate < 0 || crossoverRate > 1)
                throw std::invalid_argument("crossoverRate must be in [0,1]");
            if (mutationRate < 0 || mutationRate > 1)
                throw std::invalid_argument("mutationRate must be in [0,1]");
            if (tournamentSize == 0)
                throw std::invalid_argument("tournamentSize must be > 0");
            if (eliteCount > populationSize)
                throw std::invalid_argument("eliteCount exceeds populationSize");
            if (maxInitializationAttempts == 0)
                throw std::invalid_argument("maxInitializationAttempts must be > 0");
            if (maxOffspringAttempts == 0)
                throw std::invalid_argument("maxOffspringAttempts must be > 0");
            if (timeLimitSeconds < 0)
                throw std::invalid_argument("timeLimitSeconds must be >= 0");
            if (penaltyFactor <= 0 || penaltyFactor > 1)
                throw std::invalid_argument("penaltyFactor must be in (0,1]");
        }
    };
}