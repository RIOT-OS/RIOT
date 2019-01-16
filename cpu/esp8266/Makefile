# Define the module that is built:
MODULE = cpu

# Add a list of subdirectories, that should also be built:
DIRS += $(RIOTCPU)/esp_common
DIRS += periph
DIRS += sdk
DIRS += vendor

ifneq (, $(filter esp_wifi, $(USEMODULE)))
    DIRS += esp-wifi
endif

include $(RIOTBASE)/Makefile.base
