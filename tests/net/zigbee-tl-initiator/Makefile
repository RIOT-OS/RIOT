include ../Makefile.net_common

BOARD_WHITELIST = native32 native64   # socket_zep is only available on native

USEMODULE += shell
USEMODULE += luid
USEMODULE += socket_zep
USEPKG += zboss

CFLAGS += -DZB_ED_ROLE

include $(RIOTBASE)/Makefile.include
