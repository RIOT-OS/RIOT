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

export ENABLE_DOCKER_PINNING_TEST=${ENABLE_DOCKER_PINNING_TEST:-1}

: "${RIOTBASE:="$(cd "$(dirname "$0")/../../../" || exit; pwd)"}"

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"
: "${RIOTMAKE:=${RIOTBASE}/makefiles}"
# not running shellcheck with -x in the CI --> disable SC1091
# shellcheck disable=SC1091
. "${RIOTTOOLS}"/ci/github_annotate.sh

SCRIPT_PATH=dist/tools/buildsystem_sanity_check/check.sh

error_with_message() {
    while read -r INPUT; do
        MESSAGE="${1}"
        if github_annotate_is_on; then
            FILE=$(echo "${INPUT}" | cut -d: -f1)
            LINE=$(echo "${INPUT}" | cut -d: -f2)
            MATCH=$(echo "${INPUT}" | cut -d: -f3)
            if [[ ! $LINE =~ ^[0-9]+$ ]] || [  -z "$MATCH" ]; then
                # line is not provided in grep pattern
                LINE=0
            fi
            github_annotate_error "$FILE" "$LINE" "$MESSAGE"
        fi

        printf "%s:\n\t%s\n\n" "${MESSAGE}" "${INPUT}"
    done
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

    # Pathspec with exclude should start by an inclusive pathspec in git 2.7.4
    pathspec+=('*')

    # Ignore this file when matching as it self matches
    pathspec+=(":!${SCRIPT_PATH}")

    # These two files contain sanity checks using FEATURES_ so are allowed
    pathspec+=(':!Makefile.include' ':!makefiles/info-global.inc.mk')

    # We extend FEATURES_PROVIDED in Makefile.features based on what is
    # already provided to avoid clutter in each boards Makefile.features.
    # E.g. `periph_eth` will pull in `netif_ethernet`, which
    # will pull in `netif`.
    pathspec+=(':!Makefile.features')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message 'Modules should not check the content of FEATURES_PROVIDED/REQUIRED/OPTIONAL'
}

# Providing features for boards and CPUs should only be done in
# Makefile.features
check_providing_features_only_makefile_features() {
    local patterns=()
    local pathspec=()

    patterns+=(-e '^[ ]*FEATURES_PROVIDED *+= *')

    pathspec+=("*Makefile\.*")

    pathspec+=(":!*Makefile.features")

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message 'Features should only be provided in Makefile.features files'
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
UNEXPORTED_VARIABLES+=('DEBUGCLIENT' 'DEBUGCLIENT_FLAGS')
UNEXPORTED_VARIABLES+=('PREFLASHER' 'PREFFLAGS' 'FLASHDEPS')
UNEXPORTED_VARIABLES+=('OPENOCD_DEBUG_ADAPTER' 'DEBUG_ADAPTER_ID')
UNEXPORTED_VARIABLES+=('PROGRAMMER_SERIAL')
UNEXPORTED_VARIABLES+=('STLINK_VERSION')
UNEXPORTED_VARIABLES+=('PORT_LINUX' 'PORT_DARWIN')
UNEXPORTED_VARIABLES+=('PORT[ ?=:]' 'PORT$')
UNEXPORTED_VARIABLES+=('LINKFLAGS' 'LINKER_SCRIPT')
UNEXPORTED_VARIABLES+=('USEMODULE_INCLUDES')
UNEXPORTED_VARIABLES+=('OPENOCD_ADAPTER_INIT')
UNEXPORTED_VARIABLES+=('OPENOCD_CONFIG')
UNEXPORTED_VARIABLES+=('OPENOCD_RESET_USE_CONNECT_ASSERT_SRST')
UNEXPORTED_VARIABLES+=('OPENOCD_CMD_RESET_RUN')
UNEXPORTED_VARIABLES+=('OPENOCD_PRE_FLASH_CMDS' 'OPENOCD_PRE_VERIFY_CMDS')
UNEXPORTED_VARIABLES+=('OPENOCD_PRE_FLASH_CHECK_SCRIPT')
UNEXPORTED_VARIABLES+=('PYOCD_FLASH_TARGET_TYPE')
UNEXPORTED_VARIABLES+=('PYOCD_ADAPTER_INIT')
UNEXPORTED_VARIABLES+=('JLINK_DEVICE' 'JLINK_IF')
UNEXPORTED_VARIABLES+=('JLINK_PRE_FLASH' 'JLINK_POST_FLASH' 'JLINK_RESET_FILE')
UNEXPORTED_VARIABLES+=('GIT_CACHE' 'GIT_CACHE_DIR')
UNEXPORTED_VARIABLES+=('LINKXX')
UNEXPORTED_VARIABLES+=('APPDEPS' 'BUILDDEPS' 'DEBUGDEPS')
UNEXPORTED_VARIABLES+=('EMULATOR' 'EMULATOR_FLAGS')

EXPORTED_VARIABLES_ONLY_IN_VARS=()
EXPORTED_VARIABLES_ONLY_IN_VARS+=('CPU_ARCH')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('CPU_CORE')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('CPU_FAM')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('NATIVEINCLUDES')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('UNDEF')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('USEMODULE')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('TARGET_ARCH')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('TOOLCHAIN')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('WERROR')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('WPEDANTIC')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('CC[ =]' 'CXX' 'CCAS')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('AR[ =]' 'RANLIB')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('AS' 'NM' 'SIZE' 'LINK')
EXPORTED_VARIABLES_ONLY_IN_VARS+=('OBJDUMP' 'OBJCOPY')

check_not_exporting_variables() {
    local patterns=()
    local pathspec=()

    for variable in "${UNEXPORTED_VARIABLES[@]}"; do
        patterns+=(-e "export[[:blank:]]\+${variable}")
    done

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" \
        | error_with_message 'Variables must not be exported'

    # Some variables may still be exported in 'makefiles/vars.inc.mk' as the
    # only place that should export common variables
    pathspec+=('*')
    pathspec+=(':!makefiles/vars.inc.mk')
    pathspec+=(':!**/Vagrantfile')

    patterns=()
    for variable in "${EXPORTED_VARIABLES_ONLY_IN_VARS[@]}"; do
        patterns+=(-e "export[[:blank:]]\+${variable}")
    done

    # Only run if there are patterns, otherwise it matches everything
    if [ ${#patterns[@]} -ne 0 ]; then
        git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
            | error_with_message "Variables must only be exported in \`makefiles/vars.inc.mk\`"
    fi
}

# Deprecated variables or patterns
# Prevent deprecated variables or patterns to re-appear after cleanup
check_deprecated_vars_patterns() {
    local patterns=()
    local pathspec=()

    patterns+=(-e 'FEATURES_MCU_GROUP')
    patterns+=(-e 'TEST_ON_CI_WHITELIST += all')

    # Pathspec with exclude should start by an inclusive pathspec in git 2.7.4
    pathspec+=('*')

    # Ignore this file when matching as it self matches
    pathspec+=(":!${SCRIPT_PATH}")

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message 'Deprecated variables or patterns'
}

# Makefile files cpu must not be included by the board anymore
# They are included by the main Makefile.include/Makefile.features/Makefile.dep
check_board_do_not_include_cpu_features_dep() {
    local patterns=()
    local pathspec=()

    patterns+=(-e "include \$(RIOTCPU)/.*/Makefile\..*")

    pathspec+=('boards/')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
            | error_with_message 'Makefiles files from cpu must not be included by the board anymore'
}

# CPU and CPU_MODEL definition have been moved to 'BOARD|CPU/Makefile.features'
check_cpu_cpu_model_defined_in_makefile_features() {
    local patterns=()
    local pathspec=()

    # With our without space and with or without ?=
    patterns+=(-e '^ *\(export\)\? *CPU \??\?=')
    patterns+=(-e '^ *\(export\)\? *CPU_MODEL \??\?=')
    pathspec+=(':!**.md')
    pathspec+=(':!**.mdx')
    pathspec+=(':!boards/**/Makefile.features')
    pathspec+=(':!cpu/**/Makefile.features')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
            | error_with_message 'CPU and CPU_MODEL definition must be done by board/BOARD/Makefile.features, board/common/**/Makefile.features or cpu/CPU/Makefile.features'
}

# Applications Makefile must not set 'BOARD =' unconditionally
check_not_setting_board_equal() {
    local patterns=()
    local pathspec=()

    patterns+=(-e '^[[:space:]]*BOARD[[:space:]]*=')

    pathspec+=('**/Makefile')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message 'Applications Makefile should use "BOARD ?="'
}

# Examples must not provide BOARD_INSUFFICIENT_MEMORY in Makefile, but in
# Makefile.ci
check_board_insufficient_memory_not_in_makefile() {
    local patterns=()
    local pathspec=()

    patterns+=(-e '^[[:space:]]*BOARD_INSUFFICIENT_MEMORY[[:space:]:+]*=')

    pathspec+=('**/Makefile')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message 'Move BOARD_INSUFFICIENT_MEMORY to Makefile.ci'
}

# Test applications must not define the APPLICATION variable
checks_tests_application_not_defined_in_makefile() {
    local patterns=()
    local pathspec=()

    patterns+=(-e '^[[:space:]]*APPLICATION[[:space:]:+]=')

    pathspec+=('tests/**/Makefile')
    pathspec+=(':!tests/build_system/external_board_native/Makefile')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message "Don't define APPLICATION in test Makefile"
}

# Develhelp should not be set via CFLAGS
checks_develhelp_not_defined_via_cflags() {
    local patterns=()
    local pathspec=()

    patterns+=(-e '^[[:space:]]*CFLAGS[[:space:]:+]+=[[:space:]:+]-DDEVELHELP')

    pathspec+=('**/Makefile')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message "Use DEVELHELP ?= 1 instead of using CFLAGS directly"
}


# Common code in boards should not use $(BOARD) to reference files
check_files_in_boards_not_reference_board_var() {
    local patterns=()
    local pathspec=()

    patterns+=(-e "/\$(BOARD)/")

    pathspec+=('boards/')
    # boards/common/nrf52 uses a hack to resolve dependencies early
    pathspec+=(':!boards/common/nrf52/Makefile.include')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message "Code in boards/ should not use \$(BOARDS) to reference files since this breaks external BOARDS changing BOARDSDIR"
}

check_no_pseudomodules_in_makefile_dep() {
    local patterns=()
    local pathspec=()

    patterns+=(-e 'PSEUDOMODULES[\t ]*[+:]*=')

    pathspec+=('**/Makefile.dep')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message "Don't define PSEUDOMODULES in Makefile.dep"
}

check_no_usemodules_in_makefile_include() {
    local patterns=()
    local pathspec=()

    patterns+=(-e 'USEMODULE[\t ]*[+:]*=')

    pathspec+=('**/Makefile.include')
    pathspec+=(':!Makefile.include')
    pathspec+=(':!tests/**/Makefile.include')
    pathspec+=(':!examples/**/Makefile.include')

    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message "Don't include USEMODULE in Makefile.include"
}

check_no_pkg_source_local() {
    local patterns=()
    local pathspec=()

    patterns+=(-e 'PKG_SOURCE_LOCAL')

    pathspec+=('pkg/*/Makefile')
    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message "Don't push PKG_SOURCE_LOCAL definitions upstream"
}

check_no_riot_config() {
    local patterns=()
    local pathspec=()

    patterns+=(-e 'RIOT_CONFIG_.*')

    pathspec+=('Makefile*')
    pathspec+=('**/Makefile*')
    pathspec+=('**/*.mk')
    pathspec+=(':!makefiles/kconfig.mk')
    pathspec+=(':!makefiles/docker.inc.mk')
    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message "Don't push RIOT_CONFIG_* definitions upstream. Rather define configuration via Kconfig"
}

check_stderr_null() {
    local patterns=()
    local pathspec=()

    patterns+=(-e '2>[[:blank:]]*&1[[:blank:]]*>[[:blank:]]*/dev/null')

    pathspec+=('Makefile*')
    pathspec+=('**/Makefile*')
    pathspec+=('**/*.mk')
    git -C "${RIOTBASE}" grep -n "${patterns[@]}" -- "${pathspec[@]}" \
        | error_with_message "Redirecting stderr and stdout to /dev/null is \`>/dev/null 2>&1\`; the other way round puts the old stderr to the new stdout."
}

# Test application directories that starts by the following strings are invalid
# and should moved to the corresponding tests subdirectory.
FORBIDDEN_TEST_DIRS=()
FORBIDDEN_TEST_DIRS+=('build_system')
FORBIDDEN_TEST_DIRS+=('core')       # => move under tests/core
FORBIDDEN_TEST_DIRS+=('cpu')        # => move under tests/cpu
FORBIDDEN_TEST_DIRS+=('driver')     # => move under tests/drivers
FORBIDDEN_TEST_DIRS+=('gnrc')       # => move under tests/net
FORBIDDEN_TEST_DIRS+=('periph')     # => move under tests/periph
FORBIDDEN_TEST_DIRS+=('net')        # => move under tests/net
FORBIDDEN_TEST_DIRS+=('pkg')        # => move under tests/pkg
FORBIDDEN_TEST_DIRS+=('sys')        # => move under tests/sys
FORBIDDEN_TEST_DIRS+=('vfs')        # => move under tests/sys
FORBIDDEN_TEST_DIRS+=('xtimer')     # => move under tests/sys
FORBIDDEN_TEST_DIRS+=('ztimer')     # => move under tests/sys

check_tests_application_path() {
    local patterns=()
    patterns+=(-regex "^tests/bench_.*/Makefile")
    for forbidden in "${FORBIDDEN_TEST_DIRS[@]}"; do
        patterns+=(-o -regex "^tests/${forbidden}_.*/Makefile")
    done

    find tests/ -type f "${patterns[@]}" | error_with_message "Invalid application path in tests/"
}

check_pinned_docker_version_is_up_to_date() {
    if [ "$ENABLE_DOCKER_PINNING_TEST" != "1" ]; then
        # Skipping docker version test as requested
        return
    fi
    local pinned_repo_digest
    local upstream_repo_digest
    pinned_repo_digest="$(awk '/^DOCKER_TESTED_IMAGE_REPO_DIGEST := (.*)$/ { print substr($0, index($0, $3)); exit }' "$RIOTMAKE/docker.inc.mk")"
    # not using docker and jq here but a python script to not have to install
    # more stuff for the static test docker image
    IFS=' ' read -r upstream_repo_digest <<< "$("$RIOTTOOLS/buildsystem_sanity_check/get_dockerhub_digests.py" "riot/riotbuild")"

    if [ "$pinned_repo_digest" != "$upstream_repo_digest" ]; then
        git -C "${RIOTBASE}" grep -n '^DOCKER_TESTED_IMAGE_REPO_DIGEST :=' "$RIOTMAKE/docker.inc.mk" \
            | error_with_message  "Update manifest digest to ${upstream_repo_digest}"
    fi
}

check_if_cflags_are_touched_at_right_place() {
    for file in $(git -C "${RIOTBASE}" grep -lE 'CFLAGS[\t ]*\+='); do
        case $file in
        */Makefile.dep)
            # CFLAGS may not be touched in `Makefile.dep`
            git -C "${RIOTBASE}" grep -nE 'CFLAGS[\t ]*\+=' "$file" \
                | error_with_message  "A Makefile.dep is for dependency modelling only. Modify CFLAGS in a Makefile.include, in an app's Makefile, or (if only relevant to a single module/package) in the module's/package's Makefile"
            ;;
        esac
    done
}

error_on_input() {
    ! grep ''
}

all_checks() {
    check_not_parsing_features
    check_providing_features_only_makefile_features
    check_not_exporting_variables
    check_deprecated_vars_patterns
    check_board_do_not_include_cpu_features_dep
    check_cpu_cpu_model_defined_in_makefile_features
    check_not_setting_board_equal
    check_board_insufficient_memory_not_in_makefile
    checks_tests_application_not_defined_in_makefile
    checks_develhelp_not_defined_via_cflags
    check_files_in_boards_not_reference_board_var
    check_no_pseudomodules_in_makefile_dep
    check_no_usemodules_in_makefile_include
    check_no_pkg_source_local
    check_no_riot_config
    check_stderr_null
    check_tests_application_path
    check_pinned_docker_version_is_up_to_date
    check_if_cflags_are_touched_at_right_place
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    github_annotate_setup
    all_checks | error_on_input
    result="$?"
    github_annotate_teardown
    exit "${result}"
fi
