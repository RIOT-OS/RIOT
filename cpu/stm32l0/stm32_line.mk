# Compute CPU_LINE
LINE := $(shell echo $(CPU_MODEL) | tr 'a-z-' 'A-Z_' | sed -E -e 's/^STM32L([0-9][0-9][0-9])(.)(.)/\1 \2 \3/')
TYPE := $(word 1, $(LINE))
MODEL1 := $(word 2, $(LINE))
MODEL2 := $(word 3, $(LINE))

ifneq (, $(filter $(TYPE), 010))
  ifneq (, $(filter $(MODEL2), 4))
    CPU_LINE = STM32L$(TYPE)x4
  else ifneq (, $(filter $(MODEL2), 6))
    CPU_LINE = STM32L$(TYPE)x6
  else ifneq (, $(filter $(MODEL2), 8))
    CPU_LINE = STM32L$(TYPE)x8
  else ifneq (, $(filter $(MODEL2), B))
    CPU_LINE = STM32L$(TYPE)xB
  endif
else
  CPU_LINE = STM32L$(TYPE)xx
endif

ifeq ($(CPU_LINE), )
  $(error Unsupported CPU)
endif
