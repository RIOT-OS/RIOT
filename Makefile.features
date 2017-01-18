# import list of provided features
-include $(RIOTBOARD)/$(BOARD)/Makefile.features
-include $(RIOTCPU)/$(CPU)/Makefile.features

DEFAULT_FEATURES += periph_pm

# add available default features to required list
FEATURES_REQUIRED += $(filter-out $(DISABLE_FEATURES), $(filter $(FEATURES_PROVIDED), $(DEFAULT_FEATURES)))
