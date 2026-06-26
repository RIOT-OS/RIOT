@defgroup pkg_micropython MicroPython RIOT port
@ingroup  pkg
@brief    MicroPython - Python for microcontrollers
@see      https://micropython.org

"MicroPython is a lean and efficient implementation of the Python 3
 programming language that includes a small subset of the Python standard
 library and is optimised to run on microcontrollers and in constrained
 environments."

## Status

MicroPython on RIOT has to be considered experimental. While the basic
interpreter works fairly well on native and Cortex-M, it has not seen much
testing.

## Configuration options

Use the following environment variables in the application `Makefile` or from
the command line to configure MicroPython:

* `MP_RIOT_HEAPSIZE`: heap size for MicroPython, in bytes. Defaults to 16KiB.

Example on the command line:

```shell
MP_RIOT_HEAPSIZE=2048 make -C examples/lang_support/community/micropython
```

## Implementation details

This package builds upstream MicroPython. The RIOT port itself lives in this
package's `contrib/` directory. It is compiled with MicroPython's own build
system, and is based on MicroPython's `ports/minimal` with some extra modules
enabled.

## MicroPython's test suite

It is possible to run MicroPython's test suite for testing this port.

Steps:

1. `make -C examples/lang_support/community/micropython flash`
2. `cd examples/lang_support/community/micropython/bin/${BOARD}/pkg/micropython`
3. `cd tests`
4. `python3 run-tests.py -t <port>`

The default baud rate is 115200. The test will switch the interpreter into raw REPL mode for testing.

## MicroPython modules

Currently, the port enables only a subset of the available MicroPython
modules. See `contrib/mpconfigport.h` for details.

## RIOT specific modules

Currently, these are implemented:

* `riot`
* `xtimer`

You can import these modules and then use `help(<module name>)` to get more
information.

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

## How to use

See `examples/lang_support/community/micropython` for example code.
