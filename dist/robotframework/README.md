# Testing RIOT using the RobotFramework

## Requirements

RobotFramework is based on Python and is available as a Python package that
provides tools (binaries) and builtin libraries to run tests. Before running
or writing any test install the package by calling:

```
pip install robotframework
```

There is also a `requirements.txt` which also installs the `riot_pal` package
for HIL tests, to install all at once, run:

```
pip install -r dist/robotframework/requirements.txt
```

That is all for the initial setup. If you don't want to install RobotFramework
or any other Python dependency to your system environment you may consider to
use [virtualenv](https://virtualenv.pypa.io).

## Running Tests

The RobotFramework tests are integrated into RIOTs build and make system by
a distinct target, i.e., `make robot-test` to run tests and create the XML
and HTML output files. For the latter, you can also specify a custom output
folder by setting `RFOUTPATH` in your system environment - or directly when
calling `make robot-test`. By default all test results are store in:

```
<RIOTBASE>/build/robot/<BOARD>/<APPLICATION>/
```

A typical test invocation is to run the following from the RIOT root folder:

```
BOARD=samr21-xpro make -C tests/<test-name> flash robot-test
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
