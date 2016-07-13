APPLICATION = pkg_u8g2
include ../Makefile.tests_common

# These boards have not sufficient memory
BOARD_BLACKLIST += chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += xtimer

USEPKG += u8g2

# set default device parameters in case they are undefined
TEST_U8G2_OUTPUT    ?= 1
TEST_U8G2_DISPLAY   ?= u8g2_Setup_ssd1306_128x64_noname_1

TEST_U8G2_I2C       ?= 0
TEST_U8G2_SPI       ?= 0

TEST_U8G2_ADDR      ?= 0x78

TEST_U8G2_PIN_CS    ?= GPIO_PIN\(0,0\)
TEST_U8G2_PIN_DC    ?= GPIO_PIN\(0,0\)
TEST_U8G2_PIN_RESET ?= GPIO_PIN\(0,0\)

# features depend on output type
ifeq ($(TEST_U8G2_OUTPUT),1)
USEMODULE += u8g2_utf8
endif

ifeq ($(TEST_U8G2_OUTPUT),2)
USEMODULE += u8g2_sdl
endif

ifeq ($(TEST_U8G2_OUTPUT),3)
FEATURES_REQUIRED += periph_gpio periph_spi
endif

ifeq ($(TEST_U8G2_OUTPUT),4)
FEATURES_REQUIRED += periph_gpio periph_i2c
endif

# export parameters
CFLAGS += -DTEST_U8G2_OUTPUT=$(TEST_U8G2_OUTPUT)
CFLAGS += -DTEST_U8G2_DISPLAY=$(TEST_U8G2_DISPLAY)

CFLAGS += -DTEST_U8G2_SPI=$(TEST_U8G2_SPI)
CFLAGS += -DTEST_U8G2_I2C=$(TEST_U8G2_I2C)

CFLAGS += -DTEST_U8G2_ADDR=$(TEST_U8G2_ADDR)

CFLAGS += -DTEST_U8G2_PIN_CS=$(TEST_U8G2_PIN_CS)
CFLAGS += -DTEST_U8G2_PIN_DC=$(TEST_U8G2_PIN_DC)
CFLAGS += -DTEST_U8G2_PIN_RESET=$(TEST_U8G2_PIN_RESET)

include $(RIOTBASE)/Makefile.include
