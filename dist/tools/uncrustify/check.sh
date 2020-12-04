#!/bin/sh
CURDIR="$(cd "$(dirname "$0")" && pwd)"

${CURDIR}/uncrustify.sh --check
