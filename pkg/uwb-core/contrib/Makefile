MODULE = uwb-core_contrib

MODULE_DEFS_NEEDED += \
  uwb_core_event_thread \
  uwb_core_rng \
  uwb_core_twr_ds \
  uwb_core_twr_ds_ext \
  uwb_core_twr_ss \
  uwb_core_twr_ss_ack \
  uwb_core_twr_ss_ext \
  uwb_core_uwbcfg \
  #

SRC = uwb_core.c

ifneq (,$(filter auto_init_uwb-core,$(USEMODULE)))
  SRC += uwb_core_init.c
endif

include $(RIOTBASE)/Makefile.base
