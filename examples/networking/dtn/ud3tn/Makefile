APPLICATION = ud3tn

BOARD ?= native
RIOTBASE ?= $(CURDIR)/../../../..

USEPKG += ud3tn

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

# Select NODE_A, NODE_B, or NODE_C and adapt the peer addresses in main.c.
CFLAGS += -DNODE_A
CFLAGS += -DISR_STACKSIZE=2048
CFLAGS += -DCONFIG_GNRC_TCP_RCV_BUFFERS=8
CFLAGS += -DCONFIG_GNRC_TCP_MSL_MS=2000

WERROR ?= 0
DEVELHELP=1

ifeq ($(BOARD),esp32-wroom-32)
  CFLAGS += -DESP_WIFI_SSID=\"\"
  CFLAGS += -DESP_WIFI_PASS=\"\"
  USEMODULE += esp_wifi
endif

include $(RIOTBASE)/Makefile.include
