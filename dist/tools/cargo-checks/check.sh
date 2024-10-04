#!/bin/bash

set -eu

FAILURES=""

while IFS= read -r -d '' CARGOTOML
do
    if cargo fmt --quiet --check --manifest-path "${CARGOTOML}"; then
        continue
    else
        FAILURES="${FAILURES} ${CARGOTOML%Cargo.toml}"
    fi
done < <(find . -name Cargo.toml -print0)

if [ x"" != x"${FAILURES}" ]; then
    echo "Some Rust files are following rustfmt, in particular in:"
    echo "${FAILURES}"
    echo "You can format the code locally using:"
    echo
    echo "find -name Cargo.toml -exec cargo fmt --manifest-path '{}' ';'"
    if [ -n "${GITHUB_RUN_ID:-}" ]; then
        echo
        echo "The author of this test regrets not knowing how to provide an easy way to just click a button here that provides the right fixup commits."
    fi
    exit 1
fi
