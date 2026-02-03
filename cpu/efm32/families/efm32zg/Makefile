MODULE = cpu_efm32zg

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

# include vendor system
SRC += $(PKGDIRBASE)/gecko_sdk/dist/platform/Device/SiliconLabs/EFM32ZG/Source/system_efm32zg.c

include $(RIOTBASE)/Makefile.base
