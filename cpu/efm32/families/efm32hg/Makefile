MODULE = cpu_efm32hg

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

# include vendor system
SRC += $(PKGDIRBASE)/gecko_sdk/dist/platform/Device/SiliconLabs/EFM32HG/Source/system_efm32hg.c

include $(RIOTBASE)/Makefile.base
