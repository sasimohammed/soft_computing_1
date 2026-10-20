#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include "ga/Chromosome.h"
#include "ga/chromosomes/BinaryChromosome.h"
#include "ga/mutation/BitFlipMutation.h"

using namespace ga;

int countOnes(const BinaryChromosome &c)
{
    int s = 0;
    for (std::size_t i = 0; i < c.size(); ++i)
        s += c[i];
    return s;
}

int main()
{
    std::mt19937 rng(7);
    const std::size_t N = 40;
    const int POP = 30;
    const int GENS = 100;

    BinaryChromosomeSpec spec(N);
    BitFlipMutation<int> mutation(1.0 / N);

    std::vector<BinaryChromosome> pop;
    for (int i = 0; i < POP; ++i)
        pop.push_back(spec.random(rng));

    auto better = [](const BinaryChromosome &a, const BinaryChromosome &b)
    {
        return countOnes(a) > countOnes(b);
    };

    for (int g = 0; g < GENS; ++g)
    {
        std::sort(pop.begin(), pop.end(), better);
        for (int i = POP / 2; i < POP; ++i)
        {
            pop[i] = pop[i - POP / 2];
            mutation.mutate(pop[i], rng);
        }
        if (g % 20 == 0)
            std::cout << "generation " << g << " best = " << countOnes(pop[0]) << "/" << N << "\n";
    }
    std::sort(pop.begin(), pop.end(), better);
    std::cout << "final best = " << countOnes(pop[0]) << "/" << N << "\n";
}