# Include family-specific CPU information. This file contains one line per
# CPU_MODEL that includes additional information, such as flash and memory
# sized and supported features. This does not respect the current
# NOTE: here we use $(CPU_FAM) to parse the directory since it is used only
# to load a 'database' or set a of `FEATURES`, where the file location cant
# be obtained from the CPU and CPU_MODEL
include $(RIOTCPU)/efm32/families/$(CPU_FAM)/efm32-info.mk

EFM32_INFO = $(EFM32_INFO_$(CPU_MODEL))

ifeq (,$(EFM32_INFO))
  $(error Family-specific information for $(CPU_MODEL) is missing)
endif

# Parse the information into EFM32 specific variables. They are specific to the
# EFM32 CPU only, and should not be used outside this module.
EFM32_SERIES       = $(word 1, $(EFM32_INFO))
EFM32_ARCHITECTURE = $(word 2, $(EFM32_INFO))

EFM32_FLASH_START  = $(word 3, $(EFM32_INFO))
EFM32_FLASH_SIZE   = $(word 4, $(EFM32_INFO))
EFM32_SRAM_START   = $(word 5, $(EFM32_INFO))
EFM32_SRAM_SIZE    = $(word 6, $(EFM32_INFO))

EFM32_CRYPTO       = $(word 7, $(EFM32_INFO))
EFM32_TRNG         = $(word 8, $(EFM32_INFO))
EFM32_RADIO        = $(word 9, $(EFM32_INFO))
