#include "mm1_core.hpp"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>

// --- per-run aggregator (Welford) ---
struct RunSummary {
    long long k = 0;
    double mean = 0.0;
    double M2   = 0.0;

    void add(double x) {
        ++k;
        double d = x - mean;
        mean += d / k;
        M2   += d * (x - mean);
    }
    double variance() const { return k > 1 ? M2 / (k - 1) : 0.0; }
    double stddev()   const { return std::sqrt(variance()); }
    double stderr_()  const { return k > 0 ? stddev() / std::sqrt((double)k) : 0.0; }

    // 95% Student-t critical value for df = 1..30, else ~1.96
    double tCrit95() const {
        static const double tbl[] = {
            12.706, 4.303, 3.182, 2.776, 2.571, 2.447, 2.365, 2.306, 2.262, 2.228,
             2.201, 2.179, 2.160, 2.145, 2.131, 2.120, 2.110, 2.101, 2.093, 2.086,
             2.080, 2.074, 2.069, 2.064, 2.060, 2.056, 2.052, 2.048, 2.045, 2.042
        };
        int df = (int)k - 1;
        if (df <= 0)  return 0.0;
        if (df <= 30) return tbl[df - 1];
        return 1.96;
    }
    double halfCI95() const { return tCrit95() * stderr_(); }
};

static void printSummary(const char* name, const RunSummary& s, double theory)
{
    std::printf("  %-4s mean = %10.6f  +- %.6f   (theory %10.6f,  err %+8.4f%%)\n",
                name, s.mean, s.halfCI95(), theory,
                theory != 0.0 ? 100.0 * (s.mean - theory) / theory : 0.0);
}

int main() {
    const double lambda = 0.8;
    const double mu     = 1.0;
    const double T      = 200'000.0;
    const int    K      = 30;

    const double rho_theory = lambda / mu;
    const double L_theory   = rho_theory / (1.0 - rho_theory);
    const double Lq_theory  = rho_theory * rho_theory / (1.0 - rho_theory);
    const double W_theory   = 1.0 / (mu - lambda);
    const double Wq_theory  = rho_theory / (mu - lambda);

    RunSummary sL, sLq, sRho, sW, sWq;

    for (int run = 0; run < K; ++run) {
        std::uint32_t seed = 1000u + 777u * (std::uint32_t)run;
        Stats st = simulateMM1(lambda, mu, T, seed, 500);

        sL.add(st.L());
        sLq.add(st.Lq());
        sRho.add(st.rho());
        sW.add(st.W());
        sWq.add(st.Wq());
    }

    std::cout << "lambda = " << lambda
              << ", mu = " << mu
              << ", rho = " << rho_theory
              << ", T = " << T
              << ", K = " << K << " runs\n\n";

    std::cout << "Aggregated over runs (95% CI):\n";
    printSummary("L",   sL,   L_theory);
    printSummary("Lq",  sLq,  Lq_theory);
    printSummary("rho", sRho, rho_theory);
    printSummary("W",   sW,   W_theory);
    printSummary("Wq",  sWq,  Wq_theory);

    std::cout << "\nLittle's law on the averages:\n";
    std::printf("  L  vs lambda*W  = %.6f vs %.6f\n", sL.mean,  lambda * sW.mean);
    std::printf("  Lq vs lambda*Wq = %.6f vs %.6f\n", sLq.mean, lambda * sWq.mean);

    std::cout << "\nCoefficient of variation across runs (std/mean):\n";
    std::printf("  L:  %.4f\n", sL.stddev()  / sL.mean);
    std::printf("  Lq: %.4f\n", sLq.stddev() / sLq.mean);
    std::printf("  W:  %.4f\n", sW.stddev()  / sW.mean);
    std::printf("  Wq: %.4f\n", sWq.stddev() / sWq.mean);

    return 0;
}