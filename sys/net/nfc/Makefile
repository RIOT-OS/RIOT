ifneq (,$(filter nfc_ndef,$(USEMODULE)))
  DIRS += ndef
endif
ifneq (,$(filter nfc_nfct,$(USEMODULE)))
  DIRS += nfct
endif
ifneq (,$(filter nfc_t2t,$(USEMODULE)))
  DIRS += t2t
endif

include $(RIOTBASE)/Makefile.base
