#!/bin/sh

export RIOTBOOT_SKIP_COMPILE=1
set -e

${APPDIR}/tests/common/assert_slotnum.py 0 0

APP_VER=1 make -C${APPDIR} riotboot/flash-slot1
${APPDIR}/tests/common/assert_slotnum.py 1 1

APP_VER=2 make -C${APPDIR} riotboot/flash-slot0
${APPDIR}/tests/common/assert_slotnum.py 0 2

echo "[TEST PASSED]"
