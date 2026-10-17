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
    // TODO: random() produces valid chromosomes; isValid rejects bad genes,
    //       wrong length, duplicated permutation values, out-of-range floats.
    return finishTests("chromosome_tests");
}
