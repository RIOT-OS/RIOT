randsort.py
-----------

Reads newline separated data from standard input and outputs a random
permutation of the received data on standard output. This script is
used by the build system to provide link time reordering.

By default a secure random number generator is used. A PRNG is used
instead if a value (not equal to `-1`) for seeding the random number
generator has been specified using the `--seed` command line flag. The
same permutation will be output for a given seed.
