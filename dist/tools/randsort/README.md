randsort.py
-----------

Reads newline separated data from standard input and outputs a random
permutation of the received data on standard output. This script is
used by the build system to provide link time reordering.

If the environment variable `RIOTINSECURE` is set, the data received on
standard input is written unmodified to standard output.
