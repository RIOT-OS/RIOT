APPLICATION = pnet
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := chronos msb-430h redbee-econotag telosb wsn430-v1_3b wsn430-v1_4 z1
BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 udoo qemu-i386 stm32f0discovery \
                   stm32f3discovery stm32f4discovery pca10000 pca10005
# mbed_lpc1768:     see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430:          see https://github.com/RIOT-OS/RIOT/issues/658
# qemu-i386:        no transceiver, yet
# stm32f0discovery: no transceiver, yet
# stm32f3discovery: no transceiver, yet
# stm32f4discovery: no transceiver, yet
# pca10000:         no transceiver, yet
# pca10005:         no transceiver, yet

USEMODULE += posix
USEMODULE += pnet
USEMODULE += vtimer
USEMODULE += defaulttransceiver

include $(RIOTBASE)/Makefile.include
