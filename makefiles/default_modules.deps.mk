# This files contains dependencies for default modules. They are parsed at the
# end of the dependency loop. They MAY inlcude new modules, but this modules
# MUST NOT have dependencies themselfs.

ifneq (,$(filter auto_init_ztimer,$(USEMODULE)))
  USEMODULE += ztimer_init
endif

ifneq (,$(filter ztimer_auto_adjust,$(USEMODULE)))
  USEMODULE += ztimer_overhead
endif

ifneq (,$(filter auto_init_saul,$(USEMODULE)))
  USEMODULE += saul_init_devs
endif

ifneq (,$(filter xtimer_on_periph_timer,$(USEMODULE)))
  USEMODULE += div
endif
