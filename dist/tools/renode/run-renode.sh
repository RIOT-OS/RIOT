#!/bin/sh

#
# Unified Renode script for RIOT
#
# This script is supposed to be called from RIOTs make system,
# as it depends on certain environment variables.
#
# It will start the Renode emulator, providing it with several environment
# variables:
# $image_file          Full path to the image file (see $IMAGE_FILE below)
#
# Global environment variables used:
# RENODE:                       Renode command name, default: "renode"
# RENODE_BOARD_CONFIG:          Renode configuration file name,
#                               default: "${RIOTBOARD}/${BOARD}/dist/board.resc"
# RENODE_BOARD_BIN_CONFIG:      Renode intermediate configuration file name,
#                               default: "${BINDIR}/board.resc"
# RENODE_BOARD_REPL:            Renode board repl file name,
#                               default: "${RIOTBOARD}/${BOARD}/dist/board.repl"
# RENODE_BOARD_BIN_REPL:        Renode intermediate board repl file name,
#                               default: "${BINDIR}/board.repl"
# RENODE_CPU_REPL:              Renode cpu repl file name,
#                               default: "${RIOTCPU}/${CPU}/dist/cpu.repl"
# RENODE_CPU_BIN_REPL:          Renode intermediate cpu repl file name,
#                               default: "${BINDIR}/cpu.repl"
#
# @author       Bas Stottelaar <basstottelaar@gmail.com>
# @author       Philipp-Alexander Blum <philipp-blum@jakiku.de>

# Default path to Renode board configuration file
: ${RENODE_BOARD_CONFIG:=${RIOTBOARD}/${BOARD}/dist/board.resc}
# Default path to Renode board intermediate configuration file
: ${RENODE_BOARD_BIN_CONFIG:=${BINDIR}/board.resc}
# Default path to Renode board repl file
: ${RENODE_BOARD_REPL:=${RIOTBOARD}/${BOARD}/dist/board.repl}
# Default path to Renode board intermediate configuration file
: ${RENODE_BOARD_BIN_REPL:=${BINDIR}/board.repl}

# Default path to Renode cpu configuration file
: ${RENODE_CPU_REPL:=${RIOTCPU}/${CPU}/dist/cpu.repl}
# Default path to Renode cpu intermediate configuration file
: ${RENODE_CPU_BIN_REPL:=${BINDIR}/cpu.repl}

# Default Renode command
if [ ! -f "${RENODE}" ]; then
    : ${RENODE:=renode}
fi

# Image file used for emulation
# Default is to use $ELFFILE
: ${IMAGE_FILE:=${ELFFILE}}

#
# config test section.
#
test_config() {
    if [ ! -f "${RENODE_BOARD_CONFIG}" ]; then
        echo "Error: Unable to locate Renode board file"
        echo "       (${RENODE_BOARD_CONFIG})"
        exit 1
    fi
}

#
# helper section.
#
write_cpu_repl() {
    content=$(cat "$RENODE_CPU_REPL" | sed -e 's@$RIOTBASE@'"$RIOTBASE"'@g')
    content=$(echo "$content" | sed -e 's@$RENODE_BOARD_BIN_REPL@'"$RENODE_BOARD_BIN_REPL"'@g')
    echo "$content" > $RENODE_CPU_BIN_REPL
}

write_board_repl() {
    if [ -e $RENODE_BOARD_REPL ]; then
        content=$(cat "$RENODE_BOARD_REPL" | sed -e 's@$RIOTBASE@'"$RIOTBASE"'@g')
        content=$(echo "$content" | sed -e 's@$RENODE_CPU_BIN_REPL@'"$RENODE_CPU_BIN_REPL"'@g')
        echo "$content" > $RENODE_BOARD_BIN_REPL
    fi   
}

write_board_config() {
    echo "\$image_file = '${IMAGE_FILE}'" > "${RENODE_BOARD_BIN_CONFIG}"
    content=$(cat "$RENODE_BOARD_CONFIG" | sed -e 's@$RIOTBASE@'"$RIOTBASE"'@g')
    content=$(echo "$content" | sed -e 's@$RENODE_BOARD_BIN_REPL@'"$RENODE_BOARD_BIN_REPL"'@g')
    content=$(echo "$content" | sed -e 's@$RENODE_CPU_BIN_REPL@'"$RENODE_CPU_BIN_REPL"'@g')
    echo "$content" >> $RENODE_BOARD_BIN_CONFIG
}

write_config() {
    write_cpu_repl
    write_board_repl
    write_board_config
}

#
# now comes the actual actions
#
do_write() {
    test_config
    write_config
    echo "Script written to '${RENODE_BOARD_BIN_CONFIG}'"
}

do_start() {
    test_config
    write_config
    sh -c "${RENODE} '${RENODE_BOARD_BIN_CONFIG}'"
}

#
# parameter dispatching
#
ACTION="$1"

case "${ACTION}" in
  write)
    echo "### Writing emulation script ###"
    do_write
    ;;
  start)
    echo "### Starting Renode ###"
    do_start
    ;;
  *)
    echo "Usage: $0 {write|start}"
    exit 2
    ;;
esac
