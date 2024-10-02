#!/bin/sh

set -eu

FAILURES=""

# This is a Make based environment, therefore we don't have funny names to take
# care of, and anyone who can cause commands to be run by injecting files can
# just as well modify the scripts being run. (Otherwise we'd need to go through
# a less readable print0 / read).
for CARGOTOML in `find -name Cargo.toml`
do
    if cargo fmt --quiet --check --manifest-path "${CARGOTOML}"; then
        continue
    else
        FAILURES="${FAILURES} ${CARGOTOML%Cargo.toml}"
    fi
done

if [ x"" != x"${FAILURES}" ]; then
    echo "Some Rust files are following rustfmt, in particular in:"
    echo "${FAILURES}"
    echo "You can format the code locally using:"
    echo
    echo "find -name Cargo.toml -exec cargo fmt --manifest-path '{}' ';'"
    if [ ! -z "${GITHUB_RUN_ID:-}" ]; then
        echo
        echo "The author of this test regrets not knowing how to provide an easy way to just click a button here that provides the right fixup commits."
    fi
    exit 1
fi
