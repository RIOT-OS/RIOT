# Add a list of subdirectories, that should also be built:
DIRS += esp-idf

include $(RIOTBASE)/Makefile.base
# vendor code contains casts that increase alignment requirements. Let's hope
# those are false positives.
CFLAGS += -Wno-cast-align
