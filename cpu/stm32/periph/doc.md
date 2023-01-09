/**
 * @defgroup        stm32_eth STM32 Peripheral Ethernet Driver
 * @ingroup         drivers_netdev
 * @brief           Driver for the Peripheral STM32 Ethernet block used across
 *                  all families of STM32 MCUs
 *
 * Link Auto Negotiation
 * =====================
 *
 * To enable Link Auto Negotiation, use the (pseudo) module `stm32_eth_auto`
 * by amending your applications `Makefile` as follows:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * # Add the following line to your Makefile
 * USEMODULE += stm32_eth_auto
 *
 * # Note: Any addition of modules to USEMODULE must be done prior to the
 * # following line:
 * include $(RIOTBASE)/Makefile.include
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * In general, it is highly recommended to use auto-negotiation, as this can
 * avoid various communication issues on the PHY layer due to configuration
 * mismatch of the link partners. Note that this feature depends on the link
 * state events feature.
 *
 * Link State Events
 * =================
 *
 * To enable Link Events, use the (pseudo) module `stm32_eth_link_up` (as
 * described above).
 */
