ifneq (,$(filter congure_quic,$(USEMODULE)))
  DIRS += quic
endif
ifneq (,$(filter congure_mock,$(USEMODULE)))
  DIRS += mock
endif
ifneq (,$(filter congure_test,$(USEMODULE)))
  DIRS += test
endif

include $(RIOTBASE)/Makefile.base
