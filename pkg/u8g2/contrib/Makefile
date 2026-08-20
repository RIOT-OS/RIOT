MODULE = u8g2_csrc_riot

MODULE_DEFS_NEEDED += \
  periph_i2c \
  periph_spi \
  #

ifneq (,$(filter u8g2_disp_dev,$(USEMODULE)))
  DIRS += $(RIOTBASE)/pkg/u8g2/contrib/disp_dev
endif

include $(RIOTBASE)/Makefile.base
