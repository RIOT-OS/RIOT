include ../Makefile.pkg_common

# The following boards are known to fail or have not been tested.
BOARD_BLACKLIST :=  arduino-duemilanove \
                    arduino-leonardo \
                    arduino-mega2560 \
                    arduino-nano \
                    arduino-uno \
                    atmega256rfr2-xpro \
                    atmega328p \
                    atmega328p-xplained-mini \
                    atxmega-a1-xplained \
                    atxmega-a1u-xpro \
                    atxmega-a3bu-xplained \
                    derfmega128 \
                    derfmega256 \
                    f4vi1 \
                    hifive1 \
                    hifive1b \
                    mega-xplained \
                    microduino-corerf \
                    msb-430 \
                    msb-430h \
                    msbiot \
                    redbee-econotag \
                    stm32f0discovery \
                    stm32f3discovery \
                    telosb \
                    z1 \
                    #

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(6*THREAD_STACKSIZE_DEFAULT\)

USEPKG += relic
USEMODULE += embunit

ifeq (llvm,$(TOOLCHAIN))
  # FIXME: automated test is failing on samr21-xpro
  TEST_ON_CI_BLACKLIST += samr21-xpro
endif

# -DWSIZE=32 : 	Specifies the word width of the target system. This is
#  				currently not automatically detected so adjusted to your target
#  				platform.

# The rest of the parameters are configuration parameters for RELIC described in its documentation.
export RELIC_CONFIG_FLAGS=-DARCH=NONE -DOPSYS=NONE -DQUIET=off -DWSIZE=32 -DFP_PRIME=255 -DWITH="BN;MD;DV;FP;EP;CP;BC;EC" -DSEED=RIOTRND

include $(RIOTBASE)/Makefile.include
