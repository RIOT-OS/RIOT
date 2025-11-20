.PHONY: riotgen-installed _generate-%

RIOTMAKE=$(RIOTBASE)/makefiles

-include $(RIOTMAKE)/utils/ansi.mk
-include $(RIOTMAKE)/color.inc.mk

riotgen-installed:
	@command -v riotgen > /dev/null 2>&1 || \
	{ $(COLOR_ECHO) \
	"$(COLOR_RED)'riotgen' command is not available.\n\
	Please consider installing it with 'pip install riotgen' or download \
	it from https://pypi.python.org/pypi/riotgen.$(COLOR_RESET)"; \
	exit 1; }

GENERATORS = board driver example module pkg test
.SECONDEXPANSION:
$(addprefix generate-,$(GENERATORS)): $$(addprefix _,$$@)

_generate-%: riotgen-installed
	@RIOTBASE=$(CURDIR) riotgen $* -i
