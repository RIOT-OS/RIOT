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

SRC_C = \
  extmod/modtime.c \
  gccollect.c \
  help.c \
  machine_pin.c \
  modmachine.c \
  modriot.c \
  mp_riot.c \
  mphalport.c \
  shared/readline/readline.c \
  shared/runtime/pyexec.c \
  shared/timeutils/timeutils.c

ifneq (,$(filter periph_adc,$(USEMODULE)))
  SRC_C += machine_adc.c
  CFLAGS += -DMICROPY_PY_MACHINE_ADC=1
endif

ifneq (,$(filter periph_spi,$(USEMODULE)))
  SRC_C += \
    extmod/machine_spi.c \
    machine_hw_spi.c
  CFLAGS += -DMICROPY_PY_MACHINE_SPI=1
endif

ifneq (,$(filter xtimer,$(USEMODULE)))
  SRC_C += modxtimer.c
endif

# list of sources for qstr extraction
SRC_QSTR += $(SRC_C)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

all: $(BINDIR)/micropython.a

$(BINDIR)/micropython.a: $(OBJ)
	$(AD)$(AR) $(ARFLAGS) $@ $?

include $(TOP)/py/mkrules.mk
