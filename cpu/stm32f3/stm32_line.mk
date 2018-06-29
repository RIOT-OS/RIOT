# Compute CPU_LINE
LINE := $(shell echo $(CPU_MODEL) | tr 'a-z-' 'A-Z_' | sed -E -e 's/^STM32F([0-9][0-9][0-9])(.)(.)/\1 \2 \3/')
TYPE := $(word 1, $(LINE))
MODEL1 := $(word 2, $(LINE))
MODEL2 := $(word 3, $(LINE))

ifneq (, $(filter $(TYPE), 301))
  CPU_LINE = STM32F$(TYPE)x8
else ifneq (, $(filter $(TYPE), 302 303))
  ifneq (, $(filter $(MODEL2), 6 8))
    CPU_LINE = STM32F$(TYPE)x8
  else ifneq (, $(filter $(MODEL2), B C))
    CPU_LINE = STM32F$(TYPE)xC
  else ifneq (, $(filter $(MODEL2), D E))
    CPU_LINE = STM32F$(TYPE)xE
  else
    $(error Unsuported CPU)
  endif
else ifneq (, $(filter $(TYPE), 373))
  CPU_LINE = STM32F$(TYPE)xC
else ifneq (, $(filter $(TYPE), 334))
  CPU_LINE = STM32F$(TYPE)x8
else
  CPU_LINE = STM32F3$(TYPE)xx
endif
