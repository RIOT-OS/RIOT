# import list of provided features
include $(RIOTBOARD)/$(BOARD)/Makefile.features

# if the CPU does not specify periph_pm, use the fallback module automatically
ifeq (,$(filter periph_pm,$(FEATURES_PROVIDED)))
  USEMODULE += pm_fallback_set_lowest
  USEMODULE += pm_fallback_off
  USEMODULE += pm_fallback_reboot
  USEMODULE += pm_fallback
#FEATURES_PROVIDED += periph_pm
endif

DEFAULT_FEATURES += periph_pm

# add available default features to required list
FEATURES_REQUIRED += $(filter-out $(DISABLE_FEATURES), $(filter $(FEATURES_PROVIDED), $(DEFAULT_FEATURES)))
