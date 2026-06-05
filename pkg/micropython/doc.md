@defgroup pkg_micropython MicroPython RIOT Port
@ingroup  pkg
@brief    MicroPython - Python for microcontrollers

*MicroPython is a lean and efficient implementation of the Python 3
programming language that includes a small subset of the Python standard
library and is optimised to run on microcontrollers and in constrained
environments.*

@see      The MicroPython Project Website has more information about the
          project, usage and limitations: https://micropython.org

## Status

MicroPython on RIOT has to be considered experimental. While the basic
interpreter works fairly well on native and Cortex-M, it has not seen much
testing.

## Hardware compatibility

The RIOT OS port targets microcontrollers that have at least 16 KiB of RAM and
256 KiB of flash. Although MicroPython has many different tuning options, the
requirement provides a decent out-of-the-box experience.

## Configuration options

Use the following environment variables in the application `Makefile` or from
the command line to configure MicroPython:

* `MP_RIOT_HEAPSIZE`: heap size for MicroPython, in bytes. Defaults to 16 KiB.

Example on the command line:

```shell
MP_RIOT_HEAPSIZE=2048 make -C examples/lang_support/community/micropython
```

## Implementation details

This package builds upstream MicroPython. The RIOT port itself lives in this
package's `contrib/` directory. It is compiled with MicroPython's own build
system, and is based on MicroPython's `ports/minimal` with some extra modules
enabled.

### Interrupts

MicroPython will handle interrupts in as scheduled callbacks, so they run in
the MicroPython's thread context. This means an interrupt will have a bit of
latency before the callback is executed.

## MicroPython's test suite

It is possible to run MicroPython's test suite for testing this port.

Steps:

```shell
make -C examples/lang_support/community/micropython flash mpy-tests
```

The default baud rate is 115200. The test will switch the interpreter into raw 
REPL mode for testing. Expect some tests to fail, depending on the enabled
features and configured heap size.

## MicroPython modules

MicroPython has a large number of built-in modules available. See the port
configuration file `contrib/mpconfigport.h` for more details on the exact set
of modules enabled in this port.

The following modules are built-in, but use RIOT's own APIs internally:

* `machine` - ADC, GPIO, SPI and more
* `time` - time functions

## RIOT specific modules

The following RIOT specific modules are available:

* `riot` - RIOT-specific functions
* `xtimer` - xtimer subsystem

You can import these modules and then use `help(<module name>)` to introspect
the available functions.

### thread_getpid()

```python
import riot
print(riot.thread_getpid())
```

### xtimer

```python
import xtimer

a = 0
def inc_a():
  global a
  a += 1

t = xtimer.xtimer(inc_a)
t.set(100000)
print(a)
```

The callback does not run in interrupt context. When the timer fires, its
execution is deferred via MicroPython's scheduler and runs in thread context.
See the implementation details section for more information.

## How to use

See `examples/lang_support/community/micropython` for example code.
