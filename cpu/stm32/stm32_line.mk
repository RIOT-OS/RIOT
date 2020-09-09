# Compute CPU_LINE

ifeq (F,$(STM32_TYPE))
  ifeq (0,$(STM32_FAMILY))  # STM32F0
    ifneq (, $(filter $(STM32_MODEL), 030 031 042 070))
      ifneq (,$(filter $(STM32_ROMSIZE), 4))
        CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)x4
      else ifneq (,$(filter $(STM32_ROMSIZE), 6))
        CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)x6
      else ifneq (,$(filter $(STM32_ROMSIZE), 8))
        CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)x8
      else ifneq (,$(filter $(STM32_ROMSIZE), B))
        CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)xB
      else ifneq (,$(filter $(STM32_ROMSIZE), C))
        CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)xC
      endif
    else ifneq (,$(filter $(STM32_MODEL), 051))
      CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)x8
    else ifneq (,$(filter $(STM32_MODEL), 071 072))
      CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)xB
    else ifneq (,$(filter $(STM32_MODEL), 091))
      CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)xC
    else
      CPU_LINE = STM32F$(STM32_MODEL)xx
    endif
  endif
  ifeq (1,$(STM32_FAMILY))  # STM32F1
    ifneq (,$(filter $(STM32_MODEL), 100))
      ifneq (, $(filter $(STM32_ROMSIZE), 4 6 8 B))
        CPU_LINE = STM32F$(STM32_MODEL)xB
      else ifneq (,$(filter $(STM32_ROMSIZE), C D E))
        CPU_LINE = STM32F$(STM32_MODEL)xE
      else
        $(error Unsuported CPU)
      endif
    else ifneq (,$(filter $(STM32_MODEL), 101 102 103))
      ifneq (, $(filter $(STM32_ROMSIZE), 4 6))
        CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)x6
      else ifneq (,$(filter $(STM32_ROMSIZE), 8 B))
        CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)xB
      else ifneq (,$(filter $(STM32_ROMSIZE), C D E))
        CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)xE
      else ifneq (,$(filter $(STM32_ROMSIZE), F G))
        CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)xG
      else
        $(error Unsuported CPU)
      endif
    else ifneq (,$(filter $(STM32_MODEL), 105 107))
      CPU_LINE = STM32$(STM32_TYPE)$(STM32_MODEL)xC
    endif
  endif
  ifeq (3,$(STM32_FAMILY))  # STM32F3
    ifneq (,$(filter $(STM32_MODEL), 301))
      CPU_LINE = STM32F$(STM32_MODEL)x8
    else ifneq (,$(filter $(STM32_MODEL), 302 303))
      ifneq (, $(filter $(STM32_ROMSIZE), 6 8))
        CPU_LINE = STM32F$(STM32_MODEL)x8
      else ifneq (,$(filter $(STM32_ROMSIZE), B C))
        CPU_LINE = STM32F$(STM32_MODEL)xC
      else ifneq (,$(filter $(STM32_ROMSIZE), D E))
        CPU_LINE = STM32F$(STM32_MODEL)xE
      else
        $(error Unsuported CPU)
      endif
    else ifneq (,$(filter $(STM32_MODEL), 373))
      CPU_LINE = STM32F$(STM32_MODEL)xC
    else ifneq (,$(filter $(STM32_MODEL), 334))
      CPU_LINE = STM32F$(STM32_MODEL)x8
    else
      CPU_LINE = STM32F3$(STM32_MODEL)xx
    endif
  endif
  ifeq (4,$(STM32_FAMILY))  # STM32F4
    ifneq (,$(filter $(STM32_MODEL), 401))
      ifneq (,$(filter $(STM32_ROMSIZE), B C))
        CPU_LINE = STM32F$(STM32_MODEL)xC
      else ifneq (,$(filter $(STM32_ROMSIZE), D E))
        CPU_LINE = STM32F$(STM32_MODEL)xE
      endif
    else ifneq (,$(filter $(STM32_MODEL), 410 412))
      CPU_LINE = STM32F$(STM32_MODEL)$(STM32_PINCOUNT)x
    else ifneq (,$(filter $(STM32_MODEL), 411))
      CPU_LINE = STM32F$(STM32_MODEL)xE
    else
      CPU_LINE = STM32F$(STM32_MODEL)xx
    endif
  endif
endif
ifeq (L,$(STM32_TYPE))
  ifeq (0,$(STM32_FAMILY))  # STM32L0
    ifneq (,$(filter $(STM32_MODEL), 010))
      ifneq (,$(filter $(STM32_ROMSIZE), 4))
        CPU_LINE = STM32L$(STM32_MODEL)x4
      else ifneq (,$(filter $(STM32_ROMSIZE), 6))
        CPU_LINE = STM32L$(STM32_MODEL)x6
      else ifneq (,$(filter $(STM32_ROMSIZE), 8))
        CPU_LINE = STM32L$(STM32_MODEL)x8
      else ifneq (,$(filter $(STM32_ROMSIZE), B))
        CPU_LINE = STM32L$(STM32_MODEL)xB
      endif
    else
      CPU_LINE = STM32L$(STM32_MODEL)xx
    endif
  endif
  ifeq (1,$(STM32_FAMILY))  # STM32L1
    ifneq (,$(filter $(STM32_MODEL), 151))
      ifneq (,$(filter $(STM32_ROMSIZE), C))
        CPU_LINE = STM32L$(STM32_MODEL)xC
      else ifneq (,$(filter $(STM32_ROMSIZE), B))
        ifneq (,$(filter $(STM32_RAMMOD), _A))
          CPU_LINE = STM32L$(STM32_MODEL)xBA
        else
          CPU_LINE = STM32L$(STM32_MODEL)xB
        endif
      endif
    else ifneq (,$(filter $(STM32_MODEL), 152))
      ifneq (,$(filter $(STM32_ROMSIZE), E))
        CPU_LINE = STM32L$(STM32_MODEL)xE
      endif
    else
      CPU_LINE = STM32L$(STM32_MODEL)xx
    endif
  endif
endif
