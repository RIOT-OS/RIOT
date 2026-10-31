include ../Makefile.drivers_common

# Transport to use: tcp, uart or dummy
ESCPOS_TRANSPORT ?= tcp

USEMODULE += epson_escpos_$(ESCPOS_TRANSPORT)
USEMODULE += shell
USEMODULE += shell_cmds_default

ifeq (tcp,$(ESCPOS_TRANSPORT))
  # Epson network printers only support IPv4
  USEMODULE += netdev_default
  USEMODULE += lwip_ipv4
  USEMODULE += lwip_arp
  USEMODULE += lwip_sock_tcp

  # IP address (and optionally port) of the printer
  ESCPOS_HOST ?= 192.168.178.59
  CFLAGS += -DEPSON_ESCPOS_PARAM_TCP_HOST=\"$(ESCPOS_HOST)\"

  # Use DHCP if set to 1, otherwise the static address below is configured
  ESCPOS_DHCP ?= 0
  ifeq (1,$(ESCPOS_DHCP))
    USEMODULE += lwip_dhcp_auto
  else
    ESCPOS_IPV4_ADDR ?= 10.42.0.2
    ESCPOS_IPV4_NETMASK ?= 255.255.255.0
    ESCPOS_IPV4_GW ?= 10.42.0.1
    CFLAGS += -DESCPOS_IPV4_ADDR=\"$(ESCPOS_IPV4_ADDR)\"
    CFLAGS += -DESCPOS_IPV4_NETMASK=\"$(ESCPOS_IPV4_NETMASK)\"
    CFLAGS += -DESCPOS_IPV4_GW=\"$(ESCPOS_IPV4_GW)\"
  endif
endif

include $(RIOTBASE)/Makefile.include
