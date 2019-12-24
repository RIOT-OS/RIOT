Build system cflags with space test
===================================

This tests that the build system now handles CFLAGS correctly.

There is an automated test for CFLAGS with spaces when defined in the Makefile.


Other manual tests
------------------

There are also 2 other manual tests that can be run


### Modifying CFLAGS defined after including Makefile.include

CFLAGS defined after `Makefile.include` trigger a rebuild when changed.

Running the test once should work and then when changing `CONFIGURATION_VALUE`
it should pass too.

    make flash test
    CONFIGURATION_VALUE=1 make flash test


### Setting CFLAGS with space for docker

This one is a trickier as CFLAGS are modified in the Makefile, so cannot be
detected automatically in the docker handling. The solution is to pass it with
`DOCKER_ENVIRONMENT_CMDLINE`.

When the CFLAGS is defined like this, I did not find another solution than
escaping the space.

```
DOCKER_ENVIRONMENT_CMDLINE=$'-e CFLAGS=-DSTRING_FROM_DOCKER=\'\\\"with\ space\\\"\'' \
  BUILD_IN_DOCKER=1 make
grep '#define STRING_FROM_DOCKER "with space"' bin/native/riotbuild/riotbuild.h \
  || { echo 'ERROR CFLAGS not passed correctly' >&2; false; }

...
#define STRING_FROM_DOCKER "with space"
```
