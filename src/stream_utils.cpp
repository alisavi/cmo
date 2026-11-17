#include "stream_utils.hpp"
#include <iostream>
#include <iomanip>
#include <numeric>

namespace sim {
    std::vector<double> get_intervals(const std::vector<double>& arrivals) {
        std::vector<double> intervals(arrivals.size());
        double prev_time = 0.0;
        for (size_t i = 0; i < arrivals.size(); ++i) {
            intervals[i] = arrivals[i] - prev_time;
            prev_time = arrivals[i];
        }
        return intervals;
    }

    void print_arrivals(const std::vector<double>& arrivals, const std::vector<double>& intervals, double lambda) {
        std::cout << "\nArrival times for " << arrivals.size() << " events (lambda = " << lambda << "):\n";
        std::cout << "----------------------------------------\n";

        for (size_t i = 0; i < arrivals.size(); ++i) {
            std::cout << "Event " << std::setw(2) << (i + 1) 
                      << " | Interarrival: " << std::fixed << std::setprecision(4) << intervals[i] 
                      << " | Arrival Time: " << arrivals[i] << "\n";
        }
    }

    void print_statistics(const std::vector<double>& intervals, double lambda) {
        if (intervals.empty()) return;

        size_t n = intervals.size();
        double sum = std::accumulate(intervals.begin(), intervals.end(), 0.0);
        double mean = sum / n;
        
        double sq_sum = 0.0;
        for (double x : intervals) {
            sq_sum += (x - mean) * (x - mean);
        }
        double variance = (n > 1) ? (sq_sum / (n - 1)) : 0.0;
        
        double theory_mean = 1.0 / lambda;
        double theory_var = 1.0 / (lambda * lambda);
        double empirical_lambda = 1.0 / mean;

        std::cout << "\n--- Stream Statistics ---\n";
        std::cout << std::fixed << std::setprecision(6);
        
        std::cout << "Mean Interarrival Time:\n";
        std::cout << "  Empirical:   " << mean << "\n";
        std::cout << "  Theoretical: " << theory_mean << "\n\n";
        
        std::cout << "Interarrival Variance:\n";
        std::cout << "  Empirical:   " << variance << "\n";
        std::cout << "  Theoretical: " << theory_var << "\n\n";

        std::cout << "Estimated Lambda (intensity):\n";
        std::cout << "  Empirical:   " << empirical_lambda << "\n";
        std::cout << "  Theoretical: " << lambda << "\n";
    }
}