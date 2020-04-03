MODULE = boards_common_sodaq

ifneq (,$(filter boards_common_samd21-arduino-bootloader,$(USEMODULE)))
  # Add the samd21-arduino-bootloader directory to the build
  DIRS += $(RIOTBOARD)/common/samd21-arduino-bootloader
endif

include $(RIOTBASE)/Makefile.base
