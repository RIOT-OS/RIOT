APPLICATION = driver_mcp2515
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio periph_spi

USEMODULE += shell
USEMODULE += can
# USEMODULE += auto_init_can
USEMODULE += mcp2515

# set default device parameters in case they are undefined
TEST_MCP2515_SPI   ?= SPI_0
TEST_MCP2515_CS    ?= GPIO_PIN\(0,0\)
TEST_MCP2515_RESET ?= GPIO_PIN\(0,1\)
TEST_MCP2515_MODE  ?= GPIO_PIN\(0,2\)

# export parameters
CFLAGS += -DTEST_MCP2515_SPI=$(TEST_MCP2515_SPI)
CFLAGS += -DTEST_MCP2515_CS=$(TEST_MCP2515_CS)
CFLAGS += -DTEST_MCP2515_RESET=$(TEST_MCP2515_RESET)
CFLAGS += -DTEST_MCP2515_MODE=$(TEST_MCP2515_MODE)

include $(RIOTBASE)/Makefile.include
