BOARD ?= same54-xpro

include ../Makefile.periph_common

USEMODULE += shell

FEATURES_REQUIRED += periph_dac_play

LOW_MEMORY_BOARDS := samd10-xmini

# Disable the greeting sample if the board has too little memory
# or flashing takes too long
ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  CFLAGS += -DCONIFG_DAC_BUF_SAMPLES=512
endif

include $(RIOTBASE)/Makefile.include
