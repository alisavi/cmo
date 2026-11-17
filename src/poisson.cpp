#include "poisson.hpp"
#include "rng_utils.hpp"
#include <iostream>
#include <string>

namespace sim {
    SimulationParams get_simulation_params() {
        SimulationParams params;
        std::string input;

        std::cout << "Enter lambda (default " << params.lambda << "): ";
        std::getline(std::cin, input);
        if (!input.empty()) params.lambda = std::stod(input);

        std::cout << "Enter seed (default " << params.seed << "): ";
        std::getline(std::cin, input);
        if (!input.empty()) params.seed = std::stoull(input);

        std::cout << "Enter number of arrivals (default " << params.num_arrivals << "): ";
        std::getline(std::cin, input);
        if (!input.empty()) params.num_arrivals = std::stoi(input);

        return params;
    }

    std::vector<double> generate_poisson_arrivals(pcg64& rng, double lambda, int num_arrivals) {
        std::vector<double> arrivals(num_arrivals);
        double current_time = 0.0;
        
        for (int i = 0; i < num_arrivals; ++i) {
            current_time += exponential(rng, lambda);
            arrivals[i] = current_time;
        }
        
        return arrivals;
    }
}