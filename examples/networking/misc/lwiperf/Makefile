# Set the name of your application:
APPLICATION = lwiperf

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../../

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 to show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += ztimer
USEMODULE += ztimer_msec

USEMODULE += netdev_default

# Enables lwIP stack
LWIP_IPV4 ?= 0
LWIP_IPV6 ?= 1

ifeq (1,$(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_ipv4

  # Uncomment the following two lines to use DHCP. Refer to the README.md for
  # more details.
  #USEMODULE += lwip_dhcp_auto
  #CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
endif

ifeq (1,$(LWIP_IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6
  USEMODULE += lwip_ipv6_autoconfig
endif

USEMODULE += lwip_app_lwiperf

# Enables ifconfig command, which allows IP address configuration
USEMODULE += shell_cmds_default

include $(RIOTBASE)/Makefile.include
