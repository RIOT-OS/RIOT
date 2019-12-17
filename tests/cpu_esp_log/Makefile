BOARD ?= esp32-wroom-32
DEVELHELP=0
LOG_LEVEL=4

include ../Makefile.tests_common

FEATURES_REQUIRED += arch_esp

USEMODULE += esp_log_tagged
USEMODULE += app_metadata
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

DISABLE_MODULE += auto_init
DISABLE_MODULE += test_utils_interactive_sync

APP_SHELL_FMT ?= NONE

include $(RIOTBASE)/Makefile.include
