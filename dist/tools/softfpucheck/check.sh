#!/usr/bin/env sh

# Copyright 2023 Otto-von-Guericke-Universität Magdeburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

SCRIPTDIR="$(cd "$(dirname "$0")" || return 1; pwd)"

print_in_red() {
    printf "\e[1;31m%s\e[0m\n" "$1"
}

print_usage() {
    echo "Usage: $0 [OPTIONS] <MAP_FILE>"
    echo "Options:"
    echo "  --help: print this help"
    echo "  --fail: fail if use of soft FPU is detected (default: warn only)"
}

fail_on_soft_fpu_use=""

while [ "$#" -gt 0 ]; do
    case "$1" in
    --help)
        print_usage
        return 0
        ;;
    --fail)
        fail_on_soft_fpu_use="y"
        ;;
    *)
        mapfile="$1"
        ;;
    esac
    shift
done

if [ -z "$mapfile" ]; then
    print_usage
    return 1
fi

if offenders="$(grep -Eo -f "${SCRIPTDIR}/soft_fpu_syms.txt" "$mapfile" 2>/dev/null)"; then
    print_in_red "WARNING: Software FPU is in use"
    echo "Use of floats without FPU greatly increases ROM size and CPU cycles required."
    echo "Please consider rewriting your code to only use integer arithmetic."
    echo "Used soft FPU symbols:"
    echo "${offenders}"

    echo "Hint: Providing a GOOD_REASON_FOR_USING_FLOATS will disable this check"
    if [ -n "$fail_on_soft_fpu_use" ]; then
        return 2
    fi
fi

return 0
