APPLICATION = external_board
RIOTBASE ?= $(CURDIR)/../../

# Only support this board
# No need for a `WHITELIST` as there is only one board in `external_boards`.
#
# HACK I named the external board as 'native' to be in murdock test path for
# 'native'
# In practice it should be something else
BOARD ?= native

# Require arch_native feature so this is not compiled for other boards in
# $(RIOTBOARD)/
FEATURES_REQUIRED += arch_native

# Set without '?=' to also verify the docker integration when set with =
BOARDSDIR = $(CURDIR)/external_boards

include $(RIOTBASE)/Makefile.include
