APPLICATION = pkg_fatfs_vfs
include ../Makefile.tests_common

USEMODULE += fatfs_vfs

FATFS_DISKIO_NATIVE_DEFAULT_FILE ?= \"./bin/riot_fatfs_disk.img\"

# whitelist can be removed when the problem described in #6063 was resolved
# this list is composed of boards that support spi + native
BOARD_WHITELIST := native airfy-beacon arduino-due arduino-duemilanove arduino-mega2560 \
                   arduino-uno arduino-zero avsextrem cc2538dk fox frdm-k64f iotlab-a8-m3 \
                   iotlab-m3 limifrog-v1 maple-mini msb-430 msb-430h msba2 msbiot mulle \
                   nrf52840dk nrf52dk nrf6310 nucleo-f072 nucleo-f091 nucleo-f103 \
                   nucleo-f302 nucleo-f303 nucleo-f334 nucleo-f401 nucleo-f410 nucleo-f411 \
                   nucleo-f446 nucleo-l053 nucleo-l073 nucleo-l1 nucleo-l476 nucleo144-f207 \
                   nucleo144-f303 nucleo144-f413 nucleo144-f429 nucleo144-f446 nucleo32-f031 \
                   nucleo32-f042 nucleo32-f303 nucleo32-l031 nucleo32-l432 openmote-cc2538 \
                   pba-d-01-kw2x pca10005 remote-pa remote-reva remote-revb samd21-xpro \
                   saml21-xpro samr21-xpro sodaq-autonomo spark-core stm32f0discovery \
                   stm32f3discovery stm32f4discovery telosb udoo waspmote-pro weio \
                   wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1

# export is needed to pass this value to the Makefile of the native_diskio module
export FATFS_DISKIO_NATIVE_DEFAULT_FILE

include $(RIOTBASE)/Makefile.include

#this generates a compressed fat image file that can be used by the fat driver on native
fatimage:
	@./create_fat_image_file.sh

test:
	@tar -xjf riot_fatfs_disk.tar.gz -C ./bin/
	./tests/01-run.py
