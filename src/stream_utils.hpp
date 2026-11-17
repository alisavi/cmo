#pragma once
#include <vector>

namespace sim {
    // Calculates intervals between arrival times
    std::vector<double> get_intervals(const std::vector<double>& arrivals);

    // Prints the arrival times and intervals
    void print_arrivals(const std::vector<double>& arrivals, const std::vector<double>& intervals, double lambda);

    // Prints empirical vs theoretical statistics
    void print_statistics(const std::vector<double>& intervals, double lambda);
}