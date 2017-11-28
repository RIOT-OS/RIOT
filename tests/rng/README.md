# RNG
Test application for the RNG sourcs.

## Supported commands
* distributions [N] — Take N samples and print a bit distribution graph on the terminal.
* dump [N] — Take N samples and print them on the terminal.
* fips — Run the FIPS 140-2 random number tests.
* entropy [N] — Calculate Shannon's entropy from N samples.
* seed [N] — Set the random seed to use.
* source [N] — Select the RNG source, or list them all.
* speed [N] — Run a PRNG for N seconds and print the number of KiB/sec afterwards.

## Sources
The following sources are supported:

* PRNG
* HW RNG (if available)
* Constant number (uses the seed)

The PRNG is re-initialized before every test, using the given seed (default is 0).

A constant number source is useful to see if the test itself work, e.g. indicate failures.

## Warning
The tools available in this test do not garruantee that a given RNG source is secure. It should, however, rule out basic failures using statistical tests.
