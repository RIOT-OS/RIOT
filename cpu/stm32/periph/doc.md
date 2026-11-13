@defgroup       stm32_eth STM32 Peripheral Ethernet Driver
@ingroup        drivers_netdev
@brief          Driver for the Peripheral STM32 Ethernet block used across
                all families of STM32 MCUs

Link Autonegotiation
=====================

To enable Link Autonegotiation, use the generic (pseudo) module
`netdev_eth_autoneg`, which selects the `stm32_eth_auto` implementation of this
driver automatically, by amending your applications `Makefile` as follows:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Add the following line to your Makefile
USEMODULE += netdev_eth_autoneg

# Note: Any addition of modules to USEMODULE must be done prior to the
# following line:
include $(RIOTBASE)/Makefile.include
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In general, it is highly recommended to use auto-negotiation, as this can
avoid various communication issues on the PHY layer due to configuration
mismatch of the link partners. Note that this feature depends on the link
state monitoring feature. To force the `stm32_eth` specific implementation
instead of the generic one, use the (pseudo) module `stm32_eth_auto` directly.

Link State Monitoring
=====================

To monitor the actual state of the link, use the generic (pseudo) module
`netdev_eth_link_state` (as described above), which selects the
`stm32_eth_link_up` implementation of this driver automatically. Without this
module, the driver assumes the link is up and signals `NETDEV_EVENT_LINK_UP`
once during initialization, and never signals `NETDEV_EVENT_LINK_DOWN`. To
force the `stm32_eth` specific implementation instead of the generic one, use
the (pseudo) module `stm32_eth_link_up` directly.
