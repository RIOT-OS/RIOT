MODULE = fatfs_diskio_sdcard_spi

USEMODULE += sdcard_spi

#if periph_rtc is available use it. Otherwise use static timestamps
ifneq (, $(filter $(periph_rtc), $(FEATURES_PROVIDED)))
   CFLAGS += -DFATFS_RTC_AVAILABLE
   USEMODULE += periph_rtc
endif

include $(RIOTBASE)/Makefile.base
