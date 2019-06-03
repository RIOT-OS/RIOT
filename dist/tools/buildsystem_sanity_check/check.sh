#!/usr/bin/env bash
#
# Copyright (C) 2018 Gaëtan Harter <gaetan.harter@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

#
# Central test script to have sanity checks for the build system
# It is run unconditionally on all files.
#
#

: "${RIOTBASE:="$(cd "$(dirname "$0")/../../../" || exit; pwd)"}"

SCRIPT_PATH=dist/tools/buildsystem_sanity_check/check.sh

# Modules should not check the content of FEATURES_PROVIDED/_REQUIRED/OPTIONAL
# Handling specific behaviors/dependencies should by checking the content of:
# * `USEMODULE`
# * maybe `FEATURES_USED` if it is not a module (== not a periph_)
check_not_parsing_features() {
    local patterns=()
    local pathspec=()

    patterns+=(-e 'if.*filter.*FEATURES_PROVIDED')
    patterns+=(-e 'if.*filter.*FEATURES_REQUIRED')
    patterns+=(-e 'if.*filter.*FEATURES_OPTIONAL')

    # Pathspec with exclude should start by an inclusive pathspec in git 2.7.4
    pathspec+=('*')

    # Ignore this file when matching as it self matches
    pathspec+=(":!${SCRIPT_PATH}")

    # These two files contain sanity checks using FEATURES_ so are allowed
    pathspec+=(':!Makefile.include' ':!makefiles/info-global.inc.mk')

    git -C "${RIOTBASE}" grep "${patterns[@]}" -- "${pathspec[@]}"
}

# Some variables do not need to be exported and even cause issues when being
# exported because they are evaluated even when not needed.
#
# Currently this blacklists exported variables instead of whitelisting or
# providing a mechanism for handling it.
# It just keep things not exported anymore in the future.
UNEXPORTED_VARIABLES=()
UNEXPORTED_VARIABLES+=('FLASHFILE')
UNEXPORTED_VARIABLES+=('TERMPROG' 'TERMFLAGS')
UNEXPORTED_VARIABLES+=('FLASHER' 'FFLAGS')
UNEXPORTED_VARIABLES+=('RESET' 'RESETFLAGS')
UNEXPORTED_VARIABLES+=('DEBUGGER' 'DEBUGGER_FLAGS')
UNEXPORTED_VARIABLES+=('DEBUGSERVER' 'DEBUGSERVER_FLAGS')
UNEXPORTED_VARIABLES+=('PREFLASHER' 'PREFFLAGS' 'FLASHDEPS')
UNEXPORTED_VARIABLES+=('DEBUG_ADAPTER' 'DEBUG_ADAPTER_ID')
UNEXPORTED_VARIABLES+=('PROGRAMMER_SERIAL')
UNEXPORTED_VARIABLES+=('STLINK_VERSION')

EXPORTED_VARIABLES_ONLY_IN_VARS=()
check_not_exporting_variables() {
    local patterns=()
    local pathspec=()

    for variable in "${UNEXPORTED_VARIABLES[@]}"; do
        patterns+=(-e "export[[:blank:]]\+${variable}")
    done

    git -C "${RIOTBASE}" grep "${patterns[@]}"

    # Some variables may still be exported in 'makefiles/vars.inc.mk' as the
    # only place that should export commont variables
    pathspec+=('*')
    pathspec+=(':!makefiles/vars.inc.mk')

    patterns=()
    for variable in "${EXPORTED_VARIABLES_ONLY_IN_VARS[@]}"; do
        patterns+=(-e "export[[:blank:]]\+${variable}")
    done

    # Only run if there are patterns, otherwise it matches everything
    if [ ${#patterns[@]} -ne 0 ]; then
        git -C "${RIOTBASE}" grep "${patterns[@]}" -- "${pathspec[@]}"
    fi
}


main() {
    local errors=''

    errors+="$(check_not_parsing_features)"
    errors+="$(check_not_exporting_variables)"

    if [ -n "${errors}" ]
    then
        printf 'Invalid build system patterns found by %s:\n' "${0}"
        printf '%s\n' "${errors}"
        exit 1
    fi
    exit 0
}


if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main
fi
