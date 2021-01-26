Running and creating tests                        {#running-and-creating-tests}
==========================

There are a number of tests included in RIOT. They are located in the
[tests folder](https://github.com/RIOT-OS/RIOT/tree/master/tests). These tests
allow basic functionality to be verified as well as provide an example of
usage.


Implementing automated tests
----------------------------

The goal is to be able to run all tests in a sequential way for as many targets
as possible.

As some board can't be reset without a manual trigger tests should be implemented
with some kind of `synchronization`. This can be done in two ways:

- use `test_utils_interactive_sync` when uart input/output does not need to be
  disabled for the test. This is enabled by default.
- set up the test in a loop so the test script will be able so sync with some kind
  of start condition in the test.

The module for the first option is `test_utils_interactive_sync` and is set as a
default module in `Makefile.tests_common`. It can be disabled by setting in the
application makefile `DISABLE_MODULE += test_utils_interactive_sync`. The python
test script will adapt to it automatically.

When using the `shell` module, `test_utils_interactive_sync` will use the shell
itself to synchronize, and will not use `test_utils_interactive_sync();` function
to synchronize. Some times you will want to synchronize before the start of the
script and use `test_utils_interactive_sync();` function (e.g.:
[tests/ps_schedstatistics](tests/ps_schedstatistics/main.c)). For these cases
you can disable `test_utils_interactive_sync_shell` module in the application
`Makefile`: `DISABLE_MODULE += test_utils_interactive_sync_shell`.


Running automated tests
-----------------------

Some tests can be performed automatically. The test automation scripts are
defined in the `<test_application>/tests/` folder. They are written in python
and interact through the uart with the test application code running on a
board to do the validation. It is recommended to flash the board with the
test just before running it because some platforms cannot be reset while
testing.

From the test application directory run:

    BOARD=<board_of_your_choice> make flash test


An automated way of knowing if a test is available is to execute the
'test/available' target from the test application directory.
It executes without error if tests run by 'make test' are present.

    make test/available


Automated Tests Guidelines
--------------------------


When using `pexpect` `$` is useless for matching the end of a line, instead use
`\r\n`([pexpect end-of-line](https://pexpect.readthedocs.io/en/stable/overview.html#find-the-end-of-line-cr-lf-conventions)).

Beware of `+` and `*` at the end of patterns. These patterns will always get
a minimal match (non-greedy).([pexpect end-of-patterns](https://pexpect.readthedocs.io/en/stable/overview.html#beware-of-and-at-the-end-of-patterns))
This can be an issue when matching groups and using the matched groups to verify
some kind of behavior since `*` could return an empty match and `+` only a subset.

This is especially prevalent since `printf()` is buffered so the output might not
arrive in a single read to `pexpect`.

To avoid this make sure to match a non-ambiguous character at the end of the
pattern like `\r\n`, `\s`, `\)`, etc..

**don't**:

~~~~
    child.expect(r'some string: (\d+)')
~~~~

**do**:

~~~
    child.expect(r'some string: (\d+)\r\n')
~~~
~~~
    child.expect(r'some string: (\d+)\s')
~~~
~~~
    child.expect(r'some string: (\d+) ,')
~~~

Use expect() instead of assert()
--------------------------------

In order to make a test application functional in all cases, use `expect()`
instead of `assert()`. The former works like the latter, but will still be
compiled in if `NDEBUG` is defined. This is useful to keep a test application
working even when compiling with -DNDEBUG, allowing for the code-under-test to
be compiled with that flag.  Otherwise, the application would force compiling
all tested code with assertions enabled.
`expect()` is defined in the header `test_utils/expect.h`.

Interaction through the uart
----------------------------

Tests implemented with `testrunner` use the `cleanterm` target that
provides an interaction without adding extra text output or input handling.
It can currently be expected to have unmodified line based interaction with the
board.

The expected behavior is verified with the test in `tests/test_tools`.

Tests cannot rely on having on all boards and terminal programs:
* unbuffered input
* allowing sending special characters like `ctrl+c/ctrl+d`


Running tests that require a preliminary manual configuration
-------------------------------------------------------------

Some tests need active monitoring or manual setup steps but still have some
automated scripts. The test automation scripts are defined in the
`<test_application>/tests-with-config/` folder.
For running them, follow the setup or analysis documentation and use the
`test-with-config` target.

Running tests that require root privileges
------------------------------------------

Some tests require root privileges to launch their automated script. In this
case, the test automation scripts are defined in the
`<test_application>/tests-as-root/` folder.
For running them, follow the setup or analysis documentation and use the
`test-as-root` target.
