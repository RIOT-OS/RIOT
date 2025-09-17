---
title: Debugging Tools
description: Overview of debugging tools supported in RIOT-OS
---

## Undefined Behavior Sanitizer (ubsan)

RIOT contains Makefile convenience support for gcc/clang's undefined
behaviour sanitizer.

### Overview

Both gcc and clang allow generation of code that does runtime checks for
undefined behavior (UB).

E.g., the following code might trigger UB for some parameters:

```c
void test(int foo) {
    return (foo << 24);
}
```

In this case, the signed shift would be alright unless:

- it would "push out" all bits to the left, with undefined runtime result. Here,
  that happens on architectures with 16-bit integers.
- `foo` is negative, with implementation-specific runtime results.

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


### How to Use

1. build with `make all-ubsan`.

2. build with `UBSAN_MODE=[trap|msg_exit|msg_recover] make all-ubsan` to
   override the ubsan mode.
