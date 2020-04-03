#!/bin/sh

TESTFILE=/tmp/riot_are_types_of_same_size_compilation_check.c

ATOMIC_TYPES="
    atomic_bool
    atomic_char
    atomic_schar
    atomic_uchar
    atomic_short
    atomic_ushort
    atomic_int
    atomic_uint
    atomic_long
    atomic_ulong
    atomic_llong
    atomic_ullong
    atomic_int_least8_t
    atomic_uint_least8_t
    atomic_int_least16_t
    atomic_uint_least16_t
    atomic_int_least32_t
    atomic_uint_least32_t
    atomic_int_least64_t
    atomic_uint_least64_t
    atomic_int_fast8_t
    atomic_uint_fast8_t
    atomic_int_fast16_t
    atomic_uint_fast16_t
    atomic_int_fast32_t
    atomic_uint_fast32_t
    atomic_int_fast64_t
    atomic_uint_fast64_t
    atomic_intptr_t
    atomic_uintptr_t
    atomic_size_t
    atomic_ptrdiff_t
    atomic_intmax_t
    atomic_uintmax_t
    "

are_types_of_same_size() {
    cat > $TESTFILE << EOF
#include <stdint.h>
#ifdef __msp430__
#include <sys/cdefs.h>
#endif
#include <stdatomic.h>
int main(void) {
    _Static_assert(sizeof($1) == sizeof($2), "Assert types are equally sized");
    return 0;
}
EOF
    $CC $CFLAGS -o /dev/null -c $TESTFILE 2> /dev/null > /dev/null
}

get_size() {
    if $(are_types_of_same_size $1 uint8_t); then
        echo "1"
        return
    fi

    if $(are_types_of_same_size $1 uint16_t); then
        echo "2"
        return
    fi

    if $(are_types_of_same_size $1 uint32_t); then
        echo "4"
        return
    fi

    if $(are_types_of_same_size $1 uint64_t); then
        echo "8"
        return
    fi

    echo "Failed to determine size of $1"
    exit 1
}

get_same_sized_type() {
    if $(are_types_of_same_size $1 uint8_t); then
        echo "uint8_t"
        return
    fi

    if $(are_types_of_same_size $1 uint16_t); then
        echo "uint16_t"
        return
    fi

    if $(are_types_of_same_size $1 uint32_t); then
        echo "uint32_t"
        return
    fi

    if $(are_types_of_same_size $1 uint64_t); then
        echo "uint64_t"
        return
    fi

    echo "Failed to determine same sized type for $1"
    exit 1
}

print_defines() {
    printf "#define %-48s(%sU)\n" "$(echo $1 | tr [a-z] [A-Z])_SIZE" "$(get_size $1)"
    printf "#define %-48s%s\n" "$(echo $1 | tr [a-z] [A-Z])_SAME_SIZED_TYPE" "$(get_same_sized_type $1)"
}

if [ $# -ge 1 ]; then
    if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
        echo "Usage: $0 [CC [CFLAGS]]"
        echo "See README.md for more details"
        exit 1
    fi
    CC="$1"
fi

if [ $# -ge 2 ]; then
    CFLAGS="$2"
fi

if [ -z "$CC" ]; then
    echo "C compiler needs to be specified either as environment variable, or"
    echo "as first argument. See README.md for documentation."
    exit 1
fi

echo "/* This file was automatically generated using $0 */"
echo "#pragma once"
echo

for atomic_type in $ATOMIC_TYPES; do
    print_defines $atomic_type
done
