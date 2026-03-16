# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`dlp_toolkit` is a C++20 toolkit for cryptographic number theory, implementing modular arithmetic, extended GCD, modular inverse, and Baby-step Giant-step (BSGS) discrete logarithm solving. It exposes a static library, a demo executable, and optional Python bindings via pybind11.

## Build Commands

```bash
make              # Debug build (C++ executable)
make run          # Build and run the demo executable
make bindings     # Build Python bindings (dlp_toolkit_py.so in project root)
make release      # Optimized release build
make tracy        # Build with Tracy profiler enabled
make full         # Build with Python bindings + Tracy
make clean        # Remove build/ and artifacts
```

CMake flags (passed via Makefile or directly):
- `BUILD_PYTHON_BINDINGS=ON` — enable pybind11 Python module
- `ENABLE_TRACY=ON` — enable Tracy profiler (`ZoneScoped` macros activate)
- `TYPE=Release` — release optimization (default: Debug)
- `NATIVE=ON` — adds `-O3 -march=native`

Dependencies are fetched automatically via CMake `FetchContent` (Tracy v0.13.1, pybind11 v2.11.1) — no manual setup needed.

## Architecture

### Library structure
- `src/dlp_toolkit.h` — all algorithms are implemented here as inline/header functions; `src/dlp_toolkit.cpp` is a stub
- `src/` compiles into the `myproject_core` static library
- `apps/` auto-discovers `.cpp` files and links each against `myproject_core`
- `python/bindings.cpp` wraps the library as a pybind11 module

### Running Python tests
```bash
make bindings
python3 python/tests.py
```

### Core API (`src/dlp_toolkit.h`)
Type aliases: `u64`, `i64`, `u128`, `i128` (backed by `__uint128_t` / `__int128_t`)

| Function | Description |
|---|---|
| `addmod(a, b, m)` | Modular addition |
| `mulmod(a, b, m)` | Modular multiplication (128-bit intermediate to prevent overflow) |
| `powmod(a, b, m)` | Binary exponentiation |
| `gcd(a, b) → gcd_ctx` | Extended Euclidean algorithm; returns `{g, u, v}` |
| `modinv(a, m)` | Modular multiplicative inverse via extended GCD |
| `bsgs(g, order, h, m)` | BSGS DLP solver using `unordered_map`; O(√n) time/space |
| `bsgs_naive(g, order, h, m)` | BSGS with linear vector search (for comparison) |

### Profiling
Tracy zones (`ZoneScoped`) are scattered through the hot paths. They compile away unless `TRACY_ENABLE` is defined. Use `make tracy` or `make full` to activate them.

### Python bindings
`python/bindings.cpp` exposes all core functions. `gcd` returns a `(g, u, v)` tuple. To add new functions, add `m.def(...)` entries and rebuild with `make bindings`.
