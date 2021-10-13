# Include this for boards whose timer periph cannot generate a clock frequency
# suitable for xtimer with the available clock sources and dividers.
# This will use ztimer to perform the required frequency conversion.
# By default, xtimer is still used with ztimer as backed, unless
# ztimer_xtimer_compat is used.

ifneq (,$(filter xtimer,$(USEMODULE)))
  ifeq (,$(filter ztimer_xtimer_compat,$(USEMODULE)))
    USEMODULE += xtimer_on_ztimer
  endif
endif
