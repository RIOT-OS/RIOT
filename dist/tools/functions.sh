#!/bin/sh

# run arguments as command, only print output when command errors
quiet() {
    OUT="$("$@" 2>&1)"
    RES=$?
    [ -n "$OUT" ] && [ $RES -ne 0 -o "$QUIET" = "0" ] && echo "$OUT"
    return $RES
}
