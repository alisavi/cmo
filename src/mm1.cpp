#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <vector>

struct Stats {
    double time      = 0.0;   // накопленное модельное время
    double areaN     = 0.0;   // ∫ N(t) dt
    double areaNq    = 0.0;   // ∫ N_q(t) dt
    double busyTime  = 0.0;   // ∫ 1{N>0} dt
    double sumW      = 0.0;   // Σ W_i  (для завершённых)
    double sumWq     = 0.0;   // Σ Wq_i (для завершённых)
    long long served = 0;     // число завершённых запросов
    std::vector<double> stateTime;  // время в состоянии n

    void reset(double maxN) { *this = Stats{}; stateTime.assign((size_t)maxN, 0.0); }

    double L()   const { return time > 0 ? areaN  / time : 0.0; }
    double Lq()  const { return time > 0 ? areaNq / time : 0.0; }
    double rho() const { return time > 0 ? busyTime / time : 0.0; }
    double W()   const { return served > 0 ? sumW  / served : 0.0; }
    double Wq()  const { return served > 0 ? sumWq / served : 0.0; }
};

Stats simulateMM1(double lambda, double mu, double T,
                  std::uint32_t seed = 42, int MAXN = 500)
{
    std::mt19937 rng(seed);
    std::exponential_distribution<double> arrDist(lambda);
    std::exponential_distribution<double> srvDist(mu);

    // --- состояние модели ---
    double t = 0.0;
    double nextArrival   = arrDist(rng);
    double nextDeparture = std::numeric_limits<double>::infinity();

    int n = 0;                      // запросов в системе
    std::queue<double> q;           // времена прихода ждущих
    double currentArrival = 0.0;    // время прихода обслуживаемого
    double currentServiceStart = 0.0; // когда начал обслуживаться (для Wq)

    // --- накопители ---
    Stats res;
    res.reset(MAXN);

    double tPrev = 0.0;
    auto addInterval = [&](double t0, double t1) {
        if (t1 <= t0) return;
        double dt = t1 - t0;
        res.time      += dt;
        res.areaN     += n * dt;
        res.areaNq    += (double)q.size() * dt;
        if (n > 0) res.busyTime += dt;
        res.stateTime[std::min<size_t>(n, MAXN-1)] += dt;
    };

    auto recordService = [&](double startT, double endT, double arrivalT) {
        res.sumW  += endT - arrivalT;
        res.sumWq += startT - arrivalT;
        res.served++;
    };

    while (t < T) {
        // накопить интервал [tPrev, t] (площади — точно, т.к. N и N_q постоянны)
        addInterval(tPrev, t);
        tPrev = t;

        if (nextArrival < nextDeparture) {
            // ===== ПРИХОД =====
            t = nextArrival;
            n++;
            if (n == 1) {
                // прибор был свободен — сразу на обслуживание
                currentArrival       = t;
                currentServiceStart  = t;
                nextDeparture        = t + srvDist(rng);
            } else {
                q.push(t);
            }
            nextArrival = t + arrDist(rng);
        } else {
            // ===== УХОД =====
            t = nextDeparture;
            n--;
            // запрос завершил обслуживание — фиксируем W, Wq
            recordService(currentServiceStart, t, currentArrival);

            if (!q.empty()) {
                double arrT = q.front(); q.pop();
                currentArrival      = arrT;
                currentServiceStart = t;
                nextDeparture       = t + srvDist(rng);
            } else {
                nextDeparture = std::numeric_limits<double>::infinity();
            }
        }
    }

    // хвост: досчитать площади до T
    addInterval(tPrev, T);

    return res;
}

void printStats(const Stats& s, const char* title,
                double lambda, double mu)
{
    const double rho_theory = lambda / mu;
    const double L_theory   = rho_theory / (1.0 - rho_theory);
    const double Lq_theory  = rho_theory * rho_theory / (1.0 - rho_theory);
    const double W_theory   = 1.0 / (mu - lambda);
    const double Wq_theory  = rho_theory / (mu - lambda);

    std::cout << "=== " << title << " ===\n";
    std::cout << "  time: " << s.time << "\n";
    std::cout << "  served:   " << s.served << "\n";
    std::printf("  L   = %.6f   (theory %.6f)\n", s.L(),  L_theory);
    std::printf("  Lq  = %.6f   (theory %.6f)\n", s.Lq(), Lq_theory);
    std::printf("  rho = %.6f   (theory %.6f)\n", s.rho(), rho_theory);
    std::printf("  W   = %.6f   (theory %.6f)\n", s.W(),  W_theory);
    std::printf("  Wq  = %.6f   (theory %.6f)\n", s.Wq(), Wq_theory);
    // проверка Литтла
    std::printf("  Little's law: L  vs lambda*W  = %.6f vs %.6f\n",
                s.L(),  lambda * s.W());
    std::printf("  Little's law: Lq vs lambda*Wq = %.6f vs %.6f\n",
                s.Lq(), lambda * s.Wq());
    std::cout << "\n";
}

int main() {
    const double lambda = 0.8;   // интенсивность прихода
    const double mu     = 1.0;   // интенсивность обслуживания
    const double T      = 2'000'000.0;

    auto res = simulateMM1(lambda, mu, T, /*seed=*/12345, /*MAXN=*/500);

    std::cout << "lambda = " << lambda << ", mu = " << mu
              << ", rho = " << lambda/mu << "\n\n";

    printStats(res, "M/M/1", lambda, mu);

    // Сравнение эмпирического p_n с теорией
    std::cout << "n\tp_n(exp)\tp_n(th)\n";
    const double rho = lambda / mu;
    double ptheory = 1.0 - rho;
    for (int i = 0; i < 10; ++i) {
        double pemp = res.stateTime[i] / res.time;
        std::printf("%d\t%.6f\t%.6f\n", i, pemp, ptheory);
        ptheory *= rho;
    }

    return 0;
}