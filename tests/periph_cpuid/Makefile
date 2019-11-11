include ../Makefile.tests_common

FEATURES_REQUIRED = periph_cpuid

include $(RIOTBASE)/Makefile.include

# Determine the CPUID_LEN based on the current architecture or CPU used in the
# build system.
ifneq (,$(filter arch_esp32,$(FEATURES_USED)))
  CPUID_LEN = 7
endif
ifneq (,$(filter cc2538 efm32 ezr32wg nrf5%,$(CPU)))
  CPUID_LEN = 8
endif
ifneq (,$(filter cc26x0 lpc1768 sam%,$(CPU)))
  CPUID_LEN = 16
endif
ifneq (,$(filter fe310 kinetis stm32%,$(CPU)))
  CPUID_LEN = 12
endif

# Use 4 as default as it is the minimum for all supported CPUs.
CPUID_LEN ?= 4

# Export CPUID_LEN env variable to the Python script.
export CPUID_LEN
