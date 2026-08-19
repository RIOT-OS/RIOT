# shellcheck shell=bash # this script is only sourced, so no shebang

# SPDX-FileCopyrightText: 2020 Martine S. Lenders <m.lenders@fu-berlin.de>
# SPDX-License-Identifier: LGPL-2.1-only

LOG="cat"
LOGFILE=
OUTFILE=github_annotate_outfile.log
ECHO_ESC="echo"

if [ -n "${BASH_VERSION}" ]; then
    # workaround when included in bash to escape newlines and carriage returns
    # properly in _escape
    ECHO_ESC='echo -e'
fi

github_annotate_setup() {
    if [ -n "${GITHUB_RUN_ID}" ]; then
        LOGFILE=run-${GITHUB_RUN_ID}.log
        # shellcheck disable=SC2034  # LOG can be used by the sourcing script
        LOG="tee -a ${LOGFILE}"
    fi
}

github_annotate_is_on() {
    test -n "${LOGFILE}"
    return $?
}

_escape() {
    # see https://stackoverflow.com/a/1252191/11921757
    ${ECHO_ESC} "$1" | sed -e ':a' -e 'N' -e '$!ba' \
        -e 's/%/%25/g' -e 's/\r/%0D/g' -e 's/\n/%0A/g'
}

_github_annotate() {
    MESSAGE="$(_escape "${1}")"
    LEVEL="${2:-error}"
    OPTS="${3:-}"
    echo "::${LEVEL} ${OPTS}::${MESSAGE}" >> ${OUTFILE}
}

github_annotate_error() {
    if [ -n "${GITHUB_RUN_ID}" ]; then
        FILENAME="${1}"
        LINENUM="${2}"
        DETAILS="${3}"
        _github_annotate "${DETAILS}" error "file=${FILENAME},line=${LINENUM}"
    fi
}

github_annotate_error_no_file() {
    if [ -n "${GITHUB_RUN_ID}" ]; then
        DETAILS="${1}"
        _github_annotate "${DETAILS}" error
    fi
}

github_annotate_warning() {
    if [ -n "${GITHUB_RUN_ID}" ]; then
        FILENAME="${1}"
        LINENUM="${2}"
        DETAILS="${3}"
        _github_annotate "${DETAILS}" warning "file=${FILENAME},line=${LINENUM}"
    fi
}

github_annotate_warning_no_file() {
    if [ -n "${GITHUB_RUN_ID}" ]; then
        DETAILS="${1}"
        _github_annotate "${DETAILS}" warning
    fi
}

github_annotate_parse_log_default() {
    ANNOTATE_FUNC="${1:-github_annotate_error}"

    if github_annotate_is_on; then
        PATTERN='^.\+:[0-9]\+:'

        grep "${PATTERN}" "${LOGFILE}" | while read -r line; do
            FILENAME=$(echo "${line}" | cut -d: -f1)
            LINENUM=$(echo "${line}" | cut -d: -f2)
            DETAILS=$(echo "${line}" | cut -d: -f3- |
                      sed -e 's/^[ \t]*//' -e 's/[ \t]*$//')
            ${ANNOTATE_FUNC} "${FILENAME}" "${LINENUM}" "${DETAILS}"
        done
    fi
}

github_annotate_teardown() {
    if [ -n "${LOGFILE}" ]; then
        rm -f "${LOGFILE}"
        LOGFILE=
    fi
}

github_annotate_report_last_run() {
    if [ -n "${GITHUB_RUN_ID}" ] && [ -f "${OUTFILE}" ]; then
        # de-duplicate errors
        sort -u ${OUTFILE} >&2
    fi
    rm -f ${OUTFILE}
}
