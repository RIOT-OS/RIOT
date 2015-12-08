MODULE = periph

# see periph/random.c
ifneq (, ${RIOT_XMC_EXTERNAL_SEED})
    RIOT_XMC_SEED := $(shell head -c 10 ${RIOT_XMC_EXTERNAL_SEED} | hexdump -e '1/2 "0x%2x, "')
    CFLAGS += -DRIOT_XMC_SEED="${RIOT_XMC_SEED}"
endif

include $(RIOTBASE)/Makefile.base
