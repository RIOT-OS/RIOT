MODULE = cpu_efm32gg

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

# include vendor system
SRC += $(PKGDIRBASE)/gecko_sdk/dist/platform/Device/SiliconLabs/EFM32GG/Source/system_efm32gg.c

include $(RIOTBASE)/Makefile.base
