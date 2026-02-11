MODULE = cpu_efm32lg

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

# include vendor system
SRC += $(PKGDIRBASE)/gecko_sdk/dist/platform/Device/SiliconLabs/EFM32LG/Source/system_efm32lg.c

include $(RIOTBASE)/Makefile.base
