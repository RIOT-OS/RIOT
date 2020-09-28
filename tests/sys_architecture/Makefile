BOARD ?= samr21-xpro
include ../Makefile.tests_common

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter arch_32bit,$(FEATURES_USED)))
  CFLAGS += -DCORRECT_WORD_BITS=32
endif

ifneq (,$(filter arch_16bit,$(FEATURES_USED)))
  CFLAGS += -DCORRECT_WORD_BITS=16
endif

ifneq (,$(filter arch_8bit,$(FEATURES_USED)))
  CFLAGS += -DCORRECT_WORD_BITS=8
endif
