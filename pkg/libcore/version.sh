#!/bin/sh

RUST_VERSION=""
if ! command -v rustc >/dev/null 2>&1; then
	echo "Missing rustc, please install it" 2>&1
	exit 1
fi

RUST_VERSION="$(rustc --version | cut -d ' ' -f2-)"
if [ "$(echo ${RUST_VERSION} | cut -d '-' -f2 \
		| cut -d ' ' -f1)" = "nightly" ]; then
	RUST_VERSION="$(echo ${RUST_VERSION} | cut -d ' ' -f2 | tr -d '()')"
fi

echo "${RUST_VERSION}"
