
MODULE_DEFS_NEEDED += \
  dns_cache \
  gcoap_dns_proxied \
  gcoap_dtls \
  gcoap_forward_proxy \
  gcoap_forward_proxy_thread \
  nanocoap_cache \
  nanocoap_resources \
  sock_dns \
  #
SRC := gcoap.c

SUBMODULES := 1

# Since gcoap extends nanocoap, it shall be provided with nanocoap internals
INCLUDES += -I$(RIOTBASE)/sys/net/application_layer/nanocoap

include $(RIOTBASE)/Makefile.base
