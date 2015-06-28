ifeq (, ${RIOT_CHACHA_PRNG_DEFAULT})
    RIOT_CHACHA_PRNG_DEFAULT := $(shell head -c 64 /dev/urandom | hexdump -e '"0x%4xull,"')
endif

CFLAGS += -DRIOT_CHACHA_PRNG_DEFAULT="${RIOT_CHACHA_PRNG_DEFAULT}"

include $(RIOTBASE)/Makefile.base
