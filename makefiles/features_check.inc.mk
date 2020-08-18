# Check if all required FEATURES are provided

# Receives a list of list of '|' separated features and resturns for each
# item in list, the first match that is present in the whitelist.
#
# _features_one_out_of <features-list> <whitelist>
#
# Parameters
#   features-list: list of list of '|' separated features
#     e.g.: "periph_spi|periph_i2c puf_sram|periph_hwrng"
#   whitelist: possible features to select one out of
#     e.g.: "$(FEATURES_PROVIDED)"
#
# One out of Algorithm:
#  - For each list in features-list as "item":
#     - Store the intersection of features-whitelist and the features in
#       "item" in "tmp"
#     - Append "item" to "tmp" (with pipes between features, e.g.
#       "periph_spi|periph_i2c")
#     - Appends the first's element of "tmp" to the caller
#   ==> If one (or more) features in whitelist is listed in item, the first
#       one will be taken.
#   ==> At the end of the list item itself (with pipes between features) is the
#       last item in "tmp". If no feature in the feature-list is in whitelist
#       this will be the only item in "tmp" and be picked
_features_one_out_of = $(foreach item, $1, $(firstword \
                          $(filter $(subst |, ,$(item)), $2) $(item)))

# Features that are provided and not blacklisted
FEATURES_USABLE := $(filter-out $(FEATURES_BLACKLIST),$(FEATURES_PROVIDED))

FEATURES_OPTIONAL_ONLY := $(sort $(filter-out $(FEATURES_REQUIRED),$(FEATURES_OPTIONAL)))
FEATURES_OPTIONAL_USED := $(sort $(filter $(FEATURES_PROVIDED),$(FEATURES_OPTIONAL_ONLY)))

# Optional features that will not be used because they are not provided
FEATURES_OPTIONAL_MISSING := $(sort $(filter-out $(FEATURES_PROVIDED),$(FEATURES_OPTIONAL_ONLY)))

# Features that are used without taking "one out of" dependencies into account
FEATURES_USED_SO_FAR := $(sort $(FEATURES_REQUIRED) $(FEATURES_OPTIONAL_USED))

# Additionally required features due to the "one out of" dependencies
#   For each features list in $(FEATURES_REQUIRED_ANY):
#     ==> If one (or more) features is already used then the first one
#         will be taken
#     ==> If one (or more) features is usable select the first on in list
#     ==> If no feature is usable return the list
FEATURES_REQUIRED_ONE_OUT_OF := $(call _features_one_out_of,\
                                  $(FEATURES_REQUIRED_ANY),\
                                    $(FEATURES_USED_SO_FAR) \
                                    $(FEATURES_USABLE))

# Features that are required by the application but not provided by the BSP
# Having features missing may case the build to fail.
FEATURES_MISSING := $(sort \
                      $(filter-out $(FEATURES_PROVIDED),\
                        $(FEATURES_REQUIRED) $(FEATURES_REQUIRED_ONE_OUT_OF)))

# Features that are used for an application
FEATURES_USED := $(sort $(FEATURES_REQUIRED) \
                        $(FEATURES_REQUIRED_ONE_OUT_OF) \
                        $(FEATURES_OPTIONAL_USED))

# Used features that conflict when used together
FEATURES_CONFLICTING := $(sort $(foreach conflict,\
                                 $(FEATURES_CONFLICT),\
                                 $(call _features_conflicting,$(conflict))))

# Return conflicting features from the conflict string feature1:feature2
#   $1: feature1:feature2
#   Return the list of conflicting features
_features_conflicting = $(if $(call _features_used_conflicting,$(subst :, ,$1)),$(subst :, ,$1))
# Check if all features from the list are used
#   $1: list of features that conflict together
#   Return non empty on error
_features_used_conflicting = $(filter $(words $1),$(words $(filter $(FEATURES_USED),$1)))

# Features that are used by the application but blacklisted by the BSP.
# Having blacklisted features may cause the build to fail.
FEATURES_USED_BLACKLISTED := $(sort $(filter $(FEATURES_USED), $(FEATURES_BLACKLIST)))
