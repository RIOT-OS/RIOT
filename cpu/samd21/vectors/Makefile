MODULE = samd21_vectors

NO_AUTO_SRC = 1

ifneq (,$(filter samd1%,$(CPU_MODEL)))
  SRC_FILE = vectors_samd1x.c
else
  SRC_FILE = vectors_samd2x.c
endif

SRCS += $(SRC_FILE)
# (file triggers compiler bug. see #5775)
SRC_NOLTO += $(SRC_FILE)

include $(RIOTBASE)/Makefile.base
