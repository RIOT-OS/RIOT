CFLAGS += -DCBOR_NO_PRINT

ifeq (,$(filter native,$(BOARD)))
	CFLAGS += -DCBOR_NO_FLOAT -DCBOR_NO_PRINT -DCBOR_NO_SEMANTIC_TAGGING
endif

include $(RIOTBASE)/Makefile.base
