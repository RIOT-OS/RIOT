#!/bin/sh

quote()
{
    while [ $# -ne 0 ]; do
        printf "'"
        printf '%s' "$1" | sed "s/'/'\\\\''/g"
        printf "'\n"
        shift
    done
}

# run arguments as command, only print output when command errors
quiet() {
    OUT="$(eval $(quote "$@") 2>&1)"
    RES=$?
    [ -n "$OUT" ] && [ $RES -ne 0 -o "$QUIET" = "0" ] && echo "$OUT"
    return $RES
}
