Tests for riot-wrappers
=======================

Running these tests requires setting the RIOTBASE environment variable to a checkout of RIOT.

All tests work as build tests.

The tests can all safely be run (`make flash term`) on any board (provided the board files are correct),
and some have test runners (`make test`) that work for some or all boards.
These are run in CI for native.
