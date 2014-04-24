#!/usr/bin/env sh

function _out() {
    [[ $QUIET != "1" ]] && printf "$@"
}
QUIET=0

_out "\n*INFO* restructuring OpenWSN tree\n"

_out "Moving openwsn stack ..."
# move openwsn stack directory up
mv openwsn-fw-RB-1.4/firmware/openos/openwsn/ ./

# and all needed hw dependent files too
#
# telosb disabled for now
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/spi.c openwsn
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/uart.c openwsn/uart_ow.c
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/leds.c openwsn
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/board.c openwsn/board_ow.c
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/board_info.h openwsn
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/radiotimer.c openwsn
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/eui64.c openwsn
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/debugpins.c openwsn
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/chips/cc2420/radio.c openwsn
# mv openwsn-fw-RB-1.4/firmware/openos/bsp/chips/cc2420/cc2420.h openwsn

#
# common BSP files
_out "common..."
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/board.h openwsn/board_ow.h
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/debugpins.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/eui64.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/leds.h openwsn/leds_ow.h
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/radio.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/radiotimer.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/uart.h openwsn/uart_ow.h
mv openwsn-fw-RB-1.4/firmware/openos/bsp/chips/spi.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/drivers/common/openhdlc.* openwsn
mv openwsn-fw-RB-1.4/firmware/openos/drivers/common/opentimers.* openwsn
mv openwsn-fw-RB-1.4/firmware/openos/drivers/common/openserial.{c,h} openwsn
mv openwsn-fw-RB-1.4/firmware/openos/kernel/openos/scheduler.{c,h} openwsn

#
# iot-lab_M3 BSPs
_out "BSPs..."
touch openwsn/Systick.{c,h}
touch openwsn/at86rf231.h
touch openwsn/board_info.h
touch openwsn/board_ow.c
touch openwsn/debugpins.c
touch openwsn/eui64.c
touch openwsn/exti.c
touch openwsn/gpio.{c,h}
touch openwsn/nvic.{c,h}
touch openwsn/leds_ow.c
touch openwsn/radio.c
touch openwsn/radiotimer.c
touch openwsn/rcc.{c,h}
touch openwsn/spi.c
touch openwsn/stm32f10x_lib.h
touch openwsn/uart_ow.c
touch openwsn/isr.c
_out "[OK]\n"

_out  "Removing files not needed ... "
# remove all *dox files
for i in `find ./openwsn -name "*.dox"`; do
rm -f $i
done

rm -f openwsn/SConscript
rm -rf openwsn/02.5-MPLS
_out "[OK]\n"

_out  "Initialize Makefile structure ..."
# create empty Makefiles
touch openwsn/Makefile

for i in `find ./openwsn -type d`; do
touch $i/Makefile
done

# meta Makefile
cp ../Makefile.in ./
mv ./Makefile.in ./Makefile

# oops too many Makefiles
rm -f openwsn/07-App/rxl1/Makefile \
        openwsn/07-App/rt/Makefile \
        openwsn/07-App/rrube/Makefile \
        openwsn/07-App/rreg/Makefile \
        openwsn/07-App/rleds/Makefile \
        openwsn/07-App/rheli/Makefile \
        openwsn/07-App/rex/Makefile \
        openwsn/07-App/layerdebug/Makefile \
        openwsn/07-App/imu/Makefile \
        openwsn/07-App/heli/Makefile
_out "[OK]\n"

_out  "Clean up ..."
# clean not need files
rm -rf openwsn-fw-RB-1.4
_out "[OK]\n"

_out  "Remove CRLF line endings ... "
# deal with crlf line endings
for i in `find ./openwsn -type f`; do
perl -pi -e 's/\r\n/\n/g' $i
done
_out "[OK]\n"

exit 0
