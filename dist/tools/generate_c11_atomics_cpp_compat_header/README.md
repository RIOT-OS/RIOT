# Usage

./generate_c11_atomics_cpp_compat_header.sh [CC [CFLAGS]]

## Parameter CC

The C compiler to use to determine the size of the C11 atomics types. If not
given, the environment variable CC is used instead.

## Parameter CFLAGS

The CFLAGS to pass to the C compiler during determination of the size of the
C11 atomic types. If not given, the environment variable CFLAGS is used instead.

# Output

The script will write the sizes of each type defined in C11 atomics and which
`uint<NUM>_t` type has the same size as the atomic type. Note that C11 allows
to prefix any type with the `_Atomic` specifier to get an atomic version of it.
This script will only generate entries for the default `typedef`ed types like
`atomic_bool`, `atomic_int`, or `atomic_int_least16_t`.

The output will look like this:

``` C++
/* This file was automatically generated using ./dist/tools/generate_c11_atomics_cpp_compat_header/generate_c11_atomics_cpp_compat_header.sh /*
#pragma once

#define ATOMIC_BOOL_SIZE                                (1U)
#define ATOMIC_BOOL_SAME_SIZED_TYPE                     uint8_t
#define ATOMIC_CHAR_SIZE                                (1U)
#define ATOMIC_CHAR_SAME_SIZED_TYPE                     uint8_t
#define ATOMIC_SCHAR_SIZE                               (1U)
#define ATOMIC_SCHAR_SAME_SIZED_TYPE                    uint8_t
#define ATOMIC_UCHAR_SIZE                               (1U)
#define ATOMIC_UCHAR_SAME_SIZED_TYPE                    uint8_t
#define ATOMIC_SHORT_SIZE                               (2U)
#define ATOMIC_SHORT_SAME_SIZED_TYPE                    uint16_t
#define ATOMIC_USHORT_SIZE                              (2U)
#define ATOMIC_USHORT_SAME_SIZED_TYPE                   uint16_t
#define ATOMIC_INT_SIZE                                 (4U)
#define ATOMIC_INT_SAME_SIZED_TYPE                      uint32_t
...
```
