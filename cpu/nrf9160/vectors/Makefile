MODULE = nrf9160_vectors

NO_AUTO_SRC = 1

SRC_FILE = vectors_$(CPU_MODEL).c

SRCS += $(SRC_FILE)
# (file triggers compiler bug. see #5775)
SRC_NOLTO += $(SRC_FILE)

include $(RIOTBASE)/Makefile.base
