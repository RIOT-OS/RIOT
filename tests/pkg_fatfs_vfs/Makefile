APPLICATION = pkg_fatfs_vfs
include ../Makefile.tests_common

USEMODULE += fatfs_vfs
FEATURES_OPTIONAL += periph_rtc

CFLAGS += -DVFS_FILE_BUFFER_SIZE=72 -DVFS_DIR_BUFFER_SIZE=44

FATFS_IMAGE_FILE_SIZE_MIB ?= 128

ifeq ($(BOARD),native)
  USEMODULE += mtd_native

  #overwrite default mtd_native-config to use fat image as flash device
  MTD_NATIVE_FILENAME    ?= \"./bin/riot_fatfs_disk.img\"
  MTD_PAGE_SIZE   ?= 512
  MTD_SECTOR_SIZE ?= 512
  MTD_SECTOR_NUM  ?= \(\(\(FATFS_IMAGE_FILE_SIZE_MIB\)*1024*1024\)/MTD_SECTOR_SIZE\)
  CFLAGS += -DMTD_NATIVE_FILENAME=$(MTD_NATIVE_FILENAME)
  CFLAGS += -DMTD_PAGE_SIZE=$(MTD_PAGE_SIZE)
  CFLAGS += -DMTD_SECTOR_SIZE=$(MTD_SECTOR_SIZE)
  CFLAGS += -DFATFS_IMAGE_FILE_SIZE_MIB=$(FATFS_IMAGE_FILE_SIZE_MIB)
  CFLAGS += -DMTD_SECTOR_NUM=$(MTD_SECTOR_NUM)
else
  USEMODULE += mtd_sdcard
endif

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

# this list is composed of boards with sufficient memory and support spi/gpio + native
BOARD_WHITELIST := airfy-beacon arduino-due arduino-duemilanove arduino-mega2560 \
                   arduino-mkr1000 arduino-mkrzero arduino-uno arduino-zero avsextrem \
                   b-l072z-lrwan1 b-l475e-iot01a bluepill cc2538dk ek-lm4f120xl \
                   feather-m0 fox frdm-k22f frdm-k64f ikea-tradfri iotlab-a8-m3 \
                   iotlab-m3 limifrog-v1 maple-mini msb-430 msb-430h msba2 msbiot \
                   mulle nrf52840dk nrf52dk nrf6310 nucleo-f207zg nucleo144-f303 \
                   nucleo144-f412 nucleo144-f413 nucleo144-f429 nucleo144-f446 \
                   nucleo32-f042 nucleo32-f303 nucleo32-l031 \
                   nucleo32-l432 nucleo-f072rb nucleo-f091rc nucleo-f103rb nucleo-f302r8 \
                   nucleo-f303re nucleo-f334r8 nucleo-f401re nucleo-f410rb nucleo-f411re \
                   nucleo-f446 nucleo-l053 nucleo-l073 nucleo-l152 nucleo-l476 \
                   nz32-sc151 openmote-cc2538 pba-d-01-kw2x remote-pa remote-reva \
                   remote-revb samd21-xpro saml21-xpro samr21-xpro sltb001a \
                   sodaq-autonomo sodaq-explorer spark-core stm32f0discovery \
                   stm32f3discovery stm32f4discovery telosb udoo waspmote-pro \
                   wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1 native

include $(RIOTBASE)/Makefile.include

image:
	@tar -xjf riot_fatfs_disk.tar.gz -C ./bin/

#this generates a compressed fat image file that can be used by the fat driver on native
compressed-image:
	@./create_fat_image_file.sh $(FATFS_IMAGE_FILE_SIZE_MIB)

test: image
	./tests/01-run.py
