include ../Makefile.tests_common

# The following boards currently miss entropy sources.
BOARD_BLACKLIST :=  6lowpan-clicker \
                    atxmega-a1u-xpro \
                    atxmega-a3bu-xplained \
                    avsextrem \
                    cc1312-launchpad \
                    cc1350-launchpad \
                    cc1352-launchpad \
                    cc1352p-launchpad \
                    cc2650-launchpad \
                    cc2650stk \
                    fox \
                    hifive1 \
                    hifive1b \
                    iotlab-a8-m3 \
                    iotlab-m3 \
                    limifrog-v1 \
                    lobaro-lorabox \
                    maple-mini \
                    mbed_lpc1768 \
                    msb-430 \
                    msb-430h \
                    nucleo-f103rb \
                    nucleo-f446ze \
                    nucleo-g070rb \
                    nucleo-g071rb \
                    nucleo-wl55jc \
                    opencm904 \
                    seeeduino_arch-pro \
                    spark-core \
                    stm32f3discovery \
                    stm32mp157c-dk2 \
                    teensy31 \
                    telosb \
                    z1 \
                    #

USEPKG += mbedtls
USEMODULE += mbedtls_entropy

# Increase stack size of main thread.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE

# You can disable single entropy source to be used with the entropy module
# DISABLE_MODULE += mbedtls_entropy_source_hwrng

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter mbedtls_entropy_source_adc,$(USEMODULE)))
  ifndef CONFIG_KCONFIG_USEMODULE_ENTROPY_SOURCE_ADC_NOISE
    CFLAGS += -DCONFIG_ENTROPY_SOURCE_ADC_HMIN=65536
  endif
endif
