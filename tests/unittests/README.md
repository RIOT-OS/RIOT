# Unittests

## Building and running tests
Tests can be built by calling:

```bash
cd tests/unittests
make
```

If there are tests for a module you even can build tests specifically for this module:

```bash
make tests-<module>
# e.g.
make tests-core
```

You then can run the tests by calling

```bash
make term
```

or flash them to your board as you would flash any RIOT application to the board (see [[board documentation|RIOT-Platforms]]).

### Other output formats
Other output formats using [*embUnit*](http://embunit.sourceforge.net/)'s ``textui`` library are available by setting the environment variable ``OUTPUT``:

* Compiler: ``OUTPUT="COMPILER"``
* Text: ``OUTPUT="TEXT"``
* XML: ``OUTPUT="XML"``

#### Compile example
```bash
OUTPUT="COMPILER" make tests-core
make term
```

(only outputs in case of test failures)

#### Text example
```bash
OUTPUT="TEXT" make tests-core
make term
```

```
- core_bitarithm_tests
1) OK test_SETBIT_null_null
2) OK test_SETBIT_null_limit
3) ...
- core_clist_tests
25) ...
- ...

OK (... tests)
```

#### XML example
```bash
OUTPUT="XML" make tests-core
make term
```

```XML
<?xml version="1.0" encoding='shift_jis' standalone='yes' ?>
<TestRun>
<core_bitarithm_tests>
<Test id="1">
<Name>test_SETBIT_null_null</Name>
</Test>
<Test id="2">
<Name>test_SETBIT_null_limit</Name>
</Test>
...
</core_bitarithm_tests>
<core_clist_tests>
<Test id="25">
<Name>test_clist_add_one</Name>
</Test>
...
</core_clist_tests>
<Statistics>
<Tests>...</Tests>
</Statistics>
</TestRun>
```

## Writing unit tests
### File struture
RIOT uses [*embUnit*](http://embunit.sourceforge.net/) for unit testing.
All unit tests are organized in ``tests/unittests`` and can be build module-wise, if needed.
For each module there exists a ``tests-<modulename>/tests-<modulename>.h`` file, at least one C file in ``tests-<modulename>/`` and a ``tests-<modulename>/Makefile``.
It is recommended to add a C file named ``tests-<modulename>/tests-<modulename>-<headername>.c`` for every header file that defines functions (or macros) implemented in the module.
If there is only one such header file ``tests-<modulename>/tests-<modulename>.c`` should suffice.

Each ``*.c`` file should implement a function defined in ``tests-<modulename>/tests-<modulename>.h``, named like

```C
Test *tests_<modulename>_<headername>_tests(void);

/* or respectively */

Test *tests_<modulename>_tests(void);
```

### Testing a module
To write new tests for a module you need to do three things:

1. **[Create a Makefile](#create-a-makefile)**: add a file ``tests-<modulename>/Makefile``
2. **[Define a test header](#define-a-test-header)**: add a file ``tests-<modulename>/tests-<modulename>.h``
3. **[Implement tests](#implement-tests)**: for each header file, that defines a function or macro implemented or related to the module, add a file ``tests-<modulename>/tests-<modulename>-<headername>.c`` or ``tests-<modulename>/tests-<modulename>.c`` if there is only one header.

#### Create a Makefile
The Makefile should have the following content:

```Makefile
include $(RIOTBASE)/Makefile.base
```

#### Define a test header.
The test header ``tests-<modulename>/tests-<module>.h`` of a module you add to ``tests/unittests/`` should have the following structure

```C
/*
 * Copyright (C) <year> <author>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-<module>.h
 * @brief       Unittests for the ``module`` module
 *
 * @author      <author>
 */
#ifndef __TESTS_<MODULE>_H_
#define __TESTS_<MODULE>_H_
#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generates tests for <header1>.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_<module>_<header1>_tests(void);

/**
 * @brief   Generates tests for <header2>.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_<module>_<header2>_tests(void);

/* ... */

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_<MODULE>_H_ */
/** @} */
```

#### Implement tests
Every ``tests-<modulename>/tests-<module>*.c`` file you add to ``tests/unittests/`` should have the following structure:

```C
/*
 * Copyright (C) <year> <author>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* clib includes */

#include "embUnit/embUnit.h"

#include "<header>.h"

#include "tests-<module>.h"

/* your macros */

/* your global variables */

static void set_up(void)
{
    /* omit if not needed */
}

static void tear_down(void)
{
    /* omit if not needed */
}

static void test_<function1>_<what1>(void) {
    /* ... */

    TEST_ASSERT(/* ... */);
}

static void test_<function1>_<what2>(void) {
    /* ... */

    TEST_ASSERT(/* ... */);
}

/* ... */

static void test_<function2>_<what1>(void) {
    /* ... */

    TEST_ASSERT(/* ... */);
}

static void test_<function2>_<what2>(void) {
    /* ... */

    TEST_ASSERT(/* ... */);
}

/* ... */

Test *tests_<module>_<header>_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_<function1>_<what1>),
        new_TestFixture(test_<function1>_<what2>),
        new_TestFixture(test_<function2>_<what1>),
        new_TestFixture(test_<function2>_<what2>),
        /* ... */
    };

    EMB_UNIT_TESTCALLER(<module>_<header>_tests, "<module>_<header>_tests",
                        tests_<module>_<header>_set_up,
                        tests_<module>_<header>_tear_down, fixtures);
    /* set up and tear down function can be NULL if omitted */

    return (Test *)&<module>_<header>;
}
```

The following assertion macros are available via *embUnit*

<table>
  <thead>
    <tr>
      <th>Assertion</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>
        <code>TEST_ASSERT_EQUAL_STRING(expected,actual)</code>
      </td>
      <td>
        Assert that strings actual and expected are equivalent
      </td>
    </tr>
    <tr>
      <td>
        <code>TEST_ASSERT_EQUAL_INT(expected,actual)</code>
      </td>
      <td>
        Assert that integers actual and expected are equivalent
      </td>
    </tr>
    <tr>
      <td>
        <code>TEST_ASSERT_NULL(pointer)</code>
      </td>
      <td>
        Assert that <code>pointer == NULL</code>
      </td>
    </tr>
    <tr>
      <td>
        <code>TEST_ASSERT_NOT_NULL(pointer)</code>
      </td>
      <td>
        Assert that <code>pointer != NULL</code>
      </td>
    </tr>
    <tr>
      <td>
        <code>TEST_ASSERT_MESSAGE(condition, message)</code>
      </td>
      <td>
        Assert that condition is <code>TRUE</code> (non-zero) or output customized <code>message</code> on failure.
      </td>
    </tr>
    <tr>
      <td>
        <code>TEST_ASSERT(condition)</code>
      </td>
      <td>
        Assert that condition is <code>TRUE</code> (non-zero)
      </td>
    </tr>
    <tr>
      <td>
        <code>TEST_FAIL(message)</code>
      </td>
      <td>
        Register a failed assertion with the specified message. No logical test is performed.
      </td>
    </tr>
  </tbody>
</table>
