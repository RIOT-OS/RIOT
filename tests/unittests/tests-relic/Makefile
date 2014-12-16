MODULE = tests-relic

# -DWORD=32 : 	Specifies the word width of the target system. This is currently not automatically detected so adjusted
# 				to your target platform.

# The following boards are known to fail or have not been tested.
BOARD_BLACKLIST := arduino-mega2560 chronos f4vi1 msb-430 msb-430h msbiot qemu-i386 redbee-econotag stm32f0discovery stm32f3discovery telosb wsn430-v1_3b wsn430-v1_4 z1

# The rest of the parameters are configuration parameters for RELIC described in its documentation.
export RELIC_CONFIG_FLAGS=-DARCH=NONE -DOPSYS=NONE -DQUIET=off -DWORD=32 -DFP_PRIME=255 -DWITH="BN;MD;DV;FP;EP;CP;BC;EC" -DSEED=ZERO

USEPKG += relic

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
