BOARD ?= sltb001a
include ../Makefile.cpu_common

# this test application is expected to build for every EFM32-based board
FEATURES_REQUIRED += cpu_efm32

# the EFM32_* variables are defined by cpu/efm32/efm32-info.mk, which is only
# included further down. CFLAGS is a recursively expanded variable, so the
# values below are resolved when the compiler is invoked
CFLAGS += -DTEST_EFM32_SERIES=$(EFM32_SERIES)
CFLAGS += -DTEST_EFM32_ARCHITECTURE='"$(EFM32_ARCHITECTURE)"'
CFLAGS += -DTEST_EFM32_FLASH_START=$(EFM32_FLASH_START)
CFLAGS += -DTEST_EFM32_FLASH_SIZE=$(EFM32_FLASH_SIZE)
CFLAGS += -DTEST_EFM32_SRAM_START=$(EFM32_SRAM_START)
CFLAGS += -DTEST_EFM32_SRAM_SIZE=$(EFM32_SRAM_SIZE)
CFLAGS += -DTEST_EFM32_CRYPTO=$(EFM32_CRYPTO)
CFLAGS += -DTEST_EFM32_TRNG=$(EFM32_TRNG)
CFLAGS += -DTEST_EFM32_RADIO=$(EFM32_RADIO)

include $(RIOTBASE)/Makefile.include
