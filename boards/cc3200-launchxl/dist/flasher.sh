#!/bin/sh

TOOL_DIR=$1
PORT=$2
ELFILE=$3

# change folder since cc3200prog expects dlls in its pwd
cd ${TOOL_DIR}
echo "[FLASHING] CC3200: port="${PORT}" file="${ELFILE}
# flash binary
./cc3200prog ${PORT} ${ELFILE}
