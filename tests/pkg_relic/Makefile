include ../Makefile.tests_common

# The following boards are known to fail or have not been tested.
BOARD_BLACKLIST :=  arduino-duemilanove \
                    arduino-mega2560 \
                    arduino-uno \
                    chronos \
                    f4vi1 \
                    hifive1 \
                    jiminy-mega256rfr2 \
                    mega-xplained \
                    msb-430 \
                    msb-430h \
                    msbiot \
                    redbee-econotag \
                    stm32f0discovery \
                    stm32f3discovery \
                    telosb \
                    waspmote-pro \
                    wsn430-v1_3b \
                    wsn430-v1_4 \
                    z1 \
                    #

BOARD_INSUFFICIENT_MEMORY :=    nucleo-f030r8 \
                                nucleo-f031k6 \
                                nucleo-f042k6 \
                                nucleo-l031k6 \
                                nucleo-l053r8 \
                                #

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT\)

USEPKG += relic
USEMODULE += embunit

# -DWORD=32 : 	Specifies the word width of the target system. This is
#  				currently not automatically detected so adjusted to your target
#  				platform.

# The rest of the parameters are configuration parameters for RELIC described in its documentation.
export RELIC_CONFIG_FLAGS=-DARCH=NONE -DOPSYS=NONE -DQUIET=off -DWORD=32 -DFP_PRIME=255 -DWITH="BN;MD;DV;FP;EP;CP;BC;EC" -DSEED=ZERO

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
