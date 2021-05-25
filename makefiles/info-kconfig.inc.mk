.PHONY: info-kconfig-board-supported-% info-kconfig-boards-supported

ifneq (1,$(TEST_KCONFIG))

info-kconfig-boards-supported:
	@echo "To run $@ please set TEST_KCONFIG=1"

info-kconfig-board-supported-%:
	@echo "To run $@ please set TEST_KCONFIG=1"

else

# This is the root Kconfig
KCONFIG ?= $(RIOTBASE)/Kconfig

# Include tools targets
include $(RIOTMAKE)/tools/kconfiglib.inc.mk

# This file will contain the calculated dependencies formated in Kconfig
export KCONFIG_GENERATED_DEPENDENCIES

# This file will contain external module configurations
export KCONFIG_EXTERNAL_CONFIGS

# replicate the QUIET logic as in Makefile.include
QUIET ?= 1
QQ=
ifeq ($(QUIET),1)
  Q=@
else
  Q=
endif

# Known configuration sources
KCONFIG_APP_CONFIG = $(APPDIR)/app.config
KCONFIG_USER_CONFIG = $(APPDIR)/user.config

# Similarly to `info-global.inc.mk` we gather certain variables of all riot boards, needed to run
# the kconfig script in `info-kconfig-board-supported-%`. Namely:
# - CPU
# - Configuration sources (.config files)
# - BOARDDIR
#
define gather_board_variables
  BOARD := $(1)

  # Undefine variables that must not be defined when starting.
  # Some are sometime set as `?=`
  undefine CPU
  undefine CPU_MODEL
  undefine CPU_CORE
  undefine CPU_FAM
  undefine CPU_ARCH
  undefine KCONFIG_ADD_CONFIG

  # Find matching board folder
  BOARDDIR := $$(word 1,$$(foreach dir,$$(BOARDSDIRS),$$(wildcard $$(dir)/$$(BOARD)/.)))

  include $(RIOTBASE)/Makefile.features

  $$(BOARD)_CONFIG_SOURCES := $$(KCONFIG_ADD_CONFIG)
  $$(BOARD)_CONFIG_SOURCES += $$(wildcard $(KCONFIG_APP_CONFIG))
  $$(BOARD)_CONFIG_SOURCES += $$(wildcard $(KCONFIG_APP_CONFIG).$$(BOARD))
  $$(BOARD)_CONFIG_SOURCES += $$(wildcard $(KCONFIG_USER_CONFIG))
  $$(BOARD)_CPU := $$(CPU)
  $$(BOARD)_BOARDDIR := $$(BOARDDIR)

endef

# Only gather all boards variables if `info-kconfig-boards-supported` is called, otherwise just get
# the variables of the requested boards
ifneq (,$(filter info-kconfig-boards-supported,$(MAKECMDGOALS)))
  $(foreach board, $(BOARDS), $(eval $(call gather_board_variables,$(board))))
else
  _QUERIES := $(foreach q, $(filter info-kconfig-board-supported-%,$(MAKECMDGOALS)), $(q))
  _QUERIED_BOARDS := $(patsubst info-kconfig-board-supported-%,%,$(_QUERIES))
  $(foreach board, $(_QUERIED_BOARDS), $(eval $(call gather_board_variables,$(board))))
endif

info-kconfig-boards-supported: $(foreach board, $(BOARDS), info-kconfig-board-supported-$(board))

# Determines if a board is supported for the current application, given the application dependencies
# and/or configurations.
# Firstly we check if the board exists in the board list. Then we call the Kconfig script with the
# application directory, board and configuration sources that should be merged. The script will
# check if the configurations could be applied.
#
# If the board is supported the board's name is printed, otherwise there is no output.
#
info-kconfig-board-supported-%:
	$(Q)$(if $(filter $*,$(BOARDS)), true, false) && \
	  BOARD=$* CPU=$($*_CPU) APPDIR=$(APPDIR) BOARDDIR=$($*_BOARDDIR) RIOTBOARD=$(RIOTBOARD) \
	  RIOTCPU=$(RIOTCPU) KCONFIG_GENERATED_DEPENDENCIES="none" KCONFIG_EXTERNAL_CONFIGS="none" \
	  $(GENCONFIG) \
	  --kconfig-filename $(KCONFIG) $(if $(Q),,--debug )\
	  --config-sources $($*_CONFIG_SOURCES) $(if $(Q),2> /dev/null) && \
	  printf "%s " $* || true

endif # TEST_KCONFIG
