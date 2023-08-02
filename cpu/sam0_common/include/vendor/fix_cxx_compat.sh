#!/bin/sh

# This script removes type qualifiers from anonymous padding fields in bit
# fields for compatibility with clang++.

offenders="$(grep -Erl '__I [u]*int[0-9]*_t[ \t]*:[0-9]*;')"

for file in $offenders; do
    sed -i "$file" -e 's/__I \([u]*int[0-9]*_t[\t ]*:[0-9]*;\)/\1    /g'
done
