# name of your application
APPLICATION = lwm2m

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_router_default
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# NOTE: Add the package for wakaama
USEPKG += wakaama
USEMODULE += wakaama_objects_light_control
USEMODULE += wakaama_objects_on_off_switch

# add DTLS support
USEMODULE += wakaama_client_dtls

# Specify sock DTLS implementation
USEPKG += tinydtls
# tinydtls needs crypto secure PRNG
USEMODULE += prng_sha1prng

## Application-specific Configuration options
LWM2M_SERVER_URI ?= '"coaps://[fd00:dead:beef::1]:5684"'
LWM2M_SERVER_SHORT_ID ?= 1

CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE='(4*1024)'

include $(RIOTBASE)/Makefile.include

DTLS_MAX_BUF=1024

# Configure via CFLAGS only if not done via Kconfig
ifndef CONFIG_LWM2M_SERVER_URI
  CFLAGS += -DCONFIG_LWM2M_SERVER_URI=$(LWM2M_SERVER_URI)
  CFLAGS += -DCONFIG_LWM2M_SERVER_SHORT_ID=$(LWM2M_SERVER_SHORT_ID)
  CFLAGS += -DCONFIG_DTLS_PEER_MAX=2
  CFLAGS += -DCONFIG_MAX_BUF=1024
  # Uncomment to enable Wakaama debug log
  #CFLAGS += -DCONFIG_LWM2M_WITH_LOGS=1
endif
