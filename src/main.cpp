#include <iostream>
#include "poisson.hpp"
#include "stream_utils.hpp"

int main() {
    // 1. Get parameters from user
    sim::SimulationParams params = sim::get_simulation_params();
    
    // 2. Initialize RNG with the chosen seed
    pcg64 rng(params.seed);
    
    // 3. Generate the Poisson arrivals
    std::vector<double> arrivals = sim::generate_poisson_arrivals(rng, params.lambda, params.num_arrivals);
    std::vector<double> intervals = sim::get_intervals(arrivals);
    
    // 4. Output and analyze
    sim::print_arrivals(arrivals, intervals, params.lambda);
    sim::print_statistics(intervals, params.lambda);
    
    return 0;
}