DEFAULT_MODULE += board cpu core core_msg sys

DEFAULT_MODULE += auto_init

ifneq (,$(filter ssp,$(FEATURES_PROVIDED)))
  DEFAULT_MODULE += ssp
endif
