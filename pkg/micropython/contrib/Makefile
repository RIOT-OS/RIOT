.PHONY: all

CROSS_COMPILE = $(PREFIX)

# re-use RIOT's variant of verbose build
ifeq ($(QUIET),1)
  BUILD_VERBOSE = 0
else
  BUILD_VERBOSE = 1
endif

include $(TOP)/py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

INC =  -I$(CURDIR)
INC += -I$(TOP)/shared/readline
INC += -I$(BUILD)
INC += $(INCLUDES)

CFLAGS += -std=gnu11
CFLAGS += -DNDEBUG
CFLAGS += $(INC)

# disable some warnings that would otherwise error out
CFLAGS += -Wno-vla
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-pedantic
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-cast-align

SRC_C = $(filter-out modtime_riot.c, $(wildcard *.c)) \
  extmod/machine_spi.c \
  extmod/modtime.c \
  shared/readline/readline.c \
  shared/runtime/pyexec.c \
  shared/timeutils/timeutils.c

# list of sources for qstr extraction
SRC_QSTR += $(SRC_C)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

all: $(BINDIR)/micropython.a

$(BINDIR)/micropython.a: $(OBJ)
	$(AD)$(AR) $(ARFLAGS) $@ $?

include $(TOP)/py/mkrules.mk
