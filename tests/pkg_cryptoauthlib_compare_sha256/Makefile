include ../Makefile.tests_common

# Test fails to build for these boards fails due to
# redefinition of define AES_COUNT in library, which
# is also defined in efm32 header files
BOARD_BLACKLIST := stk3200 stk3600 stk3700

USEPKG += cryptoauthlib
USEMODULE += hashes

include $(RIOTBASE)/Makefile.include
