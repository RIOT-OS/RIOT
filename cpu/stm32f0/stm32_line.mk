# Compute CPU_LINE
LINE := $(shell echo $(CPU_MODEL) | tr 'a-z-' 'A-Z_' | sed -E -e 's/^STM32F([0-9][0-9][0-9])(.)(.)/\1 \2 \3/')
TYPE := $(word 1, $(LINE))
MODEL1 := $(word 2, $(LINE))
MODEL2 := $(word 3, $(LINE))

ifneq (, $(filter $(TYPE), 030 031 042 070))
  ifneq (, $(filter $(MODEL2), 4 6))
    CPU_LINE = STM32F$(TYPE)x6
  else ifneq (, $(filter $(MODEL2), 8))
    CPU_LINE = STM32F$(TYPE)x8
  else ifneq (, $(filter $(MODEL2), B))
    CPU_LINE = STM32F$(TYPE)xB
  endif
else ifneq (, $(filter $(TYPE), 051))
  CPU_LINE = STM32F$(TYPE)x8
else ifneq (, $(filter $(TYPE), 071 072))
  CPU_LINE = STM32F$(TYPE)xB
else ifneq (, $(filter $(TYPE), 091))
  CPU_LINE = STM32F$(TYPE)xC
else
  CPU_LINE = STM32F$(TYPE)xx
endif

ifeq ($(CPU_LINE), )
  $(error Unsupported CPU)
endif
