#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include "TestUtils.h"
#include "ga/chromosomes/BinaryChromosome.h"
#include "ga/chromosomes/IntegerChromosome.h"
#include "ga/chromosomes/FloatChromosome.h"
#include "ga/mutation/BitFlipMutation.h"
#include "ga/mutation/SwapMutation.h"
#include "ga/mutation/RandomResetMutation.h"
#include "ga/mutation/GaussianMutation.h"

template<class Gene>
std::vector<Gene> sortedGenes(const ga::Chromosome<Gene>& c) {
    std::vector<Gene> v;
    for (std::size_t i = 0; i < c.size(); ++i) v.push_back(c[i]);
    std::sort(v.begin(), v.end());
    return v;
}

template<class Gene>
std::size_t countDifferences(const ga::Chromosome<Gene>& a, const ga::Chromosome<Gene>& b) {
    std::size_t d = 0;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) ++d;
    return d;
}

int main() {
    ga::BitFlipMutation<int> flip(0.1);
    ga::SwapMutation<int> swap;
    ga::RandomResetMutation<int> reset(0, 10);
    ga::GaussianMutation<double> gauss(0.1, -1.0, 1.0);
    CHECK(flip.name() == "BitFlip");
    CHECK(swap.name() == "Swap");
    CHECK(reset.name() == "RandomReset");
    CHECK(gauss.name() == "Gaussian");
    CHECK_THROWS(ga::BitFlipMutation<int>(1.5));
    CHECK_THROWS(ga::RandomResetMutation<int>(5, 1));

    std::mt19937 rng(42);

    ga::BinaryChromosomeSpec bin(50);

    bool flipStaysBinary = true;
    for (int t = 0; t < 200; ++t) {
        ga::BinaryChromosome c = bin.random(rng);
        ga::BitFlipMutation<int>(0.3).mutate(c, rng);
        if (!bin.isValid(c)) flipStaysBinary = false;
    }
    CHECK(flipStaysBinary);

    ga::BinaryChromosome flipBefore = bin.random(rng);
    ga::BinaryChromosome flipZeroRate = flipBefore;
    ga::BitFlipMutation<int>(0.0).mutate(flipZeroRate, rng);
    CHECK(countDifferences(flipBefore, flipZeroRate) == 0);

    ga::BinaryChromosome flipFullRate = flipBefore;
    ga::BitFlipMutation<int>(1.0).mutate(flipFullRate, rng);
    CHECK(countDifferences(flipBefore, flipFullRate) == flipBefore.size());
    CHECK(bin.isValid(flipFullRate));

    ga::BinaryChromosome flipPartial = flipBefore;
    ga::BitFlipMutation<int>(0.5).mutate(flipPartial, rng);
    std::size_t partialDiff = countDifferences(flipBefore, flipPartial);
    CHECK(partialDiff > 0);
    CHECK(partialDiff < flipBefore.size());

    ga::IntegerChromosomeSpec perm(12, 1, 12, true);
    bool swapKeepsMultiset = true;
    bool swapKeepsPermutation = true;
    bool swapChangesTwo = true;
    for (int t = 0; t < 500; ++t) {
        ga::IntegerChromosome c = perm.random(rng);
        ga::IntegerChromosome before = c;
        ga::SwapMutation<int>().mutate(c, rng);
        if (sortedGenes(c) != sortedGenes(before)) swapKeepsMultiset = false;
        if (!perm.isValid(c)) swapKeepsPermutation = false;
        if (countDifferences(before, c) != 2) swapChangesTwo = false;
    }
    CHECK(swapKeepsMultiset);
    CHECK(swapKeepsPermutation);
    CHECK(swapChangesTwo);

    ga::IntegerChromosomeSpec repeated(10, 0, 3);
    bool swapRepeatedKeepsMultiset = true;
    for (int t = 0; t < 200; ++t) {
        ga::IntegerChromosome c = repeated.random(rng);
        ga::IntegerChromosome before = c;
        ga::SwapMutation<int>().mutate(c, rng);
        if (sortedGenes(c) != sortedGenes(before)) swapRepeatedKeepsMultiset = false;
    }
    CHECK(swapRepeatedKeepsMultiset);

    ga::IntegerChromosome single(std::vector<int>{1});
    CHECK_THROWS(ga::SwapMutation<int>().mutate(single, rng));
    ga::IntegerChromosome empty(std::vector<int>{});
    CHECK_THROWS(ga::SwapMutation<int>().mutate(empty, rng));

    ga::IntegerChromosomeSpec range(60, 0, 10);
    bool resetInBounds = true;
    for (int t = 0; t < 200; ++t) {
        ga::IntegerChromosome c = range.random(rng);
        ga::RandomResetMutation<int>(0, 10, 0.5).mutate(c, rng);
        if (!range.isValid(c)) resetInBounds = false;
    }
    CHECK(resetInBounds);

    ga::IntegerChromosome resetBefore = range.random(rng);
    ga::IntegerChromosome resetZeroRate = resetBefore;
    ga::RandomResetMutation<int>(0, 10, 0.0).mutate(resetZeroRate, rng);
    CHECK(countDifferences(resetBefore, resetZeroRate) == 0);

    ga::IntegerChromosome resetFullRate = resetBefore;
    ga::RandomResetMutation<int>(0, 10, 1.0).mutate(resetFullRate, rng);
    CHECK(range.isValid(resetFullRate));
    CHECK(countDifferences(resetBefore, resetFullRate) > 0);

    ga::IntegerChromosome resetNarrow(std::vector<int>(20, 0));
    ga::RandomResetMutation<int>(7, 7, 1.0).mutate(resetNarrow, rng);
    bool narrowAllSeven = true;
    for (std::size_t i = 0; i < resetNarrow.size(); ++i)
        if (resetNarrow[i] != 7) narrowAllSeven = false;
    CHECK(narrowAllSeven);

    ga::BinaryChromosome resetBinary(std::vector<int>(50, 0));
    ga::RandomResetMutation<int>(0, 1, 1.0).mutate(resetBinary, rng);
    CHECK(bin.isValid(resetBinary));

    ga::FloatChromosomeSpec fspec(40, -2.0, 2.0);
    bool resetFloatInBounds = true;
    for (int t = 0; t < 100; ++t) {
        ga::FloatChromosome c = fspec.random(rng);
        ga::RandomResetMutation<double>(-2.0, 2.0, 0.5).mutate(c, rng);
        if (!fspec.isValid(c)) resetFloatInBounds = false;
    }
    CHECK(resetFloatInBounds);

    ga::FloatChromosomeSpec flt(40, -1.0, 1.0);
    bool gaussInBounds = true;
    for (int t = 0; t < 200; ++t) {
        ga::FloatChromosome c = flt.random(rng);
        ga::GaussianMutation<double>(0.8, -1.0, 1.0, 1.0).mutate(c, rng);
        if (!flt.isValid(c)) gaussInBounds = false;
    }
    CHECK(gaussInBounds);

    ga::FloatChromosome gaussBefore(std::vector<double>(40, 0.0));
    ga::FloatChromosome gaussZeroRate = gaussBefore;
    ga::GaussianMutation<double>(0.1, -1.0, 1.0, 0.0).mutate(gaussZeroRate, rng);
    CHECK(countDifferences(gaussBefore, gaussZeroRate) == 0);

    ga::FloatChromosome gaussFullRate = gaussBefore;
    ga::GaussianMutation<double>(0.1, -1.0, 1.0, 1.0).mutate(gaussFullRate, rng);
    CHECK(countDifferences(gaussBefore, gaussFullRate) == gaussBefore.size());

    ga::FloatChromosome gaussClamp(std::vector<double>(40, 1.0));
    ga::GaussianMutation<double>(5.0, -1.0, 1.0, 1.0).mutate(gaussClamp, rng);
    CHECK(flt.isValid(gaussClamp));

    CHECK_THROWS(ga::GaussianMutation<double>(0.0, -1.0, 1.0));
    CHECK_THROWS(ga::GaussianMutation<double>(0.1, 1.0, -1.0));
    CHECK_THROWS(ga::GaussianMutation<double>(0.1, -1.0, 1.0, 1.5));
    CHECK_THROWS(ga::RandomResetMutation<int>(0, 10, -0.1));

    return finishTests("mutation_tests");
}