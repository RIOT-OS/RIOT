# Testing RIOT using the RobotFramework

## Requirements

RobotFramework is based on Python and is available as a Python package that
provides tools (binaries) and builtin libraries to run tests. Before running
or writing any test install the package by calling:

```
pip install robotframework
```

That is all for the initial setup. If you don't want to install RobotFramework
to your system environment you may consider using [virtualenv](https://virtualenv.pypa.io).

## Running Tests

We integrated RobotFramework tests into RIOTs build and make system by providing
two distinct targets:

- `make robot-test` to run tests and create the XML and HTML output files
- `make robot-open` to open the generated `report.html` in your favourite browser

The latter is for convenient access of the output files, only, as they are
stored in a common build directory.
You can also specify a custom output folder by setting `RFOUTPATH` in your
system environment - or directly when calling `make robot-test`.

A typical test invocation is to run the following from the RIOT root folder:

```
BOARD=samr21-xpro make -C tests/xtimer_hang all flash robot-test
```

## Extending and Writing Tests

To write your own tests with the RobotFramework you only need to create a
`.robot` file in the `tests` subfolder of the test application, that way it
will be automatically found and executed by RIOTs build system. You may also
extend an existing robot test script in the mentioned directory - these files
also serve as examples for RIOT tests.

A general overview and examples on how to write robot tests can be found on
the [RobotFramework website](https://robotframework.org). Nevertheless, there
are a number of RIOT specific keywords that should be used when writing tests.
Please refer to the `*.keyword.txt` files in `dist/robotframework/res/`.

Note: any test script should write the system and build environment to the test
output. To do so please use the `Log System and Build Environment` keyword, e.g.
during `SUITE SETUP`:

```
Suite Setup         Run Keywords    Binary Should Exist
...                                 Log System and Build Environment
```
