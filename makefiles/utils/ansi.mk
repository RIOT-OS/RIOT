# ANSI Terminal codes and other escape sequences.
# The objective of this definitions is to be able to write special characters
# without resorting to shell commands like echo.

include $(RIOTMAKE)/utils/ansi_special.mk

ANSI_GREEN  := $(ANSI_ESC)[1;32m
ANSI_RED    := $(ANSI_ESC)[1;31m
ANSI_YELLOW := $(ANSI_ESC)[1;33m
ANSI_PURPLE := $(ANSI_ESC)[1;35m
ANSI_RESET  := $(ANSI_ESC)[0m
