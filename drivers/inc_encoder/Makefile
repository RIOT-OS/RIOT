ifneq (,$(filter inc_encoder_hardware,$(USEMODULE)))
  DIRS += backends/hardware
endif

ifneq (,$(filter inc_encoder_software,$(USEMODULE)))
  DIRS += backends/software
endif

include $(RIOTMAKE)/driver_with_saul.mk
