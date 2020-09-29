# make all code end up in "ztimer_core.a"
MODULE := ztimer_core

# ensure that "ztimer_foo" builds "ztimer_foo.c", not "ztimer_core_foo.c"
BASE_MODULE := ztimer

# ztimer_core files
SRC := core.c util.c periodic.c

# enable submodules
SUBMODULES := 1

# disable obsolete warning
CFLAGS += -Wno-missing-field-initializers

include $(RIOTBASE)/Makefile.base
