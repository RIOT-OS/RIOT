# Debugging Tools {#debugging-tools}

## Coverage

When writing new test cases or updating existing ones it can be useful
to estimate the achieved coverage.

### Overview

The `sys/coverage` module can be used to collect coverage information
directly on the microcontroller. When the `main` thread terminates,
this information is dumped on the UART and can be extracted using a
Python script. The tooling for doing so has been initially imported from
the [Zephyr][zephyr coverage] operating system.

### How to use

1. Add `USEMODULE += coverage` to your application `Makefile`.

2. Flash the application to your board.

3. Ideally wait a few seconds (to ensure we don't start reading from the
   UART in the middle of a coverage dump) and run `make coverage` and
   reset the board. This will generate Gcov `.gcda` files automatically.
   This may take a few seconds, `make coverage` will terminate when it is done.

4. Usage a Gcov frontend to generate a visualization (e.g. HTML) from the `.gcda` files.
   For example, [gcovr][gcovr web]:
   `gcovr -r . --gcov-executable ${TRIPLET}-gcov --html -o coverage.html --html-details`

## Undefined Behavior Sanitizer (ubsan) {#ubsan}

RIOT contains makefile convenience support for gcc/clang's undefined
behaviour sanitizer.

### Overview

Both gcc and clang allow generation on code that does runtime checks for
undefined behavior (UB).

E.g., the following code might trigger UB for some parameters:

```C
void test(int foo) {
    return (foo << 24);
}
```

In this case, the signed shift would be alright unless:

- it would "push out" all bits to the left, with undefined runtime result. Here,
  that happens on architectures with 16-bit integers.
- `foo` is negative, with implementation defined runtime results.

Using ubsan, these can be caught at runtime.

There are three modes for ubsan that define what happens when the sanitizer
observed undefined behaviour:

1. `trap` -> cause a trap
2. `msg_exit` -> print a message and exit
3. `msg_recover` -> print a message and continue

`trap` is available on all RIOT platforms, whereas `msg_exit` and `msg_recover`
are currently only available on `native` when building with gcc, as they require runtime support in
the form of `libubsan`.

The default is `trap`, or `msg_exit` if available (currently, on native:gnu only).


### How to use

1. build with `make all-ubsan`.

2. build with `UBSAN_MODE=[trap|msg_exit|msg_recover] make all-ubsan` to
   override the ubsan mode.

[zephyr coverage]: https://docs.zephyrproject.org/latest/develop/test/coverage.html
[gcovr web]: https://gcovr.com/en/stable/
