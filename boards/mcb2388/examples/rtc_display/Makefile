APPLICATION = rtc_display
BOARD_WHITELIST = mcb2388

BOARD ?= mcb2388

# This has to be the absolute path to the RIOT base directory
RIOTBASE ?= $(CURDIR)/../../../..

# modules required for the RTC display application
FEATURES_REQUIRED += periph_rtc
FEATURES_REQUIRED += periph_adc
USEMODULE += hd44780

# modules needed for the shell
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

include $(RIOTBASE)/Makefile.include
