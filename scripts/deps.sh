#!/bin/sh

###############################################################################
# USER INTERFACE FUNCTIONS:
###############################################################################

# add the arguments to the list of the used modules
use() {
    DEPS="$DEPS$* "
}

# true iff the argument is a used module
used() {
    [ "${DEPS#* $1 }" != "$DEPS" ];
}

# true iff the argument is the prefix of a used module name
used_prefix() {
    [ "${DEPS#* $1}" != "$DEPS" ];
}

# true iff the first argument is a used prefix,
# but ignore the following argumens as possible suffices
used_prefix_but() {
    local prefix=$1
    shift
    echo "$DEPS" | grep -qP " $prefix(?!$(join "|" $*)) "
}

# negate a result, i.e. "not used foo" is true iff "foo" is not used
not() {
    if "$@"; then
        false
    else
        true
    fi
}

# print a debug message
msg() {
    >&2 echo $*
}

###############################################################################
# INTERNALS:
###############################################################################

set -e

[ "$TRACE_DEPS" = yes ] && set -e -x

DEP_FILE="$1"
shift

DEPS=" $* "

join() {
    local IFS="$1"
    shift
    echo "$*"
}

sort_unique() {
    echo $* | tr ' ' '\n' | sort -u | tr '\n' ' '
}

while true; do
    DEPS_BEFORE=$DEPS

    . "$DEP_FILE"

    DEPS=" $(sort_unique $DEPS) "
    if [ "$DEPS" = "$DEPS_BEFORE" ]; then
        echo $DEPS
        exit 0
    fi
done
