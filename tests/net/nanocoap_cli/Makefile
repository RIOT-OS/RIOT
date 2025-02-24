include ../Makefile.net_common

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_default

# Optionally include DNS support. This includes resolution of names at an
# upstream DNS server and the handling of RDNSS options in Router Advertisements
# to auto-configure that upstream DNS server.
# USEMODULE += sock_dns              # include DNS client
# USEMODULE += gnrc_ipv6_nib_dns     # include RDNSS option handling

USEMODULE += nanocoap_sock
USEMODULE += nanocoap_resources
USEMODULE += fmt # scn_buf_hex() for shell cmd client_token

# boards where basic nanocoap functionality fits, but no VFS
LOW_MEMORY_BOARDS := \
  atmega1284p \
  atxmega-a3bu-xplained \
  blackpill-stm32f103c8 \
  bluepill-stm32f103c8 \
  derfmega128 \
  im880b \
  nucleo-f302r8 \
  saml10-xpro \
  saml11-xpro \
  sipeed-longan-nano \
  sipeed-longan-nano-tft \
  spark-core \
  stm32f7508-dk \
  stm32mp157c-dk2 \
  #

# Don't enable VFS, DTLS functions on small boards
ifeq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  USEMODULE += nanocoap_dtls
  USEMODULE += prng_sha256prng

  USEMODULE += nanocoap_vfs
  USEMODULE += vfs_default
  # USEMODULE += vfs_auto_format
  USEMODULE += shell_cmds_default

  # DTLS and VFS operations require more stack on the main thread
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)

  # always enable auto-format for native
  ifneq (,$(filter native native32 native64,$(BOARD)))
    USEMODULE += vfs_auto_format
  endif
endif

# Required by test
USEMODULE += od
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
