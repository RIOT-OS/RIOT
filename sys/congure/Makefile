ifneq (,$(filter congure_mock,$(USEMODULE)))
  DIRS += mock
endif
ifneq (,$(filter congure_reno,$(USEMODULE)))
  DIRS += reno
endif
ifneq (,$(filter congure_reno_methods,$(USEMODULE)))
  DIRS += reno/methods
endif
ifneq (,$(filter congure_test,$(USEMODULE)))
  DIRS += test
endif

include $(RIOTBASE)/Makefile.base
