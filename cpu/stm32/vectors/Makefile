MODULE = stm32_vectors

NO_AUTO_SRC = 1

SRC_FILE = vectors_$(subst stm32,,$(CPU_FAM)).c

SRCS += $(SRC_FILE)
# (file triggers compiler bug. see #5775)
SRC_NOLTO += $(SRC_FILE)

include $(RIOTBASE)/Makefile.base
