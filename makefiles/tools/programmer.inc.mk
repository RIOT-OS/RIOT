# Configure the programmer related variables

PROGRAMMER_QUIET ?= $(QUIET)
ifeq (0,$(PROGRAMMER_QUIET))
  PROGRAMMER_VERBOSE_OPT ?= --verbose
endif

# When multiple debuggers are connected then pyocd shows an interactive
# UI to select the user interface to flash, with the python wrapper this
# is lost and will also cause the wrapper to hang waiting for user input.
# As long as a similar functionality is not provided by the wrapper
# then disable it for pyocd.
PROGRAMMER_WRAPPER_BLACKLIST ?= pyocd

# Don't use the programmer wrapper for the CI (where speed and verbose output
# are important)
ifneq (1,$(RIOT_CI_BUILD))
  ifneq (,$(filter $(PROGRAMMER),$(PROGRAMMER_WRAPPER_BLACKLIST)))
    USE_PROGRAMMER_WRAPPER_SCRIPT ?= 0
  else
    USE_PROGRAMMER_WRAPPER_SCRIPT ?= 1
  endif
else
  USE_PROGRAMMER_WRAPPER_SCRIPT ?= 0
endif

ifeq (1,$(USE_PROGRAMMER_WRAPPER_SCRIPT))
  PROGRAMMER_FLASH ?= @$(RIOTTOOLS)/programmer/programmer.py \
    --action Flashing --cmd "$(FLASHER) $(FFLAGS)" \
    --programmer "$(PROGRAMMER)" $(PROGRAMMER_VERBOSE_OPT)
  PROGRAMMER_RESET ?= @$(RIOTTOOLS)/programmer/programmer.py \
  --action Resetting --cmd "$(RESET) $(RESET_FLAGS)" \
  --programmer "$(PROGRAMMER)" $(PROGRAMMER_VERBOSE_OPT)
else
  PROGRAMMER_FLASH ?= $(FLASHER) $(FFLAGS)
  PROGRAMMER_RESET ?= $(RESET) $(RESET_FLAGS)
endif
