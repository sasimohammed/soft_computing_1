#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include "ga/operators/Crossover.h"
#include<set>
#include<vector>
namespace ga {
    // k-point crossover for binary / integer / float. NOT valid for permutations (use OrderCrossover). Parents must have equal length > points.
    template<class Gene>
    class MultiPointCrossover : public Crossover<Gene> {
    public:
        explicit MultiPointCrossover(std::size_t points = 2) : points_(points) {
            if (points_ == 0) throw std::invalid_argument("points must be > 0");
        }

        // TODO: implement. Must return two VALID children.
        std::pair<Chromosome<Gene>, Chromosome<Gene>> cross(
            const Chromosome<Gene>&p1, const Chromosome<Gene>& p2,  std::mt19937& rng) const override {

            if (p1.size() != p2.size())throw std::invalid_argument("p1 size must equal p2 size");
            if ( points_>= p2.size())throw std::invalid_argument("too many crossover points");
            Chromosome<Gene> ch1 = p1;
            Chromosome<Gene> ch2 = p2;
        std::uniform_int_distribution<std::size_t>dist(1, p1.size() - 1);
            std::set<size_t> cross_points;
            while (cross_points.size() < points_) {
                cross_points.insert(dist(rng));
            }
            bool swap_ = false;

            for (int i = 0;i < p1.size();i++) {
                if (cross_points.count(i)) swap_= !swap_;
                if (swap_)std::swap(ch1[i], ch2[i]);
            }

            return{ ch1,ch2 };
        }

        std::string name() const override { return "MultiPoint"; }
    private:
        std::size_t points_;
    };
}
