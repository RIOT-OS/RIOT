# Find the header file that should exist if the CPU is supported.
EFM32_HEADER = $(wildcard $(RIOTCPU)/efm32/families/*/include/vendor/$(CPU_MODEL).h)

ifeq (,$(EFM32_HEADER))
  $(error Header file for $(CPU_MODEL) is missing)
endif

# Lookup up CPU information using grep.
EFM32_INFO = $(shell grep $(CPU_MODEL) $(shell dirname $(EFM32_HEADER))/../../cpus.txt)

ifeq (,$(EFM32_INFO))
  $(error Unable to read CPU information for $(CPU_MODEL))
endif

# Export variables to use in this build.
export EFM32_FAMILY = $(word 2, $(EFM32_INFO))
export EFM32_SERIES = $(word 3, $(EFM32_INFO))
export EFM32_ARCHITECTURE = $(word 4, $(EFM32_INFO))

export EFM32_FLASH_START = $(word 5, $(EFM32_INFO))
export EFM32_FLASH_SIZE = $(word 6, $(EFM32_INFO))
export EFM32_SRAM_START = $(word 7, $(EFM32_INFO))
export EFM32_SRAM_SIZE = $(word 8, $(EFM32_INFO))

export EFM32_CRYPTO = $(word 9, $(EFM32_INFO))
export EFM32_TRNG = $(word 10, $(EFM32_INFO))
export EFM32_RADIO = $(word 11, $(EFM32_INFO))
