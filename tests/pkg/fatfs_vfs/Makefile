include ../Makefile.pkg_common

# boards with SD Card connected to a SD/MMC host controller
BOARD_WITH_MTD_SDMMC := \
    esp32-olimex-evb \
    esp32-wrover-kit \
    stm32f746g-disco \
    same54-xpro \
    stm32l496g-disco \
    #

USEMODULE += fatfs_vfs
FEATURES_OPTIONAL += periph_rtc
FEATURES_REQUIRED += periph_spi

# remove this if you don't want to format your SD card
USEMODULE += fatfs_vfs_format
CFLAGS += -DCONFIG_FATFS_FORMAT_ALLOC_STATIC=1

FATFS_IMAGE_FILE_SIZE_MIB ?= 128

TEST_DEPS += image

ifneq (,$(filter native native32 native64,$(BOARD)))
  USEMODULE += mtd_native

  # overwrite default mtd_native-config to use fat image as flash device
  MTD_NATIVE_FILENAME    ?= \"./bin/riot_fatfs_disk.img\"
  MTD_SECTOR_NUM  ?= \(\(\(FATFS_IMAGE_FILE_SIZE_MIB\)*1024*1024\)/MTD_SECTOR_SIZE\)
  CFLAGS += -DMTD_NATIVE_FILENAME=$(MTD_NATIVE_FILENAME)
  CFLAGS += -DFATFS_IMAGE_FILE_SIZE_MIB=$(FATFS_IMAGE_FILE_SIZE_MIB)
  CFLAGS += -DMTD_SECTOR_NUM=$(MTD_SECTOR_NUM)
else ifneq  (,$(filter $(BOARD_WITH_MTD_SDMMC),$(BOARD)))
  # use mtd_sdmmc for boards with SD Card connected to SD/MMC host controller
  USEMODULE += mtd_sdmmc
else
  # use mtd_card for SD Cards in SPI mode otherwise
  USEMODULE += mtd_sdcard
endif

include $(RIOTBASE)/Makefile.include

image:
	@tar -xjf riot_fatfs_disk.tar.gz -C ./bin/

#this generates a compressed fat image file that can be used by the fat driver on native
compressed-image:
	@./create_fat_image_file.sh $(FATFS_IMAGE_FILE_SIZE_MIB)
