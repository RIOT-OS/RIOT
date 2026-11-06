# ANSI Terminal codes and other escape sequences.
# The objective of this definitions is to be able to write special characters
# without resorting to shell commands like echo.

include $(RIOTMAKE)/utils/ansi_special.mk

ANSI_GREEN  := \033[1;32m
ANSI_RED    := \033[1;31m
ANSI_YELLOW := \033[1;33m
ANSI_PURPLE := \033[1;35m
ANSI_RESET  := \033[0m
