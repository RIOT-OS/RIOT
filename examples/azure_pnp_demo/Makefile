APPLICATION = azure_pnp_demo

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

WIFI_SSID ?= "Your_WiFi_name"
WIFI_PASS ?= "Your_secure_password"

# USEMODULE += mqtts_riot_iface

CFLAGS += -DDEFAULT_CERT_DIR_CA=\"$(CURDIR)/certificates/digiCert_ca_azure.pem\"
CFLAGS += -DDEFAULT_CERT_DIR_DEVICE_CERT_CHAIN=\"$(CURDIR)/certificates/az-riot-pnp-module-test-02-full-chain.cert.pem\"
CFLAGS += -DDEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY=\"$(CURDIR)/certificates/az-riot-pnp-module-test-02.key.pem\"

CFLAGS += -DAZ_RIOT_DPS_REGISTRATION_ID=\"az-riot-pnp-module-test-02\"
CFLAGS += -DAZ_RIOT_DPS_IDSCOPE=\"0ne00B1BDDA\"

CFLAGS += -DDAZ_RIOT_HUB_HOST=\"TRHN-HUB.azure-devices.net\"
CFLAGS += -DAZ_RIOT_HUB_DEVICEID=\"az-riot-pnp-module-test-02\"

# Optionally include remoteDNS support. This includes resolution of names at an
# upstream DNS server and the handling of RDNSS options in Router Advertisements
# to auto-configure that upstream DNS server.
# USEMODULE += sock_dns              # include DNS client
# USEMODULE += gnrc_ipv6_nib_dns     # include RDNSS option handling


USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netdev_default


EXTERNAL_MODULE_DIRS += $(RIOTBASE)/modules
USEMODULE += az_riot_pnp_iface


# #### dns
USEMODULE += sock_dns
USEMODULE += auto_init_sock_dns
# USEMODULE += gnrc_ipv6_nib_dns 
CFLAGS += -DCONFIG_AUTO_INIT_SOCK_DNS_SERVER_ADDR=\"fd12:dead:beef::1\"
# #### dns
CFLAGS += -DCONFIG_DNS_MSG_LEN=256

CFLAGS += -Wno-error=unused-parameter
CFLAGS += -Wno-error=unused-value
CFLAGS += -Wno-error=unused-variable

CFLAGS += -Wno-strict-prototypes

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter arch_esp,$(FEATURES_USED)))
  CFLAGS += -DWIFI_SSID=\"$(WIFI_SSID)\"
  CFLAGS += -DWIFI_PASS=\"$(WIFI_PASS)\"
endif
