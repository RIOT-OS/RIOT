# Configure the programmer related variables

PROGRAMMER_QUIET ?= $(QUIET)
ifeq (0,$(PROGRAMMER_QUIET))
  PROGRAMMER_VERBOSE_OPT ?= --verbose
endif

# Don't use the programmer wrapper for the CI (where speed and verbose output
# are important)
ifneq (1,$(RIOT_CI_BUILD))
  USE_PROGRAMMER_WRAPPER_SCRIPT ?= 1
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
