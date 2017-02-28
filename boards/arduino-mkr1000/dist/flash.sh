#!/bin/sh

# To put the board in update mode, manually double press the reset button before
# running `make flash`.

if [ `uname` = "Linux" ]; then
    stty -F "${PORT}" raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
    "${RIOTBOARD}"/"${BOARD}"/dist/bossac --port=${PORT} -i -b -U true -i -e -w -v "${HEXFILE}" -R
elif [ `uname` = "Darwin" ]; then
    stty -f ${PORT} raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
    "${RIOTBOARD}"/"${BOARD}"/dist/bossac_osx --port=${PORT} -i -b -U true -i -e -w -v "${HEXFILE}" -R
else
    echo "CAUTION: No flash tool for your host system found!"
fi
