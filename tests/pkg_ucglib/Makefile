include ../Makefile.tests_common

USEMODULE += xtimer

USEPKG += ucglib

# set default device parameters in case they are undefined
TEST_OUTPUT ?= 1

TEST_SPI ?= 0

TEST_PIN_CS ?= GPIO_UNDEF
TEST_PIN_CD ?= GPIO_UNDEF
TEST_PIN_RESET ?= GPIO_UNDEF

ifeq ($(TEST_OUTPUT),3)
  TEST_DISPLAY ?= ucg_dev_ssd1331_18x96x64_univision
  TEST_DISPLAY_EXT ?= ucg_ext_ssd1331_18
endif

# features depend on output type
ifeq ($(TEST_OUTPUT),2)
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=48*1024
  USEMODULE += ucglib_sdl
endif

ifeq ($(TEST_OUTPUT),3)
  FEATURES_REQUIRED += periph_gpio periph_spi
endif

# export parameters
CFLAGS += -DTEST_OUTPUT=$(TEST_OUTPUT)

CFLAGS += -DTEST_SPI=$(TEST_SPI)

CFLAGS += -DTEST_PIN_CS=$(TEST_PIN_CS)
CFLAGS += -DTEST_PIN_CD=$(TEST_PIN_CD)
CFLAGS += -DTEST_PIN_RESET=$(TEST_PIN_RESET)

CFLAGS += -DTEST_DISPLAY=$(TEST_DISPLAY)
CFLAGS += -DTEST_DISPLAY_EXT=$(TEST_DISPLAY_EXT)

include $(RIOTBASE)/Makefile.include
