# Coding Conventions

## General

* Code shall be [C11](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)
  compliant, with a list of exceptions detailed below.
* Avoid dynamic memory allocation (malloc/free, new, etc.)! It will break
  real-time guarantees, increase code complexity, and make it more likely to use
  more memory than available.
* Avoid the use of floating point arithmetic. Not every MCU has a FPU and software
  floating point libraries cause unnecessary overhead.
  Instead use fixed-point integers and transform equations so that they stay within
  the range of integer math.
  An easy way to ensure this is by multiplying by a constant factor, ideally a power
  of two - this is a simple shift operation.
  Take care that intermediate values do not exceed the range of the data type you are using.
  When writing drivers, do not convert the measurement data into float, but instead
  choose an appropriate integer format / SI prefix.
* Please obey the Linux coding style as described in
  https://www.kernel.org/doc/Documentation/process/coding-style.rst with the
  following modifications and additions:
    * Line length: aim for no more than 80 characters per line, the absolute
      maximum should be 100 characters per line.
    * All line endings shall be set to LF (`\n`). (How to handle line endings in
      Git: https://help.github.com/articles/dealing-with-line-endings)
    * There must be no trailing whitespace in any line.
      The script `/dist/tools/whitespacecheck/check.sh master || exit` can be
      used to detect whitespaces at the end of line(s) that would lead to
      *Murdock* build error(s).
    * Use empty braces for empty while loops waiting for a hardware register
      instead of semicolon.
      `while (HW_STATUS != STATUS_OK) {}` is correct,
      `while (HW_STATUS != STATUS_OK);` is wrong.
    * Our policy regarding `typedef`s is
      [completely different](https://www.kernel.org/doc/html/latest/process/coding-style.html#typedefs)
      (see below) (BTW: Do we have any reason to do so?)
    * Comments should be C-style comments (see below)
* In order to follow Linux's recommendation on
  [conditional compilation](https://www.kernel.org/doc/html/latest/process/coding-style.html#conditional-compilation)
  make use of `IS_ACTIVE` and `IS_USED` macros from `kernel_defines.h` with C
  conditionals. If a symbol is not going to be defined under a certain
  condition, the usage of preprocessor `#if defined()` is fine.
* You can use [uncrustify](http://uncrustify.sourceforge.net/) with the provided
  option files: https://github.com/RIOT-OS/RIOT/blob/master/uncrustify-riot.cfg

## Standard Compliance

Using extensions to the C standard in general decreases portability and
maintainability: The former because porting RIOT to platforms for which limited
compiler options are available becomes more difficult when compiler-specific
extensions are used. The latter because extensions are often not as clearly
defined as standard C, not as well known within the C development community,
and have fewer resources to look up.

There are a number of cases in which using extensions cannot be avoided, or
would not be maintainable. For these cases, an exception can be made. A list
of recognised exceptions where we can (or even must) rely on extensions include:

- Use of `__attribute__((packed))` is allowed for serialization and
  de-serialization and only there. Ideally, it should not be used in public
  APIs and types.
- Code specific to MCU families may use extensions commonly used in this domain,
  such as inline assembly (e.g. as needed for context swapping), special
  function attributes (e.g. as needed for IRQ vector entries on some MCUs),
  etc. Code should still prefer standard compliance when there is no significant
  downside to it compared to using the extension.
- `#include_next` may be used when system headers need to be extended.
- Function attributes for which a wrapper exists in `compiler_hints.h` may be
  used using that wrapper. These wrappers either unlock additional optimization
  (such as `NORETURN` or `PURE`) or influence warnings (such as `MAYBE_UNUSED`)
  produced by the compiler and can simply be replaced by an empty token for
  compilers that do not support them.
- `__attribute__((used))`, `__attribute__((section("...")))`,
  `__attribute__((weak))`, and `__attribute__((alias("...")))`
  can be used where applicable. Unlike the wrappers in `compiler_hints.h`, we
  actually require toolchain support for them (an empty-token implementation
  will not generate correct binaries).

## Types

* Be careful with platform dependent type sizes like `int` or `long`. Use data
  types that include the bit length in the name like `uint16_t` when you need to
  make sure that a certain variable is exactly of this length.
* The use of typedefs for structs and pointers is allowed.
* Type definitions (using `typedef`) always end on "_t".
* If a typedef is used for a struct, it has to be specified at the struct
  definition (i.e., not as a separate line). E.g.:
```
    typedef struct {
        uint8_t a;
        uint8_t b;
    } foobar_t;
```
* Use of a separate line typedef for structs is allowed for forward
  declarations, e.g.,
```
    typedef struct mystruct mystruct_t;
    [...]
    struct mystruct {
         [...]
    };
```
* Guidelines for pointer types (as long as it is reasonable):
    - use `char *` for strings and only for strings
    - use `uint8_t[]` as type for arbitrary byte buffers, but use `void *` to
      pass them around. `uint8_t[]` because we're dealing with bytes and not
      characters, `void *` to avoid unnecessary casting shall the need arise to
      have their content to have a certain type
    - use `uint8_t *` to pass "typed" byte buffers, e.g., link-layer addresses,
      where it avoids unnecessary temporary variable
    - use `void *` for generic typing

## Variables

* Do NOT use global variables unless it is unavoidable.
* If you declare a variable within a header file, you MUST use the keyword
  `extern`.

## Functions

* Every function needs a prototype in addition to its definition. If a prototype
  is specified within a .c file it has to be declared BEFORE any function
  definitions.
* If the scope of a function is limited to one file, it MUST be declared static.
* Functions without parameters must be specified with `(void)`.
* Keep functions short! As a rule of thumb, the function's body should not
  exceed one screen.
* Do NOT use global macros defining more than one line of code. Use inline
  functions instead.

## Return values

* Every function must return one of the following values or none (void):
    * logical value (zero or not zero)
    * an error code (given as a negative number or zero) or a positive status
      value
    * the count of read or written bytes/values for I/O functions
    * the position or address (for search functions)
    * a pointer
* `NULL` indicates an error case, too.
* Do NOT return structs or other larger types! These would get copied to the
  stack, resulting in expensive operations. Moreover, some compilers have
  trouble with larger return types. Use pointers to structs instead and take
  care of the structs lifetime.
* If possible, prefer signed types over unsigned ones in order to be able to add
  error codes later on.

## Naming

*  Names of all public functions and variables must start with the name of the
   corresponding library, e.g.:
```
    thread_getpid(void);
    hwtimer_init_comp(uint32_t fcpu);
    int transceiver_pid;
```
* Private functions and variables do NOT have this library prefix.
* Do NOT use CamelCase. Function, variable and file names as well as enums,
  structs or typedefs are written in lowercase with underscores.
```
    /* instead of: */
    void CamelCaseNamedFunction(int camelCaseNamedVar);

    /* write: */
    void camel_case_named_function(int camel_case_named_var);
```
* When implementing constants or variables that are defined in third party
  documents such as RFCs, add a prefix to those names based on the RIOT coding
  conventions. If you use a name in the RIOT code that is different from the one
  in the third party document, you must add a reference to the original name of
  the constant or variable in the Doxygen documentation.

## Indentation and braces

* Indentations are four spaces (i.e., NO tab characters).
* As an exception to the Linux coding style, the closing brace is empty on a
  line of its own when followed by an else, too. When followed by a `while` in a
  `do`-statement, it goes into the same line.
* Use curly braces even for one-line blocks. This improves debugging and later
  additions.
```
    /* instead of: */
    if (debug) println("DEBUG");
    else println("DEBUG ELSE");

    /* write: */
    if (debug) {
        println("DEBUG");
    }
    else {
        println("DEBUG ELSE");
    }
```
* Commas are always followed by a space.
* For complex statements it is always good to use more parentheses - or split up
  the statement and simplify it.

## Indentation of Preprocessor Directives

Add two spaces of indent *after* the `#` per level of indent. Increment the
indent when entering conditional compilation using `#if`/`#ifdef`/`#ifndef`
(except for the include guard, which does not add to the indent). Treat indent
for C language statements and C preprocessor directives independently.

```
/* BAD: */
#if XOSC1
#define XOSC XOSC1
#define XOSC_NUM 1
#elif XOSC2
#define XOSC XSOC2
#define XOSC_NUM 2
#endif /* XOSC1/XOSC2 */
```

```
/* GOOD: */
#if XOSC1
#  define XOSC XOSC1
#  define XOSC_NUM 1
#elif XOSC2
#  define XOSC XSOC2
#  define XOSC_NUM 2
#endif
```

```
/* BAD: */
void init_foo(uint32_t param)
{
    (void)param;
    #if HAS_FOO
    switch (param) {
    case CASE1:
        do_foo_init_for_case1;
        break;
    #if HAS_CASE_2
    case CASE2:
        do_foo_init_for_case2;
        break;
        #endif
    #endif
}
```

```
/* GOOD: */
void init_foo(uint32_t param)
{
    (void)param;
#if HAS_FOO
    switch (param) {
    case CASE1:
        do_foo_init_for_case1;
        break;
#  if HAS_CASE_2
    case CASE2:
        do_foo_init_for_case2;
        break;
#  endif
#endif
}
```

### Reasoning

Adding the indent does improve readability a lot, more than adding comments.
Hence, we prefer the indent to allow reviewers to quickly grasp the structure
of the code.

Adding spaces before the `#` is not in compliance with the C standard (even
though in practice compilers will be just fine with whitespace in front), but
adding spaces afterwards is standard compliant. In either case, having the `#`
at the beginning of the line makes it visually stand out from C statements,
which eases reading the code.

Using an indent width of 2 makes preprocessor directives visually more
distinctive from C code, which helps to quickly understand the structure
of code.

## Includes

* The include of system headers (in <>-brackets) always precedes RIOT specific
  includes (in quotes).
* Optional headers must only be included if their corresponding module is
  selected/being build. In other words: always put an `#ifdef MODULE_...`
  statement around includes of optional headers:
```c
#ifdef MODULE_ABC
#  include "abc.h"
#endif
```

### Include What You Use (IWYU)

`#include` directives that are not actually needed should be removed to reduce
clutter and improve compilation speed. Similar: Try to add the corresponding
`#include`s for all the functions, macros, types, etc. used and do not rely on
`bar.h` to implicitly include `foo.h`, unless this is documented behavior.

Tools such as [clang's Include Cleaner][clangd-include-cleaner] can help with
that. These tools may show false positives in cases where headers are *expected*
to be included indirectly: E.g. if `foo.h` is the public header that contains
common helpers and implementations, but a per platform `foo_arch.h` is included
from within `foo.h` for platform specific implementations. If in this scenario
only functions provided by `foo_arch.h` are included, the `#include` of `foo.h`
is considered as unused. To avoid this, one should add
[`/* IWYU pragma: export */`](https://github.com/include-what-you-use/include-what-you-use/blob/master/docs/IWYUPragmas.md) after `#include "foo_arch.h"` in `foo.h`.

[clangd-include-cleaner]: https://clangd.llvm.org/design/include-cleaner

## Header Guards

All header files are required to either contain the widely supported `#pragma once`
preprocessor directive as the first line after the [copyright note](#documentation),
or header guards of the form

```c
#ifndef PATH_TO_FILE_FILENAME_H
#define PATH_TO_FILE_FILENAME_H

...
#endif /* PATH_TO_FILE_FILENAME_H */
```

Header guards are deprecated in RIOT header files and will gradually be converted
to `#pragma once`.

Rules for generating the guard name:

1. take the file name
2. if there's ```include/``` in the file's pathname, include the path from there
   on.
3. replace "/" and "." with "_"
4. convert to uppercase letters
5. if the produced guard starts with "_", prefix "PRIV"

Examples:

- "core/include/msg.h" -> "MSG_H"
- "sys/include/net/gnrc/pkt.h" -> NET_GNRC_PKT_H
- "drivers/abcd0815/abcd0815_params.h" -> ABCD0815_PARAMS_H
- "sys/module/_internal.h" -> PRIV_INTERNAL_H

Note: these rules will be enforced by the CI.

## C++ compatibility

* C Header files should be always wrapped for C++ compatibility to prevent
  issues with name mangling, i.e. mark all the containing functions and
  definitions as `extern "C"`
``` C
#ifdef __cplusplus
extern "C" {
#endif

... all your function declarations, global variables and defines belong here

#ifdef __cplusplus
}
#endif
```

* use `__restrict` instead of `restrict` in headers (compare
  https://github.com/RIOT-OS/RIOT/pull/2042)

## Absolute values

* Absolute values must be specified as macros or enums, not as literals, i.e.
  instead of
```
int timeout = 7 * 1000000;
```
write
```
int timeout = TIMEOUT_INTERVAL * USEC_PER_SEC;
```
## Comments
* All comments should be written as C-style comments.

E.g:
```
/* This is a C-style comment */
```
Wrong:
```
// C++ comment here
```

## Documentation

* All documentation must be in English.
* All files have to contain the copyright and the author note in the SPDX format.
* Doxygen documentation is mandatory for all header files.
* Every header file includes a general description about the provided
  functionality.
* Every function must be documented - including parameters and return value.

An exemplary doxygen documentation in a header file can look like this.

```
/*
 * SPDX-FileCopyrightText: 2014 Peter Schmerzl <peter@schmerzl-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     foobar
 * @{
 *
 * @file
 * @brief       Definitions for foo and bar functions.
 *
 * More detailed information about the file and the functionality implemented.
 *
 * @author      Peter Schmerzl <peter@schmerzl-os.org>
 *
 */

/**
 * @brief   Set the state of foobar.
 *
 * @param[in]  state      The new state of foobar.
 * @param[out] old_state  The old state of foobar is written to this variable.
 *
 * @return 1 if setting the state was successful, 0 otherwise.
 */
 int set_foobar(int state, int *old_state);
```

### SPDX

SPDX (System Package Data Exchange) is an open standard for adding
information about licenses, security information or other metadata to source files.
It allows for easy, automatic generation of SBOMs (Software Bill of Materials).

RIOT used to use the standard copyright format in a long form, however this
adds a lot of boilerplate code without much benefit. Furthermore the copyright
notices tend to vary depending on the author, making it difficult to parse
automatically and reliably.

Old Style - License Information:
```
/*
 * Copyright (C) 2013, 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

```

New Style - SPDX Format:
```
/*
 * SPDX-FileCopyrightText: 2013-2014 INRIA
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */
```

More information concerning the transition to SPDX format can be found
[here](https://github.com/RIOT-OS/RIOT/issues/21515).

## Common compilation warnings

Some solutions to correctly handle compilation warnings.

### -Wformat

Solution for string formatting errors:

* When printing a `size_t`
    * use `PRIuSIZE` from `architecture.h` because `newlib-nano` does not support `%zu`
* When printing an `unsigned char/uint8_t`
    * Use `%u` because `newlib-nano` does not support `%hu/PRIu8`
      [example](https://github.com/RIOT-OS/RIOT/pull/4851)
* When printing an `uint32_t`
    * Use `PRIu32` print format
      [example](https://github.com/RIOT-OS/RIOT/blob/4c74db4e7a6cf1e3be1edb3c10cdba14ba032513/drivers/sx127x/sx127x_getset.c#L126)
* When printing `64bit` variables
    * It is not correctly supported by `newlib-nano` as
      [said here](https://github.com/RIOT-OS/RIOT/issues/1891). It is
      recommended to use `fmt` module for these.
      [Example](https://github.com/RIOT-OS/RIOT/blob/e19f6463c09fc22c76c5b855799054cf27a697f1/tests/posix_semaphore/main.c#L277)

### -Wformat-nonliteral

For a `printf` style function with the following error: `error: format string is
not a string literal`.

* Function using a variable number of arguments:
    * Use `__attribute__((__format__ (__printf__, 3, 4)))`, where here `3` is
      the number of the argument with the format and `4` the format arguments,
      starting from 1. See
      [example](https://github.com/miri64/RIOT/blob/d6cdf4d06f2aeed05dcf86a5437254e2403e147b/pkg/openthread/contrib/platform_logging.c#L31-L32)
* Function using `va_list`:
    * Use `__attribute__((__format__ (__printf__, 1, 0)))`, where here `1` is
      the number of the argument with the format and `0` as there is no variable
      numbers of arguments. See
      [example](https://github.com/miri64/RIOT/blob/ad133da2096c44e001ee65071cb36db60a54e215/cpu/native/syscalls.c#L268-L271)


## Git

* Make one commit per change.
* The first line of the commit message describes the main feature of the commit.

## Continuous Integration
* If the CI tests fail due to errors these errors need to be addressed.
* If the CI tests fail due to warnings/errors emitted by cppcheck you should try
  to fix the error. If the error is definitely a false positive there is the
  possibility to suppress this warning/error. You MUST do so by adding a
  comment, including a rationale why it is a false positive and why the code
  can't be fixed otherwise, in the following format:
```
    /* cppcheck-suppress <category of error/warning>
     * (reason: cppcheck is being really silly. this is certainly not a
     * null-pointer dereference */
```

## Python coding convention

* Code shall be compliant with Python 3.10 at minimum, because this is the
  default Python 3 version in Ubuntu 22.04 (used as the reference system for
  CI).
* Code shall report no error when running the
  [Flake8](http://flake8.pycqa.org/en/latest/) tool, e.g:
    * for style checks described in
      [PEP 8](https://www.python.org/dev/peps/pep-0008/),
    * for lint checks provided by
      [Pyflakes](https://pypi.python.org/pypi/pyflakes),
    * for complexity checks provided by the
      [McCabe project](https://pypi.python.org/pypi/mccabe)
* A line length of maximum of 120 is allowed instead of 79 as per PEP 8. This
  increases tests readability as they can expects long line of output.
* Only runnable scripts shall start with `#!/usr/bin/env python3`
* Runnable scripts shall use the following scheme:

```python
#!/usr/bin/env python3

# Copyright (C) <your copyright>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# put the module imports first
# see https://www.python.org/dev/peps/pep-0008/#imports
# for more details
import module1
import module2

# Optional global variables
GLOBAL_VARIABLE = "I'm global"


# local functions, if required
def local_func():
    # Put your local function code here


# The main function
def main_func():
    # Put your main code here


if __name__ == "__main__":
    # Call the main function from here:
    main_func()
```
