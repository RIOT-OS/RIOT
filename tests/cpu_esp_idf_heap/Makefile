BOARD ?= esp32-wroom-32

include ../Makefile.tests_common

FEATURES_REQUIRED += arch_esp

USEMODULE += esp_idf_heap

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter esp_spi_ram,$(FEATURES_PROVIDED)))
  export RIOT_TEST_TIMEOUT=100
  USEMODULE += esp_spi_ram
  CFLAGS += -DCHUNK_SIZE=8192
endif
