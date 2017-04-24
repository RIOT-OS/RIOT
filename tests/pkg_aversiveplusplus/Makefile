APPLICATION = pkg_aversiveplusplus
include ../Makefile.tests_common

# temporarily disable for CI
ifeq ($(RIOT_CI_BUILD), 1)
BOARD_WHITELIST := "none"
endif

USEPKG += aversiveplusplus
FEATURES_REQUIRED += cpp
CXXSTD ?= c++11

include $(RIOTBASE)/Makefile.include
