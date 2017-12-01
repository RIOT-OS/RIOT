include ../Makefile.tests_common

FEATURES_REQUIRED = periph_hwrng

USEPKG += micro-ecc

CFLAGS += -DFEATURE_PERIPH_HWRNG

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
