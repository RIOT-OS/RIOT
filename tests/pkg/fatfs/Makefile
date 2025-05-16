include ../Makefile.pkg_common

BOARD ?= native

# boards with SD Card connected to a SD/MMC host controller
BOARD_WITH_MTD_SDMMC := \
    esp32-olimex-evb \
    esp32-wrover-kit \
    stm32f746g-disco \
    same54-xpro \
    stm32l496g-disco \
    #

FEATURES_OPTIONAL += periph_rtc
FEATURES_REQUIRED += periph_spi

USEMODULE += shell
USEMODULE += fatfs_diskio_mtd
USEMODULE += mtd
USEPKG    += fatfs

FATFS_IMAGE_FILE_SIZE_MIB ?= 128

ifneq (,$(filter native native32 native64,$(BOARD)))
  # overwrite default mtd_native-config to use fat image as flash device
  CFLAGS += -DMTD_NATIVE_FILENAME=\"./bin/riot_fatfs_disk.img\"
  CFLAGS += -DFATFS_IMAGE_FILE_SIZE_MIB=$(FATFS_IMAGE_FILE_SIZE_MIB)
  CFLAGS += -DMTD_SECTOR_NUM=\(\(\(FATFS_IMAGE_FILE_SIZE_MIB\)*1024*1024\)/MTD_SECTOR_SIZE\)
else ifneq  (,$(filter $(BOARD_WITH_MTD_SDMMC),$(BOARD)))
  # use mtd_sdmmc for boards with SD Card connected to SD/MMC host controller
  USEMODULE += mtd_sdmmc
else
  # use mtd_card for SD Cards in SPI mode otherwise
  USEMODULE += mtd_sdcard
endif

image:
	${Q}mkdir -p bin
	${Q}tar -xjf riot_fatfs_disk.tar.bz2 -C ./bin/

# this generates a compressed fat image file that can be used by the fat driver on native
compressed-image:
	${Q}./create_fat_image_file.sh $(FATFS_IMAGE_FILE_SIZE_MIB)

include $(RIOTBASE)/Makefile.include
