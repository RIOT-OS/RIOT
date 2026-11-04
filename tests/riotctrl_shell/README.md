# Integration tests for `riotctrl_shell` ShellIntegrations and Parsers

See [`riotctrl`](https://pypi.org/project/riotctrl/) and [`dist/pythonlibs/riotctrl_shell/`](../../dist/pythonlibs/riotctrl_shell/)

IF YOU NEED TO CHANGE ANYTHING IN THESE TESTS, YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
See https://github.com/RIOT-OS/Release-Specs/

## On Mocking Modules

The dependency system of RIOT should be solid enough that defining a `MODULE_*` macro instead of including
a macro allows for monkeypatching, i.e. mocking, a module. To provide the mocked functionality, the
application can implement the functions used by the depending modules (if need be, just empty stubs).
This principle is applied in these test suites to include the code path of the shell command but mock at
layers at a certain point below that.

## Debugging the Tests
To debug these tests, we recommend to use [`pytest`](https://pytest.org) (using the `TEST_EXECUTOR`
environment variable) together with the debug logging capabilities of the tests (activated via
`DEBUG_TEST`):

```sh
TEST_EXECUTOR=pytest TEST_EXECUTOR_FLAGS="-s" DEBUG_TEST=1 make test
```

Add the [`-k`](https://docs.pytest.org/en/stable/reference/reference.html#cmdoption-k) flag to
`TEST_EXECUTOR_FLAGS` in case you want to filter the tests run by name (e.g. below a test method
called `test_to_isolate`):

```sh
TEST_EXECUTOR=pytest TEST_EXECUTOR_FLAGS="-s -k test_to_isolate" DEBUG_TEST=1 make test
```
