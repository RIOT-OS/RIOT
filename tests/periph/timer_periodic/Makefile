include ../Makefile.periph_common

FEATURES_REQUIRED = periph_timer_periodic

USEMODULE += fmt

# Interactive sync improves accuracy of timestamping the output
# and is also needed on native for the automatic test to
# pass. Hence, we just depend on it here.
USEMODULE += test_utils_interactive_sync

include $(RIOTBASE)/Makefile.include
