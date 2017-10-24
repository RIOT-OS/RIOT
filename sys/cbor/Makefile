MODULE = cbor

ifneq ($(shell uname -s),Darwin)
  CFLAGS_cbor += -D_XOPEN_SOURCE=600
endif

include $(RIOTBASE)/Makefile.base
