#pragma once
#include <iostream>

// Minimal test helper (no external framework).
inline int& testFailures() { static int failures = 0; return failures; }

#define CHECK(cond)                                                          \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::cerr << "FAILED: " #cond " (" << __FILE__ << ":"            \
                      << __LINE__ << ")\n";                                  \
            ++testFailures();                                                \
        }                                                                    \
    } while (0)

#define CHECK_THROWS(expr)                                                   \
    do {                                                                     \
        bool threw_ = false;                                                 \
        try { expr; } catch (...) { threw_ = true; }                         \
        if (!threw_) {                                                       \
            std::cerr << "FAILED (no throw): " #expr " (" << __FILE__ << ":" \
                      << __LINE__ << ")\n";                                  \
            ++testFailures();                                                \
        }                                                                    \
    } while (0)

inline int finishTests(const char* suite) {
    if (testFailures() == 0) std::cout << suite << ": all checks passed\n";
    return testFailures() == 0 ? 0 : 1;
}