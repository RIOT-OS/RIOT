BOARD ?= nucleo-f767zi

include ../Makefile.cpu_common

USEMODULE += mpu_noexec_ram

# boards using highlevel_stdio will crash without printing
FEATURES_BLACKLIST += highlevel_stdio

include $(RIOTBASE)/Makefile.include
