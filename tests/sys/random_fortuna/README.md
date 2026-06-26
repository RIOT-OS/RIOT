# Fortuna PRNG

Tests for the Fortuna PRNG.

## Expected results

All tests should pass.

This test runs the full suite on a native board, because it has two unit tests
that need to write 1 MiB of random data to memory. Other platforms have these
tests disabled.
