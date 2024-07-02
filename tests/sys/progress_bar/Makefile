include ../Makefile.sys_common

USEMODULE += xtimer
USEMODULE += progress_bar

include $(RIOTBASE)/Makefile.include

# Make custom progress bar configuration from app.config
# available in Python test script via environment variables
export CONFIG_PROGRESS_BAR_FULL_CHARACTER
export CONFIG_PROGRESS_BAR_EMPTY_CHARACTER
export CONFIG_PROGRESS_BAR_LENGTH
