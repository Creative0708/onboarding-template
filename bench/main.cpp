// Evaluation harness (provided -- do not edit).
//
// Runs the PUBLIC correctness cases and the benchmark. The real evaluator runs
// these same public cases plus a set of private cases you cannot see here, so a
// submission that passes locally can still fail on hidden cases.
//
//   --check     run the public correctness cases; exit non-zero on any failure
//   (no args)   run the benchmark; print JSON to stdout
//
// Score is reference_ms / your_ms on the benchmark case -- higher is faster.

#include "grid.hpp"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>

// Your deliverable.
void apply_stencil(const Grid& old_grid, Grid& new_grid);

namespace {

using Clock = std::chrono::high_resolution_clock;

constexpr double kTolerance = 1e-6;

enum IC { IC_BLOCK, IC_GRADIENT, IC_CHECKER };

double initial_value(IC ic, std::size_t nx, std::size_t ny, std::size_t i, std::size_t j) {
    switch (ic) {
        case IC_BLOCK: {
            const std::size_t lox = nx / 2 - nx / 8, hix = nx / 2 + nx / 8;
            const std::size_t loy = ny / 2 - ny / 8, hiy = ny / 2 + ny / 8;
            return (i >= lox && i < hix && j >= loy && j < hiy) ? 100.0 : 0.0;
        }
        case IC_GRADIENT:
            return static_cast<double>(i + j);
        case IC_CHECKER:
            return ((i + j) % 2 == 0) ? 1.0 : 0.0;
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

struct Case {
    std::size_t nx, ny, steps;
    IC ic;
    const char* name;
};

// The public cases. The evaluator runs these plus additional private cases.
const Case kPublic[] = {
    {64, 64, 50, IC_BLOCK, "public/square-64"},
    {96, 128, 100, IC_GRADIENT, "public/nonsquare-96x128"},
    {200, 200, 150, IC_BLOCK, "public/square-200"},
};

double run_case_diff(std::size_t nx, std::size_t ny, std::size_t steps, IC ic) {
    Grid a0(nx, ny), a1(nx, ny);
    RefGrid r0(nx, ny), r1(nx, ny);
    for (std::size_t i = 0; i < nx; ++i) {
        for (std::size_t j = 0; j < ny; ++j) {
            const double v = initial_value(ic, nx, ny, i, j);
            a0(i, j) = v;
            r0.at(i, j) = v;
        }
    }
    Grid* ac = &a0;
    Grid* an = &a1;
    RefGrid* rc = &r0;
    RefGrid* rn = &r1;
    for (std::size_t t = 0; t < steps; ++t) {
        apply_stencil(*ac, *an);
        std::swap(ac, an);
        reference_stencil(*rc, *rn);
        std::swap(rc, rn);
    }
    double max_diff = 0.0;
    for (std::size_t i = 0; i < nx; ++i) {
        for (std::size_t j = 0; j < ny; ++j) {
            max_diff = std::max(max_diff, std::fabs((*ac)(i, j) - rc->at(i, j)));
        }
    }
    return max_diff;
}

int run_public() {
    int failures = 0;
    for (const Case& c : kPublic) {
        const double md = run_case_diff(c.nx, c.ny, c.steps, c.ic);
        const bool ok = (md <= kTolerance);
        std::fprintf(stderr, "[%s] %s  (max_diff=%.3e)\n", ok ? "PASS" : "FAIL", c.name, md);
        if (!ok) ++failures;
    }
    return failures;
}

int run_benchmark() {
    constexpr std::size_t kN = 1024;
    constexpr std::size_t kSteps = 200;

    Grid ag0(kN, kN), ag1(kN, kN);
    RefGrid rg0(kN, kN), rg1(kN, kN);
    for (std::size_t i = 0; i < kN; ++i) {
        for (std::size_t j = 0; j < kN; ++j) {
            const double v = initial_value(IC_BLOCK, kN, kN, i, j);
            ag0(i, j) = v;
            rg0.at(i, j) = v;
        }
    }

    Grid* ac = &ag0;
    Grid* an = &ag1;
    const auto a_start = Clock::now();
    for (std::size_t t = 0; t < kSteps; ++t) {
        apply_stencil(*ac, *an);
        std::swap(ac, an);
    }
    const double app_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - a_start).count();

    RefGrid* rc = &rg0;
    RefGrid* rn = &rg1;
    const auto r_start = Clock::now();
    for (std::size_t t = 0; t < kSteps; ++t) {
        reference_stencil(*rc, *rn);
        std::swap(rc, rn);
    }
    const double ref_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - r_start).count();

    const double mem_mb = 2.0 * static_cast<double>(kN) * kN * sizeof(double) / 1e6;
    const double score = (app_ms > 0.0) ? (ref_ms / app_ms) : 0.0;
    std::printf("{\"runtime_ms\": %.3f, \"memory_mb\": %.3f, \"score\": %.3f}\n",
                app_ms, mem_mb, score);
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--check") == 0) {
        return run_public() == 0 ? 0 : 1;
    }
    return run_benchmark();
}
