# add a list of subdirectories, that should also be build
DIRS += periph
DIRS += vendor

ifneq (,$(filter esp_freertos_common,$(USEMODULE)))
  DIRS += freertos
endif

ifneq (,$(filter esp_now,$(USEMODULE)))
  DIRS += esp-now
endif

ifneq (,$(filter esp_wifi,$(USEMODULE)))
  DIRS += esp-wifi
endif

include $(RIOTBASE)/Makefile.base
