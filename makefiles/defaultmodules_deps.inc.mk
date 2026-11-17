# This files contains dependencies for default modules. They are parsed at the
# end of the dependency loop. They MAY include new modules, but these modules
# MUST NOT have dependencies themselves.

ifneq (,$(filter auto_init%,$(USEMODULE)))
  USEMODULE += preprocessor preprocessor_successor
endif

ifneq (,$(filter auto_init_ztimer,$(USEMODULE)))
  USEMODULE += ztimer_init
endif

ifneq (,$(filter ztimer_auto_adjust,$(USEMODULE)))
  USEMODULE += ztimer_overhead
endif

ifneq (,$(filter auto_init_saul,$(USEMODULE)))
  USEMODULE += saul_init_devs
endif

ifneq (,$(filter auto_init_libcose_crypt,$(USEMODULE)))
  USEMODULE += libcose_crypt_init
endif

# Any nRF CPU will select nrfx_headers_only. If an application or another
# package selects USEPKG+=nrfx, the nrfx_full module will be selected
# to tell pkg/nrfx to fetch all files.
ifneq (,$(filter nrfx_headers_only,$(USEMODULE)))
  ifneq (,$(filter nrfx,$(USEPKG)))
    USEMODULE += nrfx_full
  else
    USEPKG += nrfx
  endif
endif

ifneq (,$(filter random,$(USEMODULE)))
  # select default prng if no prng is selected
  ifeq (,$(filter prng_%,$(USEMODULE)))
    USEMODULE += prng_musl_lcg
  endif
endif

ifneq (,$(filter xtimer,$(USEMODULE)))
  ifeq (,$(filter ztimer_xtimer_compat,$(USEMODULE)))
    USEMODULE += div
  endif
endif
