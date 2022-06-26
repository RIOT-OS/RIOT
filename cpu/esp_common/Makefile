# add a list of subdirectories, that should also be build
DIRS += periph

ifneq (,$(filter xtensa%,$(TARGET_ARCH)))
  DIRS += vendor
endif

ifneq (,$(filter esp_freertos_common,$(USEMODULE)))
  DIRS += freertos
endif

ifneq (,$(filter esp_now,$(USEMODULE)))
  DIRS += esp-now
endif

ifneq (,$(filter esp_wifi,$(USEMODULE)))
  DIRS += esp-wifi
endif

ifneq (,$(filter esp_xtensa,$(USEMODULE)))
  DIRS += esp-xtensa
endif

include $(RIOTBASE)/Makefile.base
