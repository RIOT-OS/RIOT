MODULE = boards_common_arduino-mkr

ifneq (,$(filter boards_common_samdx1-arduino-bootloader,$(USEMODULE)))
  # Add the samdx1-arduino-bootloader directory to the build
  DIRS += $(RIOTBOARD)/common/samdx1-arduino-bootloader
endif

include $(RIOTBASE)/Makefile.base
