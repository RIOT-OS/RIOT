#!/bin/sh

set -e

[ "$TRACE_DEPS" = yes ] && set -e -x

DEP_FILE="$1"
shift

DEPS="$*"

use() {
    DEPS="$DEPS $*"
}

used() {
    DEPS=" $DEPS"
    [ "${DEPS#* $1 }" != "$DEPS" ];
}

used_filter() {
    local pattern=$1
    shift
    local _tmp_DEPS="$DEPS"

    for dep in $*; do
        _tmp_DEPS="$(echo "$_tmp_DEPS" | tr ' ' '\n' | grep -Ev "^${dep}$" | tr '\n' ' ')"
    done

    echo $_tmp_DEPS | tr ' ' '\n' | grep -Eq "^${pattern}$"
}

not_used() {
    if used $*; then
        false
    else
        true
    fi
}

include() {
    [ "$1" = "maybe" ] && {
        shift
        [ -f "$1" ] || return 1
    }

    . "$1"
}

sort_unique() {
    echo $* | tr ' ' '\n' | sort -u | tr '\n' ' '
}

msg() {
    >&2 echo $*
}

while true; do
    DEPS_BEFORE=$DEPS

    . "$DEP_FILE"

    DEPS="$(sort_unique $DEPS)"
    if [ "$DEPS" = "$DEPS_BEFORE" ]; then
        echo "$DEPS"
        exit 0
    fi
done
