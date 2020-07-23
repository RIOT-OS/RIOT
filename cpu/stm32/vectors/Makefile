MODULE = stm32_vectors

NO_AUTO_SRC = 1

ifneq (,$(filter STM32F030x4,$(CPU_LINE)))
  SRC_FILE = $(CPU_LINE).c
else
  SRC_FILE = gen_$(CPU_LINE).c
endif

SRCS += $(SRC_FILE)
# (file triggers compiler bug. see #5775)
SRC_NOLTO += $(SRC_FILE)

include $(RIOTBASE)/Makefile.base
