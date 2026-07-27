# UWHPC Onboarding — 2D Heat Diffusion

Implement a fast 2D heat-diffusion stencil. The full problem statement is in
[`docs/UWHPC_Onboarding.pdf`](docs/UWHPC_Onboarding.pdf).

## What you implement

Edit only these two files:

- [`src/grid.hpp`](src/grid.hpp) — the `Grid` class (you choose the memory layout)
- [`src/stencil.cpp`](src/stencil.cpp) — the `apply_stencil` function

Your `Grid` must keep this interface — the harness uses it to read and write cells:

```cpp
Grid(std::size_t nx, std::size_t ny);
std::size_t Nx() const;
std::size_t Ny() const;
double& operator()(std::size_t i, std::size_t j);       // read/write
double  operator()(std::size_t i, std::size_t j) const; // read
```

Everything else — `bench/`, `CMakeLists.txt`, `CMakePresets.json` — is the
provided evaluation harness. **Don't edit it.**

## Build & run locally

Requires CMake ≥ 3.21, Ninja, and a C++17 compiler.

```bash
cmake --preset benchmark                        # configure
cmake --build --preset benchmark                # build
ctest --preset benchmark --output-on-failure    # correctness test
./build/benchmark/uwhpc_benchmark               # run the benchmark
```

The benchmark prints one line of JSON:

```json
{ "runtime_ms": 97.882, "memory_mb": 16.777, "score": 0.758 }
```

`score` is the harness's reference time divided by your time — higher is faster.
The starter (`vector<vector<double>>`) scores below 1.0; improving the memory
layout and the kernel is the point.

## Submission

Push your changes. A GitHub Action runs your code through the UWHPC evaluator
and reports the result as a GitHub Action check. We'll then invite you to a short
virtual chat to discuss your design.
