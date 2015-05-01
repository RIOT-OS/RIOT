APPLICATION = driver_pdc8544
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_gpio periph_spi

USEMODULE += uart0
USEMODULE += shell
USEMODULE += pcd8544

ifeq (,$(TEST_PCD8544_SPI))
  CFLAGS += -DTEST_PCD8544_SPI=SPI_0
endif
ifeq (,$(TEST_PCD8544_CS))
  CFLAGS += -DTEST_PCD8544_CS=GPIO_0
endif
ifeq (,$(TEST_PCD8544_RESET))
  CFLAGS += -DTEST_PCD8544_RESET=GPIO_1
endif
ifeq (,$(TEST_PCD8544_MODE))
  CFLAGS += -DTEST_PCD8544_MODE=GPIO_2
endif

include $(RIOTBASE)/Makefile.include
