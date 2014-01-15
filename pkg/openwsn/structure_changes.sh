#!/usr/bin/zsh

printf "Moving openwsn stack ..."
# move openwsn stack directory up
mv openwsn-fw-RB-1.4/firmware/openos/openwsn/ ./

# and all needed hw dependent files too
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/spi.c openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/uart.c openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/leds.c openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/board.c openwsn/board_ow.c
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/board_info.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/radiotimer.c openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/eui64.c openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/telosb/debugpins.c openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/board.h openwsn/board_ow.h
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/debugpins.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/eui64.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/leds.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/radio.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/radiotimer.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/boards/uart.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/chips/spi.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/chips/cc2420/radio.c openwsn
mv openwsn-fw-RB-1.4/firmware/openos/bsp/chips/cc2420/cc2420.h openwsn
mv openwsn-fw-RB-1.4/firmware/openos/drivers/common/openhdlc.* openwsn
mv openwsn-fw-RB-1.4/firmware/openos/drivers/common/opentimers.* openwsn
mv openwsn-fw-RB-1.4/firmware/openos/drivers/common/openserial.{c,h} openwsn
mv openwsn-fw-RB-1.4/firmware/openos/kernel/openos/scheduler.{c,h} openwsn
printf "[OK]\n"
printf  "Removing files not needed ... "
# remove all *dox files
for i in `find ./openwsn -name "*.dox"`; do
rm -f $i
done

rm -f openwsn/SConscript
rm -rf openwsn/02.5-MPLS
printf "[OK]\n"

printf  "Initialize Makefile structure ..."
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
printf "[OK]\n"

mkdir openwsn/07-App/r6tus
touch openwsn/07-App/r6tus/r6tus.c
touch openwsn/07-App/r6tus/r6tus.h

printf  "Clean up ..."
# clean not need files
rm -rf openwsn-fw-RB-1.4
printf "[OK]\n"

printf  "Remove CRLF line endings ... "
# deal with crlf line endings
for i in `find ./openwsn -type f`; do
perl -pi -e 's/\r\n/\n/g' $i
done
printf "[OK]\n"
