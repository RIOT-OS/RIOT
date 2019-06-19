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


tab_indent() {
    # Ident using 'bashism' to to the tab compatible with 'bsd-sed'
    sed 's/^/\'$'\t/'
}

prepend() {
    # 'i' needs 'i\{newline}' and a newline after for 'bsd-sed'
    sed '1i\
'"$1"'
'
}

error_with_message() {
    tab_indent | prepend "${1}"
}


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
    patterns+=(-e 'if.*filter.*FEATURES_USED') # Not a real one, it is just to show the error

    # Pathspec with exclude should start by an inclusive pathspec in git 2.7.4
    pathspec+=('*')

    # Ignore this file when matching as it self matches
    pathspec+=(":!${SCRIPT_PATH}")

    # These two files contain sanity checks using FEATURES_ so are allowed
    pathspec+=(':!Makefile.include' ':!makefiles/info-global.inc.mk')

    git -C "${RIOTBASE}" grep "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message 'Modules should not check the content of FEATURES_PROVIDED/_REQUIRED/OPTIONAL'
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
UNEXPORTED_VARIABLES+=('PORT_LINUX' 'PORT_DARWIN')
UNEXPORTED_VARIABLES+=('APPDEPS')


EXPORTED_VARIABLES_ONLY_IN_VARS=()
EXPORTED_VARIABLES_ONLY_IN_VARS+=('CC')
check_not_exporting_variables() {
    local patterns=()
    local pathspec=()

    for variable in "${UNEXPORTED_VARIABLES[@]}"; do
        patterns+=(-e "export[[:blank:]]\+${variable}")
    done

    git -C "${RIOTBASE}" grep "${patterns[@]}" \
        | error_with_message 'Variables must not be exported:'

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
        git -C "${RIOTBASE}" grep "${patterns[@]}" -- "${pathspec[@]}" \
            | error_with_message 'Variables must only be exported in `makefiles/vars.inc.mk`:'
    fi
}


error_on_input() {
    grep '' && return 1
}

all_checks() {
    check_not_parsing_features
    check_not_exporting_variables
}

main() {
    all_checks | prepend 'Invalid build system patterns found by '"${0}:"  || error_on_input >&2
    exit $?
}


if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main
fi
