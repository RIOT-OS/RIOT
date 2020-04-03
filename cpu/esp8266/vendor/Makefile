# Add a list of subdirectories, that should also be built:
DIRS += esp-idf

ifneq (, $(filter esp_gdbstub, $(USEMODULE)))
    DIRS += esp-gdbstub
endif

include $(RIOTBASE)/Makefile.base
