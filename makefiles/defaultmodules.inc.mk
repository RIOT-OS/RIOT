DEFAULT_MODULE += board cpu core core_init core_msg core_panic sys

DEFAULT_MODULE += auto_init

ifneq (,$(filter ssp,$(FEATURES_PROVIDED)))
  DEFAULT_MODULE += ssp
endif
