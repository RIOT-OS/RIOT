include ../Makefile.tests_common

USEMODULE += fatfs_vfs
FEATURES_OPTIONAL += periph_rtc

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

# this list is composed of boards with sufficient memory and support spi/gpio + native
BOARD_WHITELIST := airfy-beacon arduino-due arduino-duemilanove \
                   arduino-leonardo arduino-mega2560 arduino-mkr1000 \
                   arduino-mkrzero arduino-uno arduino-zero avsextrem \
                   b-l072z-lrwan1 b-l475e-iot01a blackpill blackpill-128kib \
                   bluepill bluepill-128kib cc2538dk ek-lm4f120xl \
                   feather-m0 fox frdm-k22f frdm-k64f ikea-tradfri i-nucleo-lrwan1 \
                   iotlab-a8-m3 iotlab-m3 limifrog-v1 maple-mini msba2 msbiot \
                   mulle nrf52840dk nrf52dk nrf6310 nucleo-f207zg nucleo-f303ze \
                   nucleo-f412zg nucleo-f413zh nucleo-f429zi nucleo-f446ze \
                   nucleo-f042k6 nucleo-f303k8 nucleo-l031k6 \
                   nucleo-l432kc nucleo-f072rb nucleo-f091rc nucleo-f103rb nucleo-f302r8 \
                   nucleo-f303re nucleo-f334r8 nucleo-f401re nucleo-f410rb nucleo-f411re \
                   nucleo-f446re nucleo-l053r8 nucleo-l073rz nucleo-l152re nucleo-l476rg \
                   nz32-sc151 openmote-cc2538 pba-d-01-kw2x remote-pa remote-reva \
                   remote-revb samd21-xpro saml21-xpro samr21-xpro \
                   sensebox_samd21 sltb001a sodaq-autonomo sodaq-explorer \
                   spark-core stm32f0discovery stm32f3discovery \
                   stm32f4discovery stm32l0538-disco udoo waspmote-pro \
                   yunjia-nrf51822 native

TEST_DEPS += image

include $(RIOTBASE)/Makefile.include

image:
	@tar -xjf riot_fatfs_disk.tar.gz -C ./bin/

#this generates a compressed fat image file that can be used by the fat driver on native
compressed-image:
	@./create_fat_image_file.sh $(FATFS_IMAGE_FILE_SIZE_MIB)
