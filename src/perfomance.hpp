#pragma once
#include <chrono>
#include <cstddef>

namespace sim {

    // Helper class to measure time using RAII or simple start/stop
    class Timer {
    private:
        std::chrono::high_resolution_clock::time_point start_time;
    public:
        void start();
        double elapsed_ms() const;
    };

    // Struct to hold all performance metrics
    struct PerformanceReport {
        double sim_time_ms = 0.0;
        double total_time_ms = 0.0;
        size_t num_arrivals = 0;
    };

    // Function to print the report and calculate derived metrics
    void print_performance_report(const PerformanceReport& report);

}