MODULE = ieee802154

SRC = \
    ieee802154.c \
    #

ifneq (,$(filter ieee802154_security,$(USEMODULE)))
	SRC += security.c
endif

ifneq (,$(filter ieee802154_submac,$(USEMODULE)))
	SRC += submac.c
	SRC += ieee802154_timings.c
endif

include $(RIOTBASE)/Makefile.base
