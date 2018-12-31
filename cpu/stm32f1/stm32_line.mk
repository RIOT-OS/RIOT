# Compute CPU_LINE
LINE := $(shell echo $(CPU_MODEL) | tr 'a-z-' 'A-Z_' | sed -E -e 's/^STM32F([0-9][0-9][0-9])(.)(.)/\1 \2 \3/')
TYPE := $(word 1, $(LINE))
MODEL1 := $(word 2, $(LINE))
MODEL2 := $(word 3, $(LINE))

ifneq (, $(filter $(TYPE), 100))
  ifneq (, $(filter $(MODEL2), 4 6 8 B))
    CPU_LINE = STM32F$(TYPE)xB
  else ifneq (, $(filter $(MODEL2), C D E))
    CPU_LINE = STM32F$(TYPE)xE
  else
    $(error Unsuported CPU)
  endif
else ifneq (, $(filter $(TYPE), 101 102 103))
  ifneq (, $(filter $(MODEL2), 4 6))
    CPU_LINE = STM32F$(TYPE)x6
  else ifneq (, $(filter $(MODEL2), 8 B))
    CPU_LINE = STM32F$(TYPE)xB
  else ifneq (, $(filter $(MODEL2), C D E))
    CPU_LINE = STM32F$(TYPE)xE
  else ifneq (, $(filter $(MODEL2), F G))
    CPU_LINE = STM32F$(TYPE)xG
  else
    $(error Unsuported CPU)
  endif
else ifneq (, $(filter $(TYPE), 105 107))
  CPU_LINE = STM32F$(TYPE)xC
else
  $(error Unsuported CPU)
endif
