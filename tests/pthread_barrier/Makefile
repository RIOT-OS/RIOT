include ../Makefile.tests_common

# arduino mega2560 uno duemilanove : unknown type name: clockid_t
# jiminy-mega256rfr2: unknown type name: clockid_t
# mega-xplained: unknown type name: clockid_t

# Modules to include.
USEMODULE += pthread
USEMODULE += random
USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

BOARD_BLACKLIST := \
  arduino-duemilanove \
  arduino-mega2560 \
  arduino-uno \
  jiminy-mega256rfr2 \
  mega-xplained \
  waspmote-pro \
  #

# exclude boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := \
  nucleo-f031k6 \
  #

include $(RIOTBASE)/Makefile.include
