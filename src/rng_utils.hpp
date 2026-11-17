#pragma once
#include <pcg_random.hpp>

namespace sim {
    // Generates U in [0, 1) using pcg64
    double uniform01(pcg64& g);

    // Generates exponential distribution
    double exponential(pcg64& g, double lambda);
}