#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

# Script to save all the dependencies resolution variables for both normal and
# in `info-boards-supported` resolution.
#
# The script takes a long time
#
# @author  Gaëtan Harter <gaetan.harter@fu-berlin.de>

: "${RIOTBASE:="$(cd "$(dirname "$0")/../../../" || exit; pwd)"}"

usage() {
    echo "Usage: $0 <output_directory>"
}


applications() {
    make --no-print-directory -C "${RIOTBASE}" info-applications
}


boards() {
    make --no-print-directory -C "${RIOTBASE}" info-boards
}


function info_deps_variables_application() {
    local application="$1"
    local output_dir="$2/${application}"

    DEPENDENCY_DEBUG=1 DEPENDENCY_DEBUG_OUTPUT_DIR="${output_dir}/info-global" make --no-print-directory -C "${RIOTBASE}/${application}" info-boards-supported >/dev/null
    for board in $(boards)
    do
        DEPENDENCY_DEBUG_OUTPUT_DIR="${output_dir}/info" BOARD="${board}" make --no-print-directory -C "${RIOTBASE}/${application}" dependency-debug >/dev/null
    done
}


main() {
    if [[ $# -ne 1 ]]; then
        usage
        exit 1
    fi
    local output_dir

    # No 'realpath' in mac by default…
    output_dir=$(python3 -c 'import os.path; print(os.path.abspath("'"$1"'"))')

    for app in $(applications)
    do
        echo "Evaluating ${app}" >&2
        info_deps_variables_application "${app}" "${output_dir}"
    done

    # Generate the aggregated files to simplify comparing both parsing
    find "${output_dir}" -type f -name 'dependencies_info_*' | sort | xargs cat > "${output_dir}"/deps_info
    find "${output_dir}" -type f -name 'dependencies_info-boards-supported_*' | sort | xargs cat > "${output_dir}"/deps_info-boards-supported
}


if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
