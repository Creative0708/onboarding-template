// Evaluation harness (provided -- do not edit).
//
// Runs the applicant's Grid + apply_stencil on a fixed problem, checks the
// result against an independent reference implementation, and reports timing.
//
//   (no args)  run benchmark, verify correctness, print JSON to stdout
//   --check    verify correctness only, exit non-zero on mismatch (used by ctest)
//
// Score is reference_ms / applicant_ms: both run on the same machine in the same
// invocation, so it is a hardware-independent speed ratio relative to a simple
// flat-array reference.

#include "grid.hpp"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>

// Applicant's deliverable.
void apply_stencil(const Grid& old_grid, Grid& new_grid);

namespace {

using Clock = std::chrono::high_resolution_clock;

constexpr std::size_t kN = 1024;     // grid is kN x kN
constexpr std::size_t kSteps = 200;  // number of time steps
constexpr double kTolerance = 1e-6;  // max allowed abs diff per grid point

// Deterministic initial condition: a hot block in the centre, zero elsewhere.
double initial_value(std::size_t i, std::size_t j) {
    const std::size_t lo = kN / 2 - kN / 8;
    const std::size_t hi = kN / 2 + kN / 8;
    if (i >= lo && i < hi && j >= lo && j < hi) {
        return 100.0;
    }
    return 0.0;
}

// Independent reference: flat row-major array, straightforward implementation.
struct RefGrid {
    std::size_t nx, ny;
    std::vector<double> a;
    RefGrid(std::size_t nx_, std::size_t ny_) : nx(nx_), ny(ny_), a(nx_ * ny_, 0.0) {}
    double& at(std::size_t i, std::size_t j) { return a[i * ny + j]; }
    double at(std::size_t i, std::size_t j) const { return a[i * ny + j]; }
};

void reference_stencil(const RefGrid& o, RefGrid& n) {
    for (std::size_t i = 0; i < o.nx; ++i) {
        n.at(i, 0) = o.at(i, 0);
        n.at(i, o.ny - 1) = o.at(i, o.ny - 1);
    }
    for (std::size_t j = 0; j < o.ny; ++j) {
        n.at(0, j) = o.at(0, j);
        n.at(o.nx - 1, j) = o.at(o.nx - 1, j);
    }
    for (std::size_t i = 1; i < o.nx - 1; ++i) {
        for (std::size_t j = 1; j < o.ny - 1; ++j) {
            n.at(i, j) = 0.5 * o.at(i, j) +
                         0.125 * (o.at(i - 1, j) + o.at(i + 1, j) +
                                  o.at(i, j - 1) + o.at(i, j + 1));
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
    const bool check_only = (argc > 1 && std::strcmp(argv[1], "--check") == 0);

    // Applicant run.
    Grid ag0(kN, kN), ag1(kN, kN);
    for (std::size_t i = 0; i < kN; ++i) {
        for (std::size_t j = 0; j < kN; ++j) {
            ag0(i, j) = initial_value(i, j);
        }
    }
    Grid* acur = &ag0;
    Grid* anxt = &ag1;
    const auto a_start = Clock::now();
    for (std::size_t t = 0; t < kSteps; ++t) {
        apply_stencil(*acur, *anxt);
        std::swap(acur, anxt);
    }
    const double app_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - a_start).count();

    // Reference run.
    RefGrid rg0(kN, kN), rg1(kN, kN);
    for (std::size_t i = 0; i < kN; ++i) {
        for (std::size_t j = 0; j < kN; ++j) {
            rg0.at(i, j) = initial_value(i, j);
        }
    }
    RefGrid* rcur = &rg0;
    RefGrid* rnxt = &rg1;
    const auto r_start = Clock::now();
    for (std::size_t t = 0; t < kSteps; ++t) {
        reference_stencil(*rcur, *rnxt);
        std::swap(rcur, rnxt);
    }
    const double ref_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - r_start).count();

    // Correctness.
    double max_diff = 0.0;
    for (std::size_t i = 0; i < kN; ++i) {
        for (std::size_t j = 0; j < kN; ++j) {
            max_diff = std::max(max_diff, std::fabs((*acur)(i, j) - rcur->at(i, j)));
        }
    }
    if (max_diff > kTolerance) {
        std::fprintf(stderr, "correctness FAILED: max_diff=%.3e (tolerance=%.1e)\n",
                     max_diff, kTolerance);
        return 1;
    }
    if (check_only) {
        std::fprintf(stderr, "correctness OK: max_diff=%.3e\n", max_diff);
        return 0;
    }

    const double mem_mb = 2.0 * static_cast<double>(kN) * kN * sizeof(double) / 1e6;
    const double score = (app_ms > 0.0) ? (ref_ms / app_ms) : 0.0;
    std::printf("{\"runtime_ms\": %.3f, \"memory_mb\": %.3f, \"score\": %.3f}\n",
                app_ms, mem_mb, score);
    return 0;
}
