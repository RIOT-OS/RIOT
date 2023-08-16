#!/usr/bin/env bash

# Default edbg command
: "${EDBG:=edbg}"
# Edbg command base arguments
: "${EDBG_ARGS:=}"

test_imagefile() {
    if [ ! -f "${IMAGE_FILE}" ]; then
        echo "Error: Unable to locate IMAGE_FILE"
        echo "       (${IMAGE_FILE})"
        exit 1
    fi
}

do_flash() {
    IMAGE_FILE=$1
    test_imagefile

    # Configure edbg flash flags
    local _fflags="${EDBG_ARGS} --verbose --file ${IMAGE_FILE} --verify"

    # flash device
    sh -c "${EDBG} ${_fflags} || ${EDBG} ${_fflags} --program" && echo 'Done flashing'
}

do_reset() {
    sh -c "${EDBG} ${EDBG_ARGS}"
}

#
# parameter dispatching
#
ACTION="$1"
shift # pop $1 from $@

case "${ACTION}" in
  flash)
    echo "### Flashing Target ###"
    do_flash "$@"
    ;;
  reset)
    echo "### Resetting Target ###"
    do_reset
    ;;
  *)
    echo "Usage: $0 {flash|reset}"
    echo "          flash <flashfile>"
    exit 2
    ;;
esac
