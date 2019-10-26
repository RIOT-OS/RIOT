include ../Makefile.tests_common


USEMODULE += embunit

FEATURES_REQUIRED += cpp

DISABLE_MODULE += auto_init

# HACK were disabled for `armv7` in unittests and fail to pass the test
BOARD_BLACKLIST += \
  avsextrem \
  msba2 \
  #

include $(RIOTBASE)/Makefile.include
