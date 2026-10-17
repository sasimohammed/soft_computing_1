#include "TestUtils.h"
#include "ga/replacement/GenerationalReplacement.h"
#include "ga/replacement/ElitistReplacement.h"

int main() {
    ga::GenerationalReplacement<int> generational;
    ga::ElitistReplacement<int> elitist;
    CHECK(generational.name() == "Generational");
    CHECK(elitist.name() == "Elitist");
    // TODO: result size == targetSize; elitist keeps the best eliteCount
    //       individuals; best fitness never decreases under elitism.
    return finishTests("replacement_tests");
}
