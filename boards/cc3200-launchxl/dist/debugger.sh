#!/bin/sh

CONF_DIR=$1
PREGIX=$2
ELFFILE=$3

# change to the current folder
cd ${CONF_DIR}

# start gdb
${PREFIX}gdb -x ./cc3200_gdb.conf ${ELFFILE}
