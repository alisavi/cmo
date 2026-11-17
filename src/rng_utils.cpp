#include "rng_utils.hpp"
#include <cmath>
#include <limits>

namespace sim {
    double uniform01(pcg64& g) {
        return static_cast<double>(g() >> 11) * (1.0 / 9007199254740992.0);
    }

    double exponential(pcg64& g, double lambda) {
        double u = uniform01(g);
        if (u == 0.0) u = std::numeric_limits<double>::min();
        return -std::log(u) / lambda;
    }
}