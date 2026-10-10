# RIOT Coding Convention

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
"SHOULD", "SHOULD NOT", "RECOMMENDED", "NOT RECOMMENDED", "MAY", and
"OPTIONAL" in this document are to be interpreted as described in
[RFC2119] when they appear in ALL CAPS.  These words may also appear
in this document in lowercase, absent their normative meanings.

[RFC 2119]: https://www.rfc-editor.org/info/rfc2119/

## RIOT-Specific Rules

1. **Language**:
   1. C MUST be used for new hand-written embedded modules and SHALL comply with
     [C11](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf),
     with a list of exceptions detailed in the following.
   1. Tools or tests run on the host machine MAY be shell or Python scripts.
2. **Dynamic memory allocation** MUST be avoided (malloc/free, new, etc.)
  in embedded code. It will break real-time guarantees, increase code
  complexity, and make it more likely to use more memory than available.
3. **Floating-point arithmetic** MUST NOT be used.
  Not every microcontroller unit (MCU) has a floating-point unit (FPU)
  and software floating-point libraries cause unnecessary overhead.
  Instead use fixed-point (fixed-width) integers and transform equations so that
  they stay within the range of integer math.
  An easy way to ensure this is by multiplying by a constant factor, ideally a power
  of two -- this is a simple bit shift operation.
  Intermediate values MUST NOT exceed the range of the data type you are using,
  so special care must be taken.
  When writing drivers, measurements MUST NOT be converted into `float`s, but instead
  choose an appropriate integer format / SI prefix.
4. Code imported verbatim, e.g., through third-party packages (`pkg` folder) or
   vendor-provided code for drivers or CPUs, MAY not follow the coding convention.
5. Documentation MUST adhere to RIOT's [Documenting Conventions].
6. Names, identifiers, comments, and text MUST be in English.

## C Coding Style

1. You SHOULD use `clang-format` to format your code according to the coding style. 
2. You MAY disable formatting for certain sections where
   `clang-format` produces less readable code or code that deviates from
the recommended coding style (`// clang-format off`, `// clang-format on`).
3. You MAY use [uncrustify](http://uncrustify.sourceforge.net/) with the provided
[`uncrustify-riot.cfg`](https://github.com/RIOT-OS/RIOT/blob/master/uncrustify-riot.cfg)
file.

### File Formatting
1. **Encoding**: Files MUST use UTF-8.
   Restricting files to Extended ASCII is RECOMMENDED.
1. **Length**: A line SHOULD not be more than 80 characters, but MUST not
   exceed 100 characters.
1. **Endings**: All line endings SHALL be set to LF (`\n`).
   More details on how to handle line endings using git is available
   at [`help.github.com`](https://help.github.com/articles/dealing-with-line-endings).
1. **Whitespace**: There MUST NOT be any trailing whitespace on any line.
   The script `/dist/tools/whitespacecheck/check.sh master || exit` can be
   used to detect whitespaces at the end of line(s) that would lead to
   *Murdock* build error(s).

### Comments

Line comments SHOULD use the double-slash (`//`) style.
   ```c
   // This is a comment.

   // This comment spanning
   // multiple
   // lines.
   ```
   1. For documentation content, triple-slash (`///`) comments SHOULD be used.
      Also see `DOCUMENTING_CONVENTIONS.md` (TBD).
      ```c
      /// @name Some section
      /// @{
      
      /// @brief Enables foobar feature XYZ
      ///
      /// ...
      void foobar_enable_feature(void);

      /// @}
      ```
   1. Comments MUST be indented to the current indentation level 
      ```c
      if (a) {
          if (b) {
              // Condition is true
          }
      }
      ```
      ```c
      typedef struct {
          /// ...
          int a;
      
          /// ...
          bool b;
      } some_struct_t;
      ```
   1. The use of `TODO` and `FIXME` comments is OPTIONAL yet encouraged.
      1. Place a `TODO` comment everywhere you see room for future enhancement,
         a need for refactoring, or a planned feature.
         ```swift
         // TODO: Add fragmentation handling once PR #1234 is merged
         ```
      1. Add `FIXME` comments to temporary patches, known issues, and bugs,
         with a higher urgency.
         ```swift
         // FIXME: Setting local interface here as tinydtls fails to connect otherwise
         ```
   1. The use of `MARK`s is OPTIONAL yet can help navigate long files by 
      indicating sections or important locations in your code in the
      editor's minimap (Visual Studio Code, Xcode). Use a
      dash (`-`) to add a line to the 
      ```swift
      // MARK: unicoap_driver_extension_point
      // MARK: - Request handling
      ```
   1. Where C-style multi-line comments are still used, each line SHOULD
      start with an asterisk (`*`)
      ```c
      /* ✅ CORRECT
       * ...
       * ...
       */
      ```
      ```c
      /* ❌ WRONG
         ...
         ...
       */
      ```

### Statements and Nesting

1. **One statement per line**: A line MUST NOT contain more than one statement.
   Chained assignments such as `a = b = 0;` MUST NOT be used.
1. **Comma**: The comma MUST NOT be used as an operator, except in the
   initialization and increment clauses of a `for` loop. This does not affect
   commas that separate arguments or declarators.
1. **One declaration per line**: Declarations SHOULD declare one name per line.
   `int a, b;` is discouraged.
1. **Nesting**: 
   Nesting SHOULD be avoided if conditions can be combined into logical expressions.
   ```c
   // ✅ CORRECT
   if (dev == NULL || !dev->ready || len == 0 || buf == NULL) {
       return -EINVAL;
   }
   ```
   ```c
   // ❌ WRONG: four levels of nesting
   if (dev != NULL) {
       if (dev->ready) {
           if (len > 0) {
               if (buf != NULL) {
                   // ...
               }
           }
       }
   }
   ```

### Spacing

1. Indentations MUST be in units of 4 spaces and MUST not use tabs.
1. A single space MUST follow `if`, `switch`, `case`, `for`,
   `while`, and `do`. There MUST NOT be a space between a function or macro
   name and the opening parenthesis.
1. **Opening braces**: 
   An opening curly brace (`{`) after a keyword or parenthesized expression
   is preceded by exactly one space
   (`) {`, `else {`, `do {`).
   ```c
   // ✅ CORRECT
   if (condition) {
       // ...
   }
   ```
   ```c
   // ❌ WRONG
   if (condition){
       // ...
   }
   ```
   ```c
   // ✅ CORRECT
   (foobar_t) {
       // ...
   }
   ```
   ```c
   // ❌ WRONG
   (foobar_t){
       // ...
   }
   ```
1. **Blank lines**: 
   There MUST NOT be more than one consecutive blank line, and
   no blank line directly after an opening brace (`{`) or directly before a 
   closing curly brace (`}`).
1. **Operators**: 
   Binary and ternary operators (including `=`, `+=`,
   comparisons, and `?:`) are surrounded by one space on each side. Unary
   operators (`!`, `~`, `-`, `+`, `*`, `&`, `++`, `--`) attach to their operand.
   The member operators `.` and `->` have no surrounding spaces.
1. **Punctuation**: 
   Commas MUST be followed by a space.
   Every comma (`,`) and every semicolon (`;`) inside the
   parentheses of a `for` MUST be followed by a space, but not preceded.
1. Cases in a `switch` MUST bear the same level of indentation as the `switch`
   ```c
   switch (foo) {
   case BAR:
       printf("Hello");
       break;
   case ZOO:
       printf("World");
       break;
   default:
       break;
   }
   ```

### Wrapping

A statement that fits within the limit MAY stay on one line. Otherwise:

1. **Concatenation**:
   Static string concatenation MAY be used to wrap a string literal over multiple
   lines.
1. **Initializers**: 
   An initializer list or compound literal that does not fit
   SHOULD be split into multiple lines such that each element or designator
   is placed in a separate line, indented by 1 indentation unit.
   If multiple lines are used, a line SHALL NOT accommodate more than one
   element or designator.
   The closing brace goes on its own line, indented like the the first line.
   ```c
   // ✅ CORRECT
   (foobar_t) {
        .kind = FOOBAR_KIND_NICE,
        .property.sub = value,
        .flags = FOO_FLAG_ASYNC | FOO_FLAG_RETRY
   };
   ```
   ```c
   // ❌ WRONG
   (foobar_t) { .kind = FOOBAR_KIND_NICE,
                .property.sub = value,
                .flags = FOO_FLAG_ASYNC | FOO_FLAG_RETRY
   };
   ```
1. **Function parameters**:
   Long function parameter lists SHOULD be split into multiple lines with one
   parameter each. The first parameter SHOULD be put in a newline and SHOULD NOT
   immediately follow the opening parenthesis.
   ```c
   // ✅ CORRECT (one line)
   void some_module_do_something(complex_type_t* some_type, const char* name);
   ```
   ```c
   // ✅ CORRECT (linebreak, several lines, one parameter each)
   void some_module_transfer(
       foo_t *dev,
       const void *data,
       size_t length,
       uint32_t flags
   ); // or {
   ```
   This prevents indenting parameters too far too the right given a long function name.
   ```c
   // ❌ WRONG (no linebreak)
   void some_module_perform_some_operation(complex_type_t* some_type,
                                           const uint8_t* next,
                                           module_flags_t flags,
                                           module_parameters_t parameters,
                                           module_callback_t callback
   );
   ```
   A parameter list spanning more than one line MUST NOT contain
   more than one parameter per line. This allows identifying changes in `git`
   differences quicker and is generally more legible.
   ```c
   // ❌ WRONG (multiple parameters per line)
   void some_module_transfer(
       complex_type_t* some_type, const uint8_t* next,
       module_flags_t flags, module_parameters_t parameters,
       module_callback_t callback
   );
   ```
1. **Conditions**: 
   In conditions of `if`, `while`, `for`, and `switch`, it is RECOMMENDED 
   put each item on its own line, with 1 additional indentation unit.
   The closing parenthesis goes on a line of its own at that line's indentation, followed
   by `;`, ` {`, or what else belongs after it. B
   Boolean operators SHOULD be placed at the end of the line.
   ```c
   // ✅ CORRECT
   if (condition_1 &&
       condition_2 &&
       condition_3
   ) {
       // ...
   }
   // or should be require newline after if ( ???
   if (
       condition_1 &&
       condition_2 &&
       condition_3
   ) {
       // ...
   }
   ```
1. **Parenthesized lists**:
   In calls, declarations, and definitions, it is RECOMMENDED 
   put each item on its own line, with 1 additional indentation unit.
   But if multiple (indented) lines are used, the first item MUST NOT be placed after the
   opening parenthesis (`(`), but instead be preceded by a linebreak.
   ```c
   // ✅ CORRECT
   res = foo_transfer(dev, buffer, length, FOO_FLAG_ASYNC | FOO_FLAG_RETRY);
   ```
   ```c
   // ✅ CORRECT
   res = foo_transfer(
       dev,
       buffer,
       length,
       FOO_FLAG_ASYNC | FOO_FLAG_RETRY
   );
   ```
   ```c
   // ❌ WRONG
   res = foo_transfer(dev,
                      buffer,
                      length,
                      FOO_FLAG_ASYNC | FOO_FLAG_RETRY
   );
   ```

### Braces and Parentheses

1. Complex subexpressions are RECOMMENDED to be wrapped in parentheses, or be split
   up to improve readability.
1. Curly braces for blocks SHOULD appear on the same line as the control clause or function
   header.
   ```c
   if (condition) {
       // ...
   } else {
       // ...
   }
   
   switch (expression) {
       // ...
   }

   while (condition) {
       // ...
   }

   do {
       // ...
   } while (condition);

   for (init; condition; action) {
       // ...
   }

   void my_function(void) {
       // ...
   }
   ```
1. Curly braces are REQUIRED even for one-line blocks. This improves debugging and later
   additions.
   ```c
   // ✅ CORRECT
   if (condition) {
       println("condition is true");
   }
   else {
       println("condition is false");
   }
   ```
   ```c
   // ❌ WRONG
   if (debug) println("condition is true");
   else println("condition is false");
   ```
1. Empty braces MUST be used for empty `while` loops instead of a semicolon, e.g., 
   while waiting for a hardware register.
   ```c
   // ✅ CORRECT
   while (HW_STATUS != STATUS_OK) {}
   ```
   ```c
   // ❌ WRONG
   while (HW_STATUS != STATUS_OK)
   ```
1. **Preprocessor directives**:
   C11 allows spaces before preprocessor directives. readability questionable?
   Thoughts?

   ```c
   // 🤨 this really more readable??
   #if XOSC1
   #  define XOSC XOSC1
   #  define XOSC_NUM 1
   #elif XOSC2
   #  define XOSC XSOC2
   #  define XOSC_NUM 2
   #endif
   ```

   ```c
   // 🤨 than this? i find this miles easier to parse tbh.
   #if XOSC1
     #define XOSC XOSC1
     #define XOSC_NUM 1
   #elif XOSC2
     #define XOSC XSOC2
     #define XOSC_NUM 2
   #endif
   ```
   What we had before:
   > Add two spaces of indent *after* the `#` per level of indent. Increment the
   > indent when entering conditional compilation using `#if`/`#ifdef`/`#ifndef`
   > (except for the include guard, which does not add to the indent). Treat indent
   > for C language statements and C preprocessor directives independently.
   > 
   > ```c
   > /* BAD: */
   > #if XOSC1
   > #define XOSC XOSC1
   > #define XOSC_NUM 1
   > #elif XOSC2
   > #define XOSC XSOC2
   > #define XOSC_NUM 2
   > #endif /* XOSC1/XOSC2 */
   > ```
   > 
   > ```c
   > /* GOOD: */
   > #if XOSC1
   > #  define XOSC XOSC1
   > #  define XOSC_NUM 1
   > #elif XOSC2
   > #  define XOSC XSOC2
   > #  define XOSC_NUM 2
   > #endif
   > ```
   > 
   > ```c
   > /* BAD: */
   > void init_foo(uint32_t param)
   > {
   >     (void)param;
   >     #if HAS_FOO
   >     switch (param) {
   >     case CASE1:
   >         do_foo_init_for_case1;
   >         break;
   >     #if HAS_CASE_2
   >     case CASE2:
   >         do_foo_init_for_case2;
   >         break;
   >         #endif
   >     #endif
   > }
   > ```
   > 
   > ```c
   > /* GOOD: */
   > void init_foo(uint32_t param)
   > {
   >     (void)param;
   > #if HAS_FOO
   >     switch (param) {
   >     case CASE1:
   >         do_foo_init_for_case1;
   >         break;
   > #  if HAS_CASE_2
   >     case CASE2:
   >         do_foo_init_for_case2;
   >         break;
   > #  endif
   > #endif
   > }
   > ```
   > 
   > ### Reasoning
   > 
   > Adding the indent does improve readability a lot, more than adding comments.
   > Hence, we prefer the indent to allow reviewers to quickly grasp the structure
   > of the code.
   > 
   > Adding spaces before the `#` is not in compliance with the C standard (even
   > though in practice compilers will be just fine with whitespace in front), but
   > adding spaces afterwards is standard compliant. In either case, having the `#`
   > at the beginning of the line makes it visually stand out from C statements,
   > which eases reading the code.
   > 
   > Using an indent width of 2 makes preprocessor directives visually more
   > distinctive from C code, which helps to quickly understand the structure
   > of code.

### Macros and Configuration

1. Macros spanning multiple lines are RECOMMENDED to have the escape character (`\`) at
   the end of reach line aligned. Emitted expressions should enclosed in parentheses.
   ```c
   #define MY_STRUCT(a, b, c) ((my_struct_t) { \
       .field_a = a,                           \
       .field_b = b,                           \
       .field_c = c                            \
    })
   ```
1. A function-like macro that expands to more than one
   statement MUST be wrapped in `do { ... } while (0)`, without a trailing `;`.
1. Macro arguments MUST be parenthesized if used as an expression in the expansion. 
1. A macro MUST NOT hide control flow (`return`, `goto`,
   `break`, `continue`), MUST NOT rely on a local variable with a fixed name (other than built-ins)
   at the call site.

### Naming

This section applies to C identifiers, i.e., variable names, parameters,
fields, `struct`/`union`/`enum` identifiers, `typedef` identifiers, macro
names, and labels.

1. **Case**: 
   You MUST use lowercase snake case (`snake_case`). Macros SHOULD use
   uppercase (screaming) snake case (`SNAKE_CASE`).
   ```c
   // ❌ WRONG
   void CamelCaseNamedFunction(int camelCaseNamedVar);
   ```
   ```c
   // ✅ CORRECT
   void snake_case_named_function(int snake_case_named_var);
   ```
1. **Wording**:
    1. Descriptive speaking name are REQUIRED over abbreviated ones
      where the abbreviation is not commonly understood.
    1. Names with a very small scope, such as loop counters
      and temporaries, MAY be short (`i`, `j`, `tmp`, `res`). Anything visible
      beyond a single function MUST have a descriptive name.
    1. You MUST NOT encode the type into names (`u8_count`, `pszName`, `btReady`).
1. **Quantities**: 
   For variables, parameters, and fields, the term `count`
   SHOULD be used for the number of elements in a collection.
   For contiguous data, such as byte buffers, the term `length` SHOULD be used.
   For the maximum available space in a collection, the term `capacity` SHOULD
   be used to describe the number of available element slots. Avoid `len` and
   `max` in new code.
1. **Functions and macros**:
   * A function that performs an **action** is named after it, e.g., `foo_start`.
   * A function that answers a yes/no question is named as a **predicate**, e.g., 
     `foo_is_running`, `foo_has_data`, `foo_can_sleep`.
1. **Namespaces**:
    1. Publicly usable names MUST be prefixed with a module, library,
       or feature name followed by
       an underscore. Additions to global utilities the core MAY not need a prefix.
       ```c
       thread_getpid(void);
       hwtimer_init_comp(uint32_t fcpu);
       int transceiver_pid;
       ```
       Private or local definitions MAY omit this prefix, e.g., those used only
       in a `.c` file.
       When implementing constants or variables that are defined in third party
       documents such as RFCs, you SHOULD add a prefix to those names based on the RIOT coding
       conventions. If you use a name in the RIOT code that is different from the one
       in the third party document, you MUST add a reference to the original name of
       the constant or variable in the Doxygen documentation.
    1. 'Private' APIs that have to exposed but are not supposed to be used
       MAY be prefixed with an underscore but MUST not have a double-underscore
       prefix (`__`) as those are reserved for the C compiler.
1. **Inclusive terminology**: 
   New code SHOULD NOT use _master/slave_ or
   _blacklist/whitelist_.
   Instead, you SHOULD adopt _controller/peripheral_ (SPI), _controller/target_ (I²C),
   _primary/secondary_, _initiator/target_, and _denylist/allowlist_ throughout.

### Copyright Information

SPDX (System Package Data Exchange) is an open standard for adding
information about licenses, security information or other metadata to source files.
It allows for easy, automatic generation of SBOMs (Software Bill of Materials).

RIOT used to use the standard copyright format in a long form, however this
adds a lot of boilerplate code without much benefit. Furthermore the copyright
notices tend to vary depending on the author, making it difficult to parse
automatically and reliably.

Old Style - License Information:
```c
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
```c
// SPDX-FileCopyrightText: 2013-2014 INRIA
// SPDX-FileCopyrightText: 2015 Freie Universität Berlin
// SPDX-License-Identifier: LGPL-2.1-only
```

More information concerning the transition to SPDX format can be found
[here](https://github.com/RIOT-OS/RIOT/issues/21515).


## C Coding Practices

### Magic Numbers and Absolute Values

Absolute values MUST be specified as macros or enum cases, i.e.,
MUST NOT appear as literals.

```c
// ❌ WRONG
int timeout = 7 * 1000000;
```
```c
// ✅ CORRECT
int timeout = TIMEOUT_INTERVAL * USEC_PER_SEC;
```

### Types

1. Fixed-width integer types SHOULD be used to reliably control the size
   of integer fields, such as using `uint16_t`.
   Be careful with platform-dependent type sizes like `int` or
   `long` as their bit width depends on the platform you are building for.
2. `size_t` and `ssize_t` are RECOMMENDED to be used for counts, lengths, and capacities
   instead of, e.g., `unsigned int`.
3. `typedef`
    1. `typedef`s MAY be used for `struct`s, `union`s, `enum`s, and pointers.
    1. `typedef` names MUST end in `_t`
    1. `typedef`s that alias a `struct`, `union`, or `enum` MUST include the struct definition.
      ```c
      typedef struct {
          uint8_t a;
          uint8_t b;
      } foobar_t;
      ```
      You MAY forward-declare a `struct`, `union`, or `enum` and use a separate
      line for the `typedef` then.
      ```c
      typedef struct mystruct mystruct_t;
      // ...
      struct mystruct {
          // ...
      };
      ```
      In referencing code, you MUST use the `typedef` name ending in `_t` whenever possible.
4. Pointers
     1. `char *` SHOULD only be used for strings, i.e., text
     1. `uint8_t *` SHOULD be used for byte buffers
     1. `void *` MAY be used for collections of data with
        variable, generic, or unknown type.
       `void *` SHOULD be avoided whenever this is feasible.
5. **Enumerations vs. macros**:
   `typedef`'ed `enum`s SHOULD be used for special numbers of common set instead of
    macros if all cases/flags are known. E.g., `enum`s are RECOMMENDED
    for bit flags.

### Variables

1. Global variables MUST not be used except in rare exception cases where this
   global variables cannot be avoided.
2. Variables declared in C headers MUST be specified with the `extern` keyword.
3. You SHOULD not define multiple variables in the statement using a comma, i.e.,
   a declarator SHOULD NOT have multiple parallel declarators:
   Declarations such as `int a, b;` are discouraged, especially if the
   declarators in the declaration are of diverging kind, such as in
   `const char multi[], single;`.

### Functions

1. **Prototypes/parameters:**
  Functions are REQUIRED to have a prototype with named parameters.
   1. If a prototype is specified within a `.c` file, it MUST be declared BEFORE any function definitions.
   1. Functions without parameters MUST be specified with `(void)`.
1. **`static`**: 
  If the scope of a function is limited to one file, it MUST be declared `static`.
1. **`static` `inline`**
   functions in headers are RECOMMENDED over macros, but
   use of either depends on the specific circumstance. `static inline`
   is for tiny functions that can likely be folded away by the compiler.
   File-local functions MUST NOT be marked `inline` just because
   they have one caller (that's up to the compiler).
1. **`extern`** MUST NOT be used on function declarations in headers, in contrast
   to variables.
1. **Complexity**: 
   A function SHOULD do one thing. For its body, towards 100 lines you start
   thinking about splitting up the function. Note that this does obviously not
   include longer multi-line comments.
   A function SHOULD NOT have more than about 10 local variables.
1. **`goto`** MAY be used to jump forward within a function to
    cleanup or error handling code. 
    It MUST NOT be used to build loops or to jump into a
    block. If a function needs no cleanup, it SHOULD return directly.

### Return values

1. **Error handling**
    1. It is RECOMMENDED to return an integer that is negative to indicate
      an error and zero or positive to indicate success.
    1. Whenever possible, errors returned SHOULD be the negative version of
      an error number in `errno.h`.
    1. To avoid source-breaking changes later on, you SHOULD consider
      using a signed integer return type instead of an unsigned one, if functionality
      added in the future can lead to the function failing
    1. If the function does return a useful integer value, such as
      a count, length, size, or capacity, and otherwise
      an error, you SHOULD prefer `ssize_t` over `int`. Otherwise, the function
      SHOULD return an `int` if it does not have a useful return value apart
      from the error number.
    1. If the function needs to return a pointer, it SHOULD return a value of
      `NULL` to indicate an error that is not further specified. Consider
      returning a signed integer and turning the pointer return value to
      an out parameter to specify what error occurred.
    1. A function that performs an **action** (e.g., `foo_start`) SHOULD NOT
      have a return type of `bool` to indicate error/success outcomes.
      Likewise, a **predicate** function (e.g., `foo_is_running`,
      `foo_has_data`, `foo_can_sleep`) SHOULD return `bool` and SHOULD
      NOT return errors.
1. **Returning by value:** 
   Functions with external linkage MUST NOT return `struct`s, `struct`s in a `union`s, or other
   large types. These would get copied onto the
   stack, resulting in expensive operations.
   Use pointers to structs instead and take
   care of the structs lifetime. `static inline` function with small, simple
   bodies MAY construct and return `struct`s.

### Assertions

1. `assert` MAY be used to assert preconditions on function parameters (not `NULL`, etc)
1. Invalid configurations that can be caught a compile time should use `static_assert` or
   a preprocessor `if` directive combined with `#error` or `#warning`.

### Conditional Compilation

In order to follow Linux's recommendation on
[conditional compilation](https://www.kernel.org/doc/html/latest/process/coding-style.html#conditional-compilation)
make use of `IS_ACTIVE` and `IS_USED` macros from `kernel_defines.h` with C
conditionals.
If a symbol is not going to be defined under a certain condition,
you MAY use `#if defined()`.

Optional branches in C code SHOULD use macros instead of preprocessor
conditionals:

```c
// ✅ GOOD
if (IS_USED(MODULE_GNRC_IPV6_EXT_FRAG_STATS)) {
    stats.fragments += 1;
    stats.datagrams += 1;
}
```

```c
// ❌ BAD
#if MODULE_GNRC_IPV6_EXT_FRAG_STATS
    stats.fragments += 1;
    stats.datagrams += 1;
#endif // MODULE_GNRC_IPV6_EXT_FRAG_STATS
```

That way tooling such as language servers and static code analysers will still
see the code and perform their checks on it.
The compiler will eliminate dead branches.

If preprocessor conditionals are needed, use `#ifdef MODULE_FOO` or
`#if MODULE_FOO` instead of `#if IS_USED(MODULE_FOO)`
The general rule is to reduce preprocessor statements as much as possible.
Instead of guarding individual code sections, add a stub or use early returns:

```c
// ❌ BAD
#ifdef MODULE_FOO
#  include "foo.h"
#endif // defined(MODULE_FOO)

#ifdef MODULE_FOO
static void _do_foo(void) {
    // do foo
    ...
}
#endif // defined(MODULE_FOO)

void bar(my_type t) {
    switch(t)
#ifdef MODULE_FOO
    case MY_TYPE_FOO:
        _do_foo();
#endif // defined(MODULE_FOO)
    ...
}
```

```c
// ✅ GOOD: using stubs
#include "foo.h"

#ifdef MODULE_FOO
static void _do_foo(void) {
    // ...
}
#else
// No-op stub
void _do_foo(void) {
    return;
}
#endif // defined(MODULE_FOO)

void bar(my_type t) {
    switch(t)
    case MY_TYPE_FOO:
        _do_foo();
    ...
}
```

```c
// ✅ GOOD: returning early
#include "foo.h"

static void _do_foo(void) {
    if (!IS_USED(MODULE_FOO)) {
        return;
    }
    // ...
}

void bar(my_type t) {
    switch(t)
    case MY_TYPE_FOO:
        _do_foo();
        break;
    // ...
}
```

### Headers

1. **Guard**:
   All header files are REQUIRED to either contain the widely supported `#pragma once`
   preprocessor directive as the first line after the [copyright note](#documentation),
   or header guards of the form. `#pragma once` is RECOMMENDED.
   
   Header guards are deprecated in RIOT header files and will gradually be converted
   to `#pragma once`.

   ```c
   #ifndef PATH_TO_FILE_FILENAME_H
   #define PATH_TO_FILE_FILENAME_H
   // ...
   #endif /* PATH_TO_FILE_FILENAME_H */
   ```

   Rules for generating the guard name:
   1. Take the file name.
   2. If there's `include/` in the file path, include the path from there on.
   3. Replace `/` and `.` with `_`.
   4. Convert to uppercase letters.
   5. If the produced guard starts with `_`, prefix "PRIV".
   
   Examples:
   - `core/include/msg.h -> MSG_H`
   - `sys/include/net/gnrc/pkt.h -> NET_GNRC_PKT_H`
   - `drivers/abcd0815/abcd0815_params.h -> ABCD0815_PARAMS_H`
   - `sys/module/_internal.h -> PRIV_INTERNAL_H`

    These rules will be enforced by the CI.
1. **Includes**:
    1. **System headers**:
      System-header `#include`s MUST precede RIOT-specific `#include`s.
      System header names MUST be written in angled brackets (`<header.h>`)
      RIOT header names MUST be written in double quotes (`"header.h"`)
    1. **Include What You Use (IWYU)**:
     `#include` directives that are not actually needed SHOULD be removed to reduce
     clutter and improve compilation speed. Similarly, try to add the corresponding
     `#include`s for all the functions, macros, types, etc. used and do not rely on
     `bar.h` to implicitly include `foo.h`, unless this is documented behavior.
     Tools such as [clang's Include Cleaner][clangd-include-cleaner] can help with
     that. These tools may show false positives in cases where headers are *expected*
     to be included indirectly: E.g. if `foo.h` is the public header that contains
     common helpers and implementations, but a per platform `foo_arch.h` is included
     from within `foo.h` for platform specific implementations. If in this scenario
     only functions provided by `foo_arch.h` are included, the `#include` of `foo.h`
     is considered as unused. To avoid this, one SHOULD add
     [`/* IWYU pragma: export */`](https://github.com/include-what-you-use/include-what-you-use/blob/master/docs/IWYUPragmas.md) after `#include "foo_arch.h"` in `foo.h`.
1. **C++ compatibility**:
    1. C Header files MUST always be wrapped in `extern "C" {` blocks
      for C++ compatibility to prevent issues with name mangling.
      ```c
      // Includes go here
      
      #ifdef __cplusplus
      extern "C" {
      #endif
      
      // All your function declarations, global variables and defines belong here...
      
      #ifdef __cplusplus
      }
      #endif
      ```
    1. `__restrict` MUST be used instead of `restrict` in headers.
      (cf. [PR #2042](https://github.com/RIOT-OS/RIOT/pull/2042))
     
[clangd-include-cleaner]: https://clangd.llvm.org/design/include-cleaner

The top of a header should look like this.

```c
// SPDX-FileCopyrightText: Year Entity
// SPDX-License-Identifier: LGPL-2.1-only

#pragma once

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "ztimer.h"
#include "thread.h"
#include "mutex.h"

#ifdef MODULE_ABC
#  include "abc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ...

#ifdef __cplusplus
}
#endif
```

### Format Strings and `printf`

1. Use static string concatenation wherever possible.
    1.g., `"Hello, " "World"`.
1. Format string SHOULD NOT use format specifiers directly but only through
   macros defined in `"architecture.h"` and `<inttypes.h>`. Note that
   `"architecture.h"` includes `<inttypes.h>`.
   * Use `PRIuSIZE` for `size_t` values, `PRIdSIZE` for `ssize_t`.
   * Use `PRIu8` for `uint8_t`.
   * Use `PRIu32` for `uint32_t`.
   * `%i` MAY be used for `int`s.
   * Printing 64-bit integers is not correctly supported by `newlib-nano` 
      ([#1891](https://github.com/RIOT-OS/RIOT/issues/1891)). It is
      RECOMMENDED to use `fmt` module for these
      ([example](https://github.com/RIOT-OS/RIOT/blob/e19f6463c09fc22c76c5b855799054cf27a697f1/tests/posix_semaphore/main.c#L277)).
1. Functions using a variable number of arguments:
   Use `__attribute__((__format__ (__printf__, 3, 4)))`, where `3` is
   the 1-based index of the format argument `4` the starting index of
   variadic arguments 
   ([example](https://github.com/miri64/RIOT/blob/d6cdf4d06f2aeed05dcf86a5437254e2403e147b/pkg/openthread/contrib/platform_logging.c#L31-L32)).
1. Function using `va_list`:
   Use `__attribute__((__format__ (__printf__, 1, 0)))`, where `1` is
   1-based index of the format argument and `0` indicating there are not
   variadic arguments
   ([example](https://github.com/miri64/RIOT/blob/ad133da2096c44e001ee65071cb36db60a54e215/cpu/native/syscalls.c#L268-L271)).

### Language Extensions

C language extensions SHOULD be avoided whenever possible.
Using extensions to the C standard in general decreases portability and
maintainability: The former because porting RIOT to platforms for which limited
compiler options are available becomes more difficult when compiler-specific
extensions are used. The latter because extensions are often not as clearly
defined as standard C, not as well known within the C development community,
and have fewer resources to look up.

There are a number of cases in which using extensions cannot be avoided, or
would not be maintainable. For these cases, an exception MAY be made. A list
of recognized exceptions where we can (or even must) rely on extensions include:

- Use of `__attribute__((packed))` is allowed for serialization and
  de-serialization and only there. Ideally, it SHOULD NOT be used in public
  APIs and types.
- Code specific to MCU families may use extensions commonly used in this domain,
  such as inline assembly (e.g. as needed for context swapping), special
  function attributes (e.g. as needed for IRQ vector entries on some MCUs),
  etc. Code SHOULD still prefer standard compliance when there is no significant
  downside to it compared to using the extension.
- `#include_next` MAY be used when system headers need to be extended.
- Function attributes for which a wrapper exists in `compiler_hints.h` MAY be
  used using that wrapper. These wrappers either unlock additional optimization
  (such as `NORETURN` or `PURE`) or influence warnings (such as `MAYBE_UNUSED`)
  produced by the compiler and can simply be replaced by an empty token for
  compilers that do not support them.
- `__attribute__((used))`, `__attribute__((section("...")))`,
  `__attribute__((weak))`, and `__attribute__((alias("...")))`
  MAY be used where applicable. Unlike the wrappers in `compiler_hints.h`, we
  actually require toolchain support for them (an empty-token implementation
  will not generate correct binaries).
- `__restrict`, `__attribute__((format))` and `ssize_t`
- Inline assembly MUST only appear in code specific to an MCU family
  or architecture, never in portable code, and only where C cannot express it.
  
### Compiler Hints

Wherever possible, the following indications are RECOMMENDED.
To leverage them, include `compiler_hints.h`, which also defines
further compiler annotations. These are the ones to 

1. **Control paths**: 
   `UNREACHABLE()` signals this code path can never be taken.
   `likely(...)` and `unlikely(...)`applied to boolean expressions, such
   as in `if` and `while` conditions help differentiate hot and cold paths.
   ```c
   if (unlikely(sun_streak && temp >= 25 && grass == "green")) {
       // ...
   }
   switch (operation) {
   case FOOBAR_ADD:
       // ...
       break;
   case FOOBAR_UPDATE:
       // ...
       break;
   case FOOBAR_REMOVE:
       // ...
       break;
   default:
       // Impossible to land here
       UNREACHABLE();
   }
   ```
1. **Strings**:
   `NONSTRING` can be added to string pointers `char *`/`char[]` that are not
   null-terminated.
   ```c
   // guess is not null-terminated, length indicated as argument
   void play_who_am_i(const char* guess NONSTRING, size_t length);
   ```


## Python Coding Style and Practices

1. Code SHALL be compliant with Python 3.10 at minimum, because this is the
   default Python 3 version in Ubuntu 22.04 (used as the reference system for
   CI).
1. Code SHALL report no error when running the
   [Flake8](http://flake8.pycqa.org/en/latest/) tool, e.g:
     * for style checks described in
       [PEP 8](https://www.python.org/dev/peps/pep-0008/),
     * for lint checks provided by
       [Pyflakes](https://pypi.python.org/pypi/pyflakes),
     * for complexity checks provided by the
       [McCabe project](https://pypi.python.org/pypi/mccabe)
   * A line MUST NOT exceed a length of 120 characters instead of 79 as per
     PEP 8. This increases tests readability as they can expects long line of
     output.
* Only runnable scripts SHALL start with `#!/usr/bin/env python3`
* Runnable scripts SHALL use the following scheme:

```python
#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 <your name/company>
# SPDX-License-Identifier: LGPL-2.1-only

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

## Tooling

### Git

* Try to group your changes into commits that focus on a certain area,
  for example: "cpu/stm32: fix ADC resolution check"
* For more information about using Git and our Commit Conventions see
  https://github.com/RIOT-OS/RIOT/blob/master/CONTRIBUTING.md#commit-conventions

### Continuous Integration
* If the CI tests fail due to errors these errors need to be addressed.
* If the CI tests fail due to warnings/errors emitted by cppcheck you SHOULD try
  to fix the error. If the error is definitely a false positive there is the
  possibility to suppress this warning/error. You MUST do so by adding a
  comment, including a rationale why it is a false positive and why the code
  can't be fixed otherwise, in the following format:
```c
/* cppcheck-suppress <category of error/warning>
 * (reason: cppcheck is being really silly. this is certainly not a
 * null-pointer dereference */
```
