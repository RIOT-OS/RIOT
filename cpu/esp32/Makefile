# Define the module that is built:
MODULE = cpu

# Add a list of subdirectories, that should also be built:
DIRS += $(RIOTCPU)/esp_common
DIRS += periph
DIRS += vendor

ifneq (, $(filter esp_cxx, $(USEMODULE)))
    DIRS += cxx
endif

ifneq (, $(filter esp_eth, $(USEMODULE)))
    DIRS += esp-eth
endif

ifneq (, $(filter esp_freertos, $(USEMODULE)))
    DIRS += freertos
endif

include $(RIOTBASE)/Makefile.base
