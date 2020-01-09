include ../Makefile.tests_common

# avr8, msp430, esp8266 and mips don't support ssp (yet)
FEATURES_BLACKLIST += arch_avr8 arch_esp8266 arch_mips32r2 arch_msp430

USEMODULE += ssp

include $(RIOTBASE)/Makefile.include

ifneq (,$(shell $(CC) --help=warnings | grep '\-Wstringop-overflow='))
  CFLAGS += -Wstringop-overflow=0
endif
