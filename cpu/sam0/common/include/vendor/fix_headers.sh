#!/bin/sh

# This function removes type qualifiers from anonymous padding fields in bit
# fields for compatibility with clang++.
strip_type_qualifiers_from_bit_fields() {
    echo "Searching for header files with bogus type qualifiers"
    offenders="$(grep -Erl '__I [u]*int[0-9]*_t[ \t]*:[0-9]*;')"

    for file in $offenders; do
        echo "Sanitizing $file for type qualifiers in padding"
        sed -i "$file" -e 's/__I \([u]*int[0-9]*_t[\t ]*:[0-9]*;\)/\1    /g'
    done
}

# This functions removes a bogus `LITTLE_ENDIAN` define which conflicts with
# <endian.h> and tinyDTLS notion of it. Luckily, that define is not used
# anywhere in the vendor header files, so we can just drop it.
remove_bogus_endian_define() {
    echo "Searching for header files with bogus LITTLE_ENDIAN define"
    offenders="$(grep -rl '^#define LITTLE_ENDIAN')"

    for file in $offenders; do
        echo "Removing bogus LITTLE_ENDIAN define from $file"
        sed -i "$file" -e '/^#define LITTLE_ENDIAN/d'
    done
}

strip_type_qualifiers_from_bit_fields
remove_bogus_endian_define
