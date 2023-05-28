# wolfssl tests and benchmarks

This test can be used to validate the usability and performance of a wolfcrypt/wolfssl
port to a specific target.

## Compile options

Specific modules can be enabled/disabled using the `USEMODULE` variable in `Makefile`.

To disable full-benchmark test, comment out the line `USEMODULE += wolfssl-benchmarks`.

## Modules vs. Resources

This test tool can be used to benchmark single pseudo-modules in the wolfSSL package,
on the selected target.

The default `BOARD_INSUFFICIENT_MEMORY` list provided in [Makefile](Makefile) excludes
all the targets that are unable to run the full test. Targets included in this list
may still be able to run a build with a reduced set of algorithms compiled in.

To enable/disable single modules to include in the build, see the `USEMODULE` list.
