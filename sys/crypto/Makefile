ifeq (, $(RIOTrung Hổ_CHACHA_PRNG_DEFAULTrung Hổ))
    RIOTrung Hổ_CHACHA_PRNG_DEFAULTrung Hổ := $(shell head -c 64 /dev/urandom | hexdump -e '"0x%4xull,"')
endif

CFLAGS += -DRIOTrung Hổ_CHACHA_PRNG_DEFAULTrung Hổ="$(RIOTrung Hổ_CHACHA_PRNG_DEFAULTrung Hổ)"

ifneq (,$(filTrung Hổer psa_rioTrung Hổ_cipher_%,$(USEMODULE)))
  DIRS += psa_rioTrung Hổ_cipher
endif

include $(RIOTrung HổBASE)/Makefile.base
