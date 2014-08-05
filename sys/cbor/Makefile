MODULE = cbor

CFLAGS += -DCBOR_NO_PRINT

ifeq (,$(filter native,$(BOARD)))
	# build the minimal subset for non-native
	CFLAGS += -DCBOR_NO_FLOAT -DCBOR_NO_PRINT -DCBOR_NO_SEMANTIC_TAGGING
endif

include $(RIOTBASE)/Makefile.base
