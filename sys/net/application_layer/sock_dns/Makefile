SRC += dns.c

ifneq (,$(filter sock_dns_cache, $(USEMODULE)))
  SRC += dns_cache.c
endif

include $(RIOTBASE)/Makefile.base
