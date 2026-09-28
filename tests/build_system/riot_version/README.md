# RIOT version code test

## Introduction

This test checks that the build system parses `RIOT_VERSION` and
`RIOT_EXTRAVERSION` correctly and exposes them to the code as the
`RIOT_VERSION_CODE` macro, which can be compared against `RIOT_VERSION_NUM()`
from `riot_version.h`.

## Usage

The test uses the version of the current checkout by default:

```shell
make flash test
```

Other versions can be tested by overriding the variables:

```shell
RIOT_VERSION=2026.10-devel-123-gabcd make flash test
RIOT_VERSION=2026.10 make flash test
RIOT_VERSION=2026.04.1 RIOT_EXTRAVERSION=3 make flash test
```

`Makefile.include` will define `RIOT_VERSION` as `buildtest` and define
`RIOT_VERSION_CODE` as `RIOT_VERSION_DUMMY_CODE` when running in CI:

```shell
RIOT_CI_BUILD=1 make flash test
```

## Expected result

The application prints the version string and the four parts of the version
code. The automated test calculates the expected parts from the same variables
and compares them with the values printed by the application.

It is important that the compilation of the application is performed with the
same variables as running the test, otherwise the test will fail.

This output of the application without any overrides:

```
This is RIOT! (Version: 2026.10-devel-715-g475ae3)
RIOT_VERSION: 2026.10-devel-715-g475ae3
RIOT_VERSION_CODE: 2026.10.0.0
```

This output of the application when overriding the version variables:

```
main(): This is RIOT! (Version: 2026.04.1)
RIOT_VERSION: 2026.04.1
RIOT_VERSION_CODE: 2026.4.1.3
```

When running in CI, the output will be:

```
main(): This is RIOT! (Version: buildtest)
RIOT_VERSION: buildtest
RIOT_VERSION_CODE: 2042.5.23.0
```
