#pragma once
#include <pcg_random.hpp>
#include <vector>
#include <cstdint>

namespace sim {
    struct SimulationParams {
        double lambda = 1.5;
        uint64_t seed = 42;
        int num_arrivals = 20;
    };

    // Prompts user for inputs
    SimulationParams get_simulation_params();

    // Generates Poisson arrival times
    std::vector<double> generate_poisson_arrivals(pcg64& rng, double lambda, int num_arrivals);
}