# UWHPC Onboarding — 2D Heat Diffusion

Starter repository for the UWHPC onboarding problem.

**The problem statement lives here: [UWHPC Onboarding](https://docs.uwhpc.com/onboarding/problem-statement/).** Read it first.
This file only covers building and submitting.

## What you edit

One file: [`src/submission.hpp`](src/submission.hpp). Both the `Grid` class and
the `apply_stencil` function go there, and the interface your `Grid` has to
provide is documented at the top of that file.

Everything else (`bench/`, `CMakeLists.txt`, `CMakePresets.json`, etc) is the
evaluation harness. **Don't edit it.**

## Build & run

Requires CMake ≥ 3.21, Ninja, and a C++17 compiler.

```bash
cmake --preset benchmark                        # configure
cmake --build --preset benchmark                # build
ctest --preset benchmark --output-on-failure    # correctness test
./build/benchmark/uwhpc_benchmark               # run the benchmark
```

`--check` runs three public correctness cases. The evaluator runs those plus
private cases you can't see here, so passing locally is necessary but not
sufficient.

The benchmark prints one line of JSON, where `score` is the harness's reference
time divided by yours:

```json
{ "runtime_ms": 148.404, "memory_mb": 16.777, "score": 1.293 }
```

Use the commands above rather than an IDE's own CMake integration. Those default
to a Debug build, which will cost you most of your score. See the
[problem statement](https://docs.uwhpc.com/onboarding/problem-statement/) for what the number means.

## Submitting

Push your changes. A GitHub Action runs your code through the UWHPC evaluator
and reports the result as a commit check. We'll then invite you to a short
virtual chat to discuss your design.
