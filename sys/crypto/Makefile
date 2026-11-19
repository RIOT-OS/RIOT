ifeq (, $(RIOT_CHACHA_PRNG_DEFAULT))
    RIOT_CHACHA_PRNG_DEFAULT := $(shell hexdump -v -n 64 -e '"0x%08xull,"' /dev/urandom)
endif

CFLAGS += -DRIOT_CHACHA_PRNG_DEFAULT="$(RIOT_CHACHA_PRNG_DEFAULT)"

ifneq (,$(filter psa_riot_cipher_%,$(USEMODULE)))
  DIRS += psa_riot_cipher
endif

include $(RIOTBASE)/Makefile.base
