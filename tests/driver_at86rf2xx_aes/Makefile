include ../Makefile.tests_common

DISABLE_MODULE += auto_init_at86rf2xx

# define the driver to be used for selected boards
ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox,$(BOARD)))
  DRIVER := at86rf231
endif
ifneq (,$(filter mulle,$(BOARD)))
  DRIVER := at86rf212b
endif

TEST_ON_CI_WHITELIST += samr21-xpro iotlab-m3 mulle

# use the at86rf231 as fallback device
DRIVER ?= at86rf231

# include the selected driver
USEMODULE += $(DRIVER)
USEMODULE += at86rf2xx_aes_spi

include $(RIOTBASE)/Makefile.include
