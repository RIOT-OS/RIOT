COLOR_GREEN  :=
COLOR_RED    :=
COLOR_PURPLE :=
COLOR_RESET  :=
COLOR_ECHO   := /bin/echo

ifeq (0,  $(shell tput colors 2>&1 > /dev/null; echo $$?))
  COLOR_GREEN  := \033[1;32m
  COLOR_RED    := \033[1;31m
  COLOR_YELLOW := \033[1;33m
  COLOR_PURPLE := \033[1;35m
  COLOR_RESET  := \033[0m
  ifeq ($(OS),Darwin)
    COLOR_ECHO   := echo -e
    SHELL=bash
  else
    COLOR_ECHO   := /bin/echo -e
  endif
endif
