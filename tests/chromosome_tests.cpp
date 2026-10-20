#include <cmath>
#include <random>
#include <vector>
#include "TestUtils.h"
#include "ga/chromosomes/BinaryChromosome.h"
#include "ga/chromosomes/IntegerChromosome.h"
#include "ga/chromosomes/FloatChromosome.h"

int main() {
    ga::BinaryChromosomeSpec bin(8);
    ga::IntegerChromosomeSpec perm(5, 1, 5, true);
    ga::FloatChromosomeSpec flt(3, -1.0, 1.0);
    CHECK(bin.length() == 8);
    CHECK(perm.isPermutation());
    CHECK(flt.upper() == 1.0);
    CHECK_THROWS(ga::BinaryChromosomeSpec(0));
    CHECK_THROWS(ga::IntegerChromosomeSpec(3, 5, 1));
    CHECK_THROWS(ga::FloatChromosomeSpec(3, 2.0, 1.0));

    std::mt19937 rng(42);

    bool binRandomValid = true;
    bool binSizeOk = true;
    for (int t = 0; t < 200; ++t) {
        ga::BinaryChromosome c = bin.random(rng);
        if (!bin.isValid(c)) binRandomValid = false;
        if (c.size() != 8) binSizeOk = false;
    }
    CHECK(binRandomValid);
    CHECK(binSizeOk);

    std::vector<int> binBadGenes(8, 0);
    binBadGenes[3] = 2;
    ga::BinaryChromosome binBad(binBadGenes);
    CHECK(!bin.isValid(binBad));

    std::vector<int> binNegGenes(8, 0);
    binNegGenes[0] = -1;
    ga::BinaryChromosome binNeg(binNegGenes);
    CHECK(!bin.isValid(binNeg));

    ga::BinaryChromosome binShort(std::vector<int>(5, 0));
    ga::BinaryChromosome binLong(std::vector<int>(9, 1));
    CHECK(!bin.isValid(binShort));
    CHECK(!bin.isValid(binLong));

    ga::BinaryChromosome binAllZero(std::vector<int>(8, 0));
    ga::BinaryChromosome binAllOne(std::vector<int>(8, 1));
    CHECK(bin.isValid(binAllZero));
    CHECK(bin.isValid(binAllOne));

    bool permRandomValid = true;
    for (int t = 0; t < 200; ++t) {
        ga::IntegerChromosome c = perm.random(rng);
        if (!perm.isValid(c)) permRandomValid = false;
    }
    CHECK(permRandomValid);

    std::vector<int> permDupGenes = {1, 1, 3, 4, 5};
    ga::IntegerChromosome permDup(permDupGenes);
    CHECK(!perm.isValid(permDup));

    std::vector<int> permOutGenes = {1, 2, 3, 4, 6};
    ga::IntegerChromosome permOut(permOutGenes);
    CHECK(!perm.isValid(permOut));

    std::vector<int> permZeroGenes = {0, 1, 2, 3, 4};
    ga::IntegerChromosome permZero(permZeroGenes);
    CHECK(!perm.isValid(permZero));

    std::vector<int> permShortGenes = {1, 2, 3, 4};
    ga::IntegerChromosome permShort(permShortGenes);
    CHECK(!perm.isValid(permShort));

    std::vector<int> permGoodGenes = {5, 3, 1, 4, 2};
    ga::IntegerChromosome permGood(permGoodGenes);
    CHECK(perm.isValid(permGood));

    ga::IntegerChromosomeSpec range(20, 3, 9);
    CHECK(!range.isPermutation());
    bool rangeRandomValid = true;
    for (int t = 0; t < 200; ++t) {
        ga::IntegerChromosome c = range.random(rng);
        if (!range.isValid(c)) rangeRandomValid = false;
    }
    CHECK(rangeRandomValid);

    std::vector<int> rangeAbove(20, 5);
    rangeAbove[10] = 10;
    ga::IntegerChromosome rangeAboveChrom(rangeAbove);
    CHECK(!range.isValid(rangeAboveChrom));

    std::vector<int> rangeBelow(20, 5);
    rangeBelow[0] = 2;
    ga::IntegerChromosome rangeBelowChrom(rangeBelow);
    CHECK(!range.isValid(rangeBelowChrom));

    std::vector<int> rangeRepeat(20, 5);
    ga::IntegerChromosome rangeRepeatChrom(rangeRepeat);
    CHECK(range.isValid(rangeRepeatChrom));

    CHECK_THROWS(ga::IntegerChromosomeSpec(4, 1, 5, true));
    CHECK_THROWS(ga::IntegerChromosomeSpec(0, 1, 5));

    bool fltRandomValid = true;
    for (int t = 0; t < 200; ++t) {
        ga::FloatChromosome c = flt.random(rng);
        if (!flt.isValid(c)) fltRandomValid = false;
    }
    CHECK(fltRandomValid);

    std::vector<double> fltHighGenes = {0.0, 0.5, 1.5};
    ga::FloatChromosome fltHigh(fltHighGenes);
    CHECK(!flt.isValid(fltHigh));

    std::vector<double> fltLowGenes = {-1.5, 0.0, 0.5};
    ga::FloatChromosome fltLow(fltLowGenes);
    CHECK(!flt.isValid(fltLow));

    std::vector<double> fltNanGenes = {0.0, std::nan(""), 0.5};
    ga::FloatChromosome fltNan(fltNanGenes);
    CHECK(!flt.isValid(fltNan));

    std::vector<double> fltShortGenes = {0.0, 0.5};
    ga::FloatChromosome fltShort(fltShortGenes);
    CHECK(!flt.isValid(fltShort));

    std::vector<double> fltEdgeGenes = {-1.0, 0.0, 1.0};
    ga::FloatChromosome fltEdge(fltEdgeGenes);
    CHECK(flt.isValid(fltEdge));

    return finishTests("chromosome_tests");
}