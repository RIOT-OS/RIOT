APPLICATION = clocksync_gtsp
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := chronos msb-430h redbee-econotag telosb wsn430-v1_3b wsn430-v1_4 z1
BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 udoo qemu-i386 stm32f0discovery \
                   stm32f3discovery stm32f4discovery pca10000 pca10005 pttu redbee-econotag \
                   telosb wsn430-v1_4 z1 iot-lab_M3 arduino-mega2560
# mbed_lpc1768:     see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430:          see https://github.com/RIOT-OS/RIOT/issues/658
# arduino-due:      no transceiver, yet
# mbed_lpc1768:     no transceiver, yet
# stm32f4discovery: no transceiver, yet
# qemu-i386:        no transceiver, yet
# stm32f0discovery: no transceiver, yet
# stm32f3discovery: no transceiver, yet
# stm32f4discovery: no transceiver, yet
# pca10000:         no transceiver, yet
# pca10005:         no transceiver, yet
# udoo:             no transceiver, yet
# stm32f0discovery: no transceiver, yet
# stm32f3discovery: no transceiver, yet
# arduino-mega2560  no transceiver, yet
# pttu:             see https://github.com/RIOT-OS/RIOT/issues/659
# wsn430-v1_4		no gtimer toa support for cc2420, yet
# wsn430-v1_3b		no gtimer toa support for cc2420, yet
# telosb			no gtimer toa support for cc2420, yet
# z1				no gtimer toa support for cc2420, yet
# redbee-econotag   no gtimer toa support for maca, yet
# iot-lab_M3   		no gtimer toa support for at86rf231, yet

USEMODULE += gtimer
USEMODULE += gtsp
USEMODULE += defaulttransceiver

include $(RIOTBASE)/Makefile.include
