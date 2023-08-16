#!/usr/bin/env bash
#
# usage: has_minimal_version.sh <version> <minimal_version> [toolname]
#   Checks that version >= minimal_version
#   Version format MAJOR.MINOR.PATCH ex 3.1.4

# Adapted from https://stackoverflow.com/a/24067243 to have >=
# Checking versions with '-rcX' does not work properly due to the usage of
# `sort -V`.

if [ $# -lt 2 ]; then
    echo "usage: ${0} <version> <minimal_version> [toolname]" >&2
    echo "  Checks that version >= minimal_version" >&2
    echo "  Version format MAJOR.MINOR.PATCH ex 3.1.4" >&2
    exit 1
fi

readonly TOOLVERSION="${1}"
readonly MINVERSION="${2}"
readonly TOOLNAME="${3}"

if [ "${TOOLNAME}" != "" ]; then
  readonly TOOLSTR="${TOOLNAME} "
else
  readonly TOOLSTR=""
fi


HIGHEST=$(printf "%s\n%s\n" "${TOOLVERSION}" "${MINVERSION}" | sort -rV | head -n 1)
test "${HIGHEST}" = "${TOOLVERSION}" && exit 0

printf "\nInvalid version, found %s%s, minimal required is %s\n\n" "${TOOLSTR}" "${TOOLVERSION}" "${MINVERSION}" >&2
exit 1
