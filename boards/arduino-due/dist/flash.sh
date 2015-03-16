#!/bin/sh

if [ `uname` = "Linux" ]; then
    stty -F "${PORT}" raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
    "${RIOTBOARD}"/"${BOARD}"/dist/bossac  -R -e -w -v -b "${HEXFILE}"
elif [ `uname` = "Darwin" ]; then
    stty -f $(PORT) raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
    "${RIOTBOARD}"/"${BOARD}"/dist/bossac_osx -R -e -w -v -b "${HEXFILE}"
else
    echo "CAUTION: No flash tool for your host system found!"
fi
