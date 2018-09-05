# exclude pthread_tls if not selected explicitly
ifeq (,$(filter pthread_tls,$(USEMODULE)))
  SRC := $(filter-out pthread_tls.c,$(wildcard *.c))
endif

include $(RIOTBASE)/Makefile.base
