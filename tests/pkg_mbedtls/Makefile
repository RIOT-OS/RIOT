include ../Makefile.tests_common

USEPKG += mbedtls
USEMODULE += mbedtls_entropy

# Increase stack size of main thread.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE

# You can disable single entropy source to be used with the entropy module
# DISABLE_MODULE += mbedtls_entropy_source_hwrng

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter mbedtls_entropy_source_adc,$(USEMODULE)))
  ifndef CONFIG_KCONFIG_USEMODULE_ENTROPY_SOURCE_ADC_NOISE
    # This is an exemplary entropy value of 1 bit/sample provided by
    # the ADC noise source. To avoid float, this value has to be
    # multiplied by 65536 beforehand.
    CFLAGS += -DCONFIG_ENTROPY_SOURCE_ADC_HMIN=65536
  endif
endif
