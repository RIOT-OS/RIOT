---
title: Static Analysis
description: Using Static Analysis via RIOT's Build System
---

## Motivation

Static Analysis is a process of analyzing software without executing it. This
is particularly useful for embedded software that requires special hardware to
run, as it allows its use in the CI even without integrating the target
hardware into the CI infrastructure.

While there are plenty of static analysis tools, the following will focus only
on the built-in static analysis capabilities of GCC and LLVM.

## Using Static Analysis with GCC

With GCC, static analysis is part of the regular build process by setting the
flag `STATIC_ANALYSIS=1` (either as an argument to `make` or as an environment
variable).

Since new warnings pop up during compilation with static analysis that --
unless compiling with `WERROR=0` -- are propagated to errors, modules need to
explicitly declare compatibility with static analysis. They do so by adding the
following snippet to the module's `Makefile`:

```makefile
# this module is expected to pass static analysis
MODULE_SUPPORTS_STATIC_ANALYSIS := 1
```

:::note
`MODULE_SUPPORTS_STATIC_ANALYSIS` can also be passed in as a parameter to
`make` or via an environment variable to force the use of static analysis, even
for modules that do not declare support. You might want to also pass `WERROR=0`
to complete the build despite the new issues found by static analysis.
:::

:::caution
Since static analysis is part of the regular build process, incremental builds
will provide only partial coverage. Use `make clean` to do a fresh build.
:::

### Use SARIF Output

By default, GCC will output the static analysis to `stdout` just like any other
compilation error. It is possible to create output using the
[Static Analysis Results Interchange Format (SARIF)][SARIF] format instead by
adding the `WRITE_SARIF=1` flag either as a parameter to `make` or via an
environment variable. SARIF is a cross-vendor standard format based on JSON
(see the [SARIF specification][SARIF spec] for more details). Even though the
format is also human readable, using a dedicated SARIF viewer (either as local
program or as web app) is likely providing a better user experience.

:::caution
All required SARIF output features are
[supported since release 15][SARIF GCC support]. Compiling with `WRITE_SARIF=1`
using older releases of GCC will fail.
:::

[SARIF]: https://sarifweb.azurewebsites.net/
[SARIF spec]: https://docs.oasis-open.org/sarif/sarif/v2.1.0/sarif-v2.1.0.html
[SARIF GCC support]: https://gcc.gnu.org/wiki/SARIF

### tl;dr: Use Static Analysis for Supported Modules

```
make STATIC_ANALYSIS=1 BOARD=native64 -C examples/basic/default clean all
```

### tl;dr: Use Static Analysis for All Modules

```
make STATIC_ANALYSIS=1 MODULE_SUPPORTS_STATIC_ANALYSIS=1 WERROR=0 BOARD=native64 -C examples/basic/default clean all
```

### tl;dr: Full SARIF Report

```
make STATIC_ANALYSIS=1 MODULE_SUPPORTS_STATIC_ANALYSIS=1 WRITE_SARIF=1 WERROR=0 BOARD=native64 -C examples/basic/default clean all
```

## Using Static Analysis with LLVM

With LLVM, static analysis is a distinct make goal: `make scan-build`. It behaves
similarly to a full build, but creates an HTML summary containing all the
findings and opens that in a browser once it completes.

If no issues are found, no HTML will be created and the browser will not be
opened. In that case the console output will look like this:

```
scan-build: Removing directory '<DIRNAME>' because it contains no reports.
scan-build: No bugs found.
Showing most recent report in your web browser...
No report found
```

:::note
Unlike GCC's static analysis, `make scan-build` will perform a full rebuild
automatically. So there is no incremental build footgun to avoid.
:::

The LLVM static analysis does not use the variable
`MODULE_SUPPORTS_STATIC_ANALYSIS`, so it will enable static analysis for all
modules. However, it will treat the additional findings as warnings rather than
as errors, so that builds won't fail.

### tl;dr: Use Static Analysis for All Modules

```
make BOARD=native64 -C examples/basic/default scan-build
```
