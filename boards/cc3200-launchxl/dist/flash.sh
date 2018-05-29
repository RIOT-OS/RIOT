#!/bin/sh
#
# cc3200 launchpad flash with Energia cc3200prog
#
# @author Attilio Dona' <@AttilioDona>

ENERGIA_ROOT=$1
UART_PORT=$2

CC3200PROG_DIR=$ENERGIA_ROOT/hardware/tools/lm4f/bin/

cd $CC3200PROG_DIR

$CC3200PROG_DIR/cc3200prog $UART_PORT $3