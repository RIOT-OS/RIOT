MODULE = gcoap_sysuri

SRC = gcoap_sysuri.c gs_sys.c

ifneq (,$(filter ps,$(USEMODULE)))
  SRC += gs_ps.c
endif

include $(RIOTBASE)/Makefile.base
