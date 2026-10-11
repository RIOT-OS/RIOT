MODULE = ieee802154

SRC = \
    ieee802154.c \
    ieee802154_timings.c \
    #

ifneq (,$(filter ieee802154_security,$(USEMODULE)))
	SRC += security.c
endif

ifneq (,$(filter ieee802154_submac,$(USEMODULE)))
	SRC += submac.c
endif

include $(RIOTBASE)/Makefile.base
