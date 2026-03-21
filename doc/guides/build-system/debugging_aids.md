---
title: Debugging Tools
description: Overview of debugging tools supported in RIOT-OS
---

## Undefined Behavior Sanitizer (ubsan)

RIOT contains Makefile convenience support for gcc/clang's undefined
behaviour sanitizer.

### Overview

Both gcc and clang allow generation of code that does runtime checks for
undefined behavior (UB).

E.g., the following code might trigger UB for some parameters:

```c
void test(int foo)
{
    return (foo << 24);
}
```

In this case, the signed shift would be alright unless:

- it would "push out" all bits to the left, with undefined runtime result. Here,
  that happens on architectures with 16-bit integers.
- `foo` is negative, with implementation-specific runtime results.

Using ubsan, these can be caught at runtime.

There are three modes for ubsan that define what happens when the sanitizer
observed undefined behaviour:

1. `trap` -> cause a trap
2. `msg_exit` -> print a message and exit
3. `msg_recover` -> print a message and continue

`trap` is available on all RIOT platforms, whereas `msg_exit` and `msg_recover`
are currently only available on `native` when building with gcc, as they require runtime support in
the form of `libubsan`.

The default is `trap`, or `msg_exit` if available (currently, on native:gnu only).


### How to Use

1. build with `make all-ubsan`.

2. build with `UBSAN_MODE=[trap|msg_exit|msg_recover] make all-ubsan` to
   override the ubsan mode.

## Code Coverage (gcov)

RIOT contains Makefile convenience support for gcov-based code coverage
reporting on the `native` board.

### Overview

When built with `-fprofile-arcs -ftest-coverage`, GCC injects execution
counters into every branch of the compiled code.  Running the binary causes
the counters to be flushed to `.gcda` files.  `lcov` and `genhtml` then
combine those files with the static `.gcno` graphs to produce an HTML report
showing which lines and branches were executed.

Coverage measurement is dynamic — it only counts code that actually runs
during the test execution, making it a complement to unit tests rather than
a replacement for static analysis.

### Prerequisites

- **Without Docker**: `lcov` must be installed on the host
  (`apt install lcov` / `brew install lcov`).  On macOS the system `gcov`
  is LLVM-based and incompatible with GCC `.gcda` files; install GCC via
  Homebrew (`brew install gcc`) so that a compatible `gcov` is available.
- **With `BUILD_IN_DOCKER=1`**: only Docker is required.  The build and run
  steps execute inside the `riotbuild` container; a separate `ubuntu:22.04`
  container (which provides GCC 11, matching `riotbuild`) then runs
  `lcov`/`genhtml`, avoiding `.gcda` format mismatches entirely.

### How to Use

1. From any native application directory (e.g. `tests/unittests`):

   ```sh
   # Without Docker (host lcov required)
   make coverage BOARD=native

   # With Docker (recommended — no host toolchain needed)
   BUILD_IN_DOCKER=1 make coverage BOARD=native

   # Run coverage for a specific unit-test suite
   BUILD_IN_DOCKER=1 make coverage BOARD=native UNIT_TESTS=tests-fmt
   ```

2. Open `coverage/index.html` in a browser to view the report.

### Sub-targets

| Target | Description |
|---|---|
| `coverage/run` | Run the binary to produce `.gcda` files |
| `coverage/collect` | Run `lcov` to collect coverage data |
| `coverage/html` | Run `genhtml` to produce the HTML report |
| `coverage/report-docker` | Run collect + html inside a Docker container |
| `coverage/clean` | Remove `.gcda`/`.gcno` files and the report directory |
