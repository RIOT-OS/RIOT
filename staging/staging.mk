# staging.mk
# this makefile is being included by staging/Makefile.include and staging/Makefile.dep.

#
# list of staging modules
#
# In order to mark a module as broken, add it to the "BROKEN" variable.
# That way, it's dependees will be marked broken implicitly.
#
#STAGING_MODULES += test
#BROKEN += test    # (broken due to change in X, see #12345)

#
# list of stating applications
#
# In order to mark a staging application as "broken", turn it's
# line into a comment (and add a note).
#
#STAGING_APPS += broken/application/example # (intentionally broken by design)

#
# housekeeping
#
STAGING_MODULES := $(sort $(STAGING_MODULES))
STAGING_MODULES_USED := $(filter $(USEMODULE),$(STAGING_MODULES))
STAGING_APPS := $(sort $(STAGING_APPS))

BROKEN := $(sort $(BROKEN))

STAGING_MODULES_USED_BROKEN := \
  $(sort $(filter $(BROKEN), $(STAGING_MODULES_USED)))

# turn dependencies to broken staging modules into unsatisfied feature
# requirements (broken-staging-module-<module_name>).
# This will cause
#  a) manually called "make" issue a warning
#  b) "make info-boards-supported" skip any dependee, effectively disabling the CI
#
ifneq (, $(STAGING_MODULES_USED_BROKEN))
  FEATURES_REQUIRED += $(patsubst %,broken-staging-module-%,$(STAGING_MODULES_USED_BROKEN))
endif

# list of folders with staging modules
STAGING_DIRS := $(sort $(wildcard $(STAGING_MODULES_USED:%=$(RIOTBASE)/staging/%)))

# create app make file list to be consumed by makefiles/app_dirs.inc.mk
STAGING_APPLICATION_MAKEFILES := \
    $(sort \
        $(wildcard \
            $(patsubst %,$(RIOTBASE)/staging/%/Makefile,$(STAGING_APPS)) \
    ))
