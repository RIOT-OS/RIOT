# name of the application
APPLICATION = sock_tcp_http

# If no BOARD is found in the environment, use this default:
BOARD ?= esp8266-esp-12x

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += xtimer
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += core_idle_thread
# Uncomment this to enable scheduler statistics for ps:
USEMODULE += schedstatistics

# lwIP
USEMODULE += ipv4_addr
USEMODULE += lwip_arp
USEMODULE += lwip_ipv4
USEMODULE += lwip_dhcp_auto
CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
USEMODULE += lwip lwip_netdev
USEMODULE += lwip_udp
USEMODULE += lwip_tcp
USEMODULE += sock_tcp
USEMODULE += netdev_default

# lwip_netdev_mux will need more than the default 1024 bytes
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=2048

CFLAGS += -DMEM_DEBUG=LWIP_DBG_ON
# The javascript queries the sensor data every two seconds but after
# about on minute the requests failed due to memory saturation.
# LwIP uses dynamic memory allocation and after netconn_delete() memory
# is not released until the LAST WAIT timer expires (2 MSF of 60s) so we
# set the MSF to 1 second to prevent memory saturation.
CFLAGS += -DTCP_MSL=1000

# AM2301 (DHT21)
USEMODULE += dht
# Use GPIO2 for ESP01 compatibility
CFLAGS += -DDHT_PARAM_PIN=2
# TYPE 0: DHT11 ; TYPE 1: DHT21/DHT22
CFLAGS += -DDHT_PARAM_TYPE=1

$(shell $(CURDIR)/make_index_html_h)

include $(RIOTBASE)/Makefile.include
