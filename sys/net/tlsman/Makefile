SRC := tlsman.c

ifneq (,$(filter tlsman_tinydtls,$(USEMODULE)))
	ifneq (,$(filter sock_udp,$(USEMODULE)))
		SRC += tinydtls_sock.c
	endif
endif

include $(RIOTBASE)/Makefile.base
