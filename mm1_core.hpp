#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <queue>
#include <random>
#include <vector>

struct Stats {
    double time      = 0.0;   // accumulated model time
    double areaN     = 0.0;   // ∫ N(t) dt
    double areaNq    = 0.0;   // ∫ N_q(t) dt
    double busyTime  = 0.0;   // ∫ 1{N>0} dt
    double sumW      = 0.0;   // Σ W_i  (completed requests)
    double sumWq     = 0.0;   // Σ Wq_i (completed requests)
    long long served = 0;     // number of completed requests
    std::vector<double> stateTime;  // time spent in state n

    void reset(double maxN) { *this = Stats{}; stateTime.assign((size_t)maxN, 0.0); }

    double L()   const { return time > 0 ? areaN  / time : 0.0; }
    double Lq()  const { return time > 0 ? areaNq / time : 0.0; }
    double rho() const { return time > 0 ? busyTime / time : 0.0; }
    double W()   const { return served > 0 ? sumW  / served : 0.0; }
    double Wq()  const { return served > 0 ? sumWq / served : 0.0; }
};

inline Stats simulateMM1(double lambda, double mu, double T,
                         std::uint32_t seed = 42, int MAXN = 500)
{
    std::mt19937 rng(seed);
    std::exponential_distribution<double> arrDist(lambda);
    std::exponential_distribution<double> srvDist(mu);

    // --- model state ---
    double t = 0.0;                 // time of last processed event
    double nextArrival   = arrDist(rng);
    double nextDeparture = std::numeric_limits<double>::infinity();

    int n = 0;                      // requests in system
    std::queue<double> q;           // arrival times of waiting requests
    double currentArrival      = 0.0;  // arrival time of request in service
    double currentServiceStart = 0.0;  // when it started service (for Wq)

    // --- accumulators ---
    Stats res;
    res.reset(MAXN);

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
        double tEvent = std::min(nextArrival, nextDeparture);
        if (tEvent > T) {
            addInterval(t, T);      // tail up to T with current n
            t = T;
            break;
        }
        addInterval(t, tEvent);     // interval [t, tEvent] with CURRENT n
        t = tEvent;

        if (nextArrival <= nextDeparture) {
            // ===== ARRIVAL =====
            n++;
            if (n == 1) {
                // server was idle -> start service immediately
                currentArrival      = t;
                currentServiceStart = t;
                nextDeparture       = t + srvDist(rng);
            } else {
                q.push(t);
            }
            nextArrival = t + arrDist(rng);
        } else {
            // ===== DEPARTURE =====
            n--;
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

    return res;
}