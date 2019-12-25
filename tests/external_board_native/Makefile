APPLICATION = external_board
RIOTBASE ?= $(CURDIR)/../../

# HACK I named the external board as 'native' to be in murdock test path for
# 'native'
# In practice it should be something else
BOARD ?= native

# Only support this board
BOARD_WHITELIST = native

# Set without '?=' to also verify the docker integration when set with =
BOARDSDIR = $(CURDIR)/external_boards

include $(RIOTBASE)/Makefile.include
