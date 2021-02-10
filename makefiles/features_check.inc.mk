# Check if all required FEATURES are provided

# Features that are provided and not blacklisted
FEATURES_USABLE := $(filter-out $(FEATURES_BLACKLIST),$(FEATURES_PROVIDED))

# Features that may be used, if provided.
FEATURES_OPTIONAL_ONLY := $(sort $(filter-out $(FEATURES_REQUIRED),$(FEATURES_OPTIONAL)))

# Optional features that end up being used
FEATURES_OPTIONAL_USED := $(sort $(filter $(FEATURES_USABLE),$(FEATURES_OPTIONAL_ONLY)))

# Optional features that will not be used because they are not provided or blacklisted
FEATURES_OPTIONAL_MISSING := $(sort $(filter-out $(FEATURES_USABLE),$(FEATURES_OPTIONAL_ONLY)))

# Features that are used without taking "one out of" dependencies into account
FEATURES_USED_SO_FAR := $(sort $(FEATURES_REQUIRED) $(FEATURES_OPTIONAL_USED))

# Additionally required features due to the "one out of" dependencies
# Algorithm:
#  - For each list in FEATURES_REQUIRED_ANY as "item":
#     - Store the intersection of FEATURES_USED_SO_FAR and the features in
#       "item" in "tmp"
#     - Append the intersection of FEATURES_USABLE and the features in "item"
#       to "tmp"
#     - Append "item" to "tmp" (with pipes between features, e.g.
#       "periph_spi|periph_i2c")
#     - Append the first element of "tmp" to FEATURES_REQUIRED_ONE_OUT_OF
#   ==> If one (or more) already used features is listed in item, this (or
#       one of these) will be in the front of "tmp" and be taken
#   ==> If one (or more) usable features is listed in item, this will come
#       afterwards. If no no feature in item is used so for, one of the
#       features supported and listed in item will be picked
#   ==> At the end of the list item itself (with pipes between features) is the
#       last item in "tmp". If no feature is item is supported or used, this
#       will be the only item in "tmp" and be picked
FEATURES_REQUIRED_ONE_OUT_OF := $(foreach item,\
                                  $(FEATURES_REQUIRED_ANY),\
                                  $(word 1,\
                                    $(filter $(FEATURES_USED_SO_FAR),$(subst |, ,$(item)))\
                                    $(filter $(FEATURES_USABLE),$(subst |, ,$(item)))\
                                    $(item)))

# Features that are required by the application but not provided by the BSP
# Having features missing may case the build to fail.
FEATURES_MISSING := $(sort \
                      $(filter-out $(FEATURES_PROVIDED),\
                        $(FEATURES_REQUIRED) $(FEATURES_REQUIRED_ONE_OUT_OF)))

# Features that are used for an application
FEATURES_USED := $(sort $(FEATURES_REQUIRED) \
                        $(FEATURES_REQUIRED_ONE_OUT_OF) \
                        $(FEATURES_OPTIONAL_USED))

# Return conflicting features from the conflict string feature1:feature2
#   $1: feature1:feature2
#   Return the list of conflicting features
_features_conflicting = $(if $(call _features_used_conflicting,$(subst :, ,$1)),$(subst :, ,$1))
# Check if all features from the list are used
#   $1: list of features that conflict together
#   Return non empty on error
_features_used_conflicting = $(filter $(words $1),$(words $(filter $(FEATURES_USED),$1)))

# Used features that conflict when used together
FEATURES_CONFLICTING := $(sort $(foreach conflict,\
                                 $(FEATURES_CONFLICT),\
                                 $(call _features_conflicting,$(conflict))))

# Features that are used by the application but blacklisted by the BSP.
# Having blacklisted features may cause the build to fail.
FEATURES_USED_BLACKLISTED := $(sort $(filter $(FEATURES_USED), $(FEATURES_BLACKLIST)))
