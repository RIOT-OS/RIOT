# PSA Crypto casts like hell, so this needs to shut the f up
CFLAGS += -Wno-cast-align

# include the PSA headers
INCLUDES += -I$(RIOTBASE)/sys/psa_crypto/include

ifneq (,$(filter psa_key_slot_mgmt,$(USEMODULE)))
  DIRS += psa_key_slot_mgmt
endif

ifneq (,$(filter psa_se_mgmt,$(USEMODULE)))
  DIRS += psa_se_mgmt
endif

include $(RIOTBASE)/Makefile.base
