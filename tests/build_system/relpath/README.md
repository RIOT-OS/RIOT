# Relative path compile test

## Introduction

This test is supposed to check if `__FILE__` evaluates to the desired value,
which should resolve to the path of a filename relative to `$(RIOTBASE)`. The
goal of this test is to ensure that the build system does not use absolute
paths in the build process, which affects reproducibility and caching of
builds.

## Expected output

The test should print the name of the source file relative to `$(RIOTBASE)` and
then print `[SUCCESS]`.

### Ccache

The default compiler options will set `__FILE__` relative to `$(RIOTBASE)`, but
when Ccache is used, it will rewrite the path to be relative to the working
directory of the compiler, which is the application directory. That means that
the tests will only check if the `__FILE__` macro is relative.
