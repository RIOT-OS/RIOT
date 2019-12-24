# add a list of subdirectories, that should also be build
DIRS += vendor

ifneq (, $(filter esp_now, $(USEMODULE)))
    DIRS += esp-now
endif

include $(RIOTBASE)/Makefile.base
