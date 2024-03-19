# Include this for boards whose timer periph cannot generate a clock frequency
# suitable for xtimer with the available clock sources and dividers.
# This will use ztimer to perform the required frequency conversion.
# By default, ztimer_xtimer_compat is used unless xtimer_on_ztimer is
# selected.

ifneq (,$(filter xtimer,$(USEMODULE)))
  ifeq (,$(filter xtimer_on_ztimer,$(USEMODULE)))
    USEMODULE += ztimer_xtimer_compat
  endif
endif
