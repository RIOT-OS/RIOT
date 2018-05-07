# Compute CPU_LINE
LINE := $(shell echo $(CPU_MODEL) | tr 'a-z-' 'A-Z_' | sed -E -e 's/^STM32F([0-9][0-9][0-9])(.)(.)/\1 \2 \3/')
TYPE := $(word 1, $(LINE))
MODEL1 := $(word 2, $(LINE))
MODEL2 := $(word 3, $(LINE))

ifneq (, $(filter $(TYPE), 401))
  ifneq (, $(filter $(MODEL2), B C))
    CPU_LINE = STM32F$(TYPE)xC
  else ifneq (, $(filter $(MODEL2), D E))
    CPU_LINE = STM32F$(TYPE)xE
  endif
else ifneq (, $(filter $(TYPE), 410 412))
  CPU_LINE = STM32F$(TYPE)$(MODEL1)x
else ifneq (, $(filter $(TYPE), 411))
  CPU_LINE = STM32F$(TYPE)xE
else
  CPU_LINE = STM32F$(TYPE)xx
endif

ifeq ($(CPU_LINE), )
  $(error Unsupported CPU)
endif
