#!/bin/sh

# Start in-circuit debugging on this board using openocd
#

# @author Attilio Dona'

CFGDIR=$1
ELFFILE=$2

GDB_INIT_TEST_CASE=$3

cd $CFGDIR

sed "s/TEST_SCRIPT/${GDB_INIT_TEST_CASE}.py/g" ${RIOTBASE}/boards/${BOARD}/dist/gdb_template > ${GDB_INIT_TEST_CASE}

# run GDB
arm-none-eabi-gdb -command=$GDB_INIT_TEST_CASE $ELFFILE

rm ${GDB_INIT_TEST_CASE}