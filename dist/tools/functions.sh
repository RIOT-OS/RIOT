#!/bin/sh

quote()
{
    while [ $# -ne 0 ]; do
        printf '%q\n' "$1"
        shift
    done
}

# run arguments as command, only print output when command errors
quiet() {
    OUT="$($(quote "$@") 2>&1)"
    RES=$?
    [ -n "$OUT" ] && [ $RES -ne 0 -o "$QUIET" = "0" ] && echo "$OUT"
    return $RES
}
