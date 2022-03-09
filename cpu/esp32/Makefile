# Define the module that is built:
MODULE = cpu

SRC = irq_arch.c startup.c syscalls.c

# Add a list of subdirectories, that should also be built:
DIRS += $(RIOTCPU)/esp_common
DIRS += periph
DIRS += vendor

ifneq (, $(filter esp_eth, $(USEMODULE)))
    DIRS += esp-eth
endif

ifneq (, $(filter esp_freertos, $(USEMODULE)))
    DIRS += freertos
endif

ifneq (,$(filter esp_wifi% esp_eth, $(USEMODULE)))
    SRC += esp_ztimer.c
endif

include $(RIOTBASE)/Makefile.base
