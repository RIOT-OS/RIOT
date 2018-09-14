include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

USEMODULE += shell
USEMODULE += pcd8544

# set default device parameters in case they are undefined
TEST_PCD8544_SPI   ?= SPI_DEV\(0\)
TEST_PCD8544_CS    ?= GPIO_PIN\(0,0\)
TEST_PCD8544_RESET ?= GPIO_PIN\(0,1\)
TEST_PCD8544_MODE  ?= GPIO_PIN\(0,2\)

# export parameters
CFLAGS += -DTEST_PCD8544_SPI=$(TEST_PCD8544_SPI)
CFLAGS += -DTEST_PCD8544_CS=$(TEST_PCD8544_CS)
CFLAGS += -DTEST_PCD8544_RESET=$(TEST_PCD8544_RESET)
CFLAGS += -DTEST_PCD8544_MODE=$(TEST_PCD8544_MODE)

include $(TESTBASE)/../Makefile.include
