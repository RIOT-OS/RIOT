/**
* @defgroup config_ieee802154 IEEE802.15.4 configurations
* @brief   Default IEEE802.15.4 values for devices to choose
* @ingroup net_ieee802154
* @ingroup configurations
* @{
*/

/**
* @name    IEEE802.15.4 sub-GHZ channel.
* @{
*/
#ifndef CONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL
#define CONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL   (5U)
#endif
/** @} */

/**
* @name    IEEE802.15.4 channel.
* @{
*/
#ifndef CONFIG_IEEE802154_DEFAULT_CHANNEL
#define CONFIG_IEEE802154_DEFAULT_CHANNEL          (26U)
#endif
/** @} */

/**
* @name    IEEE802.15.4 sub-GHZ page.
* @{
*/
#ifndef CONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE
#define CONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE      (2U)
#endif
/** @} */

/**
* @name    IEEE802.15.4 PAN ID.
* @{
*/
#ifndef CONFIG_IEEE802154_DEFAULT_PANID
#define CONFIG_IEEE802154_DEFAULT_PANID            (0x0023U)
#endif
/** @} */

/**
* @name    IEEE802.15.4 TX power.
* @brief   Tranmission power in dBm
* @{
*/
#ifndef CONFIG_IEEE802154_DEFAULT_TXPOWER
#define CONFIG_IEEE802154_DEFAULT_TXPOWER          (0) /* in dBm */
#endif
/** @} */
/** @} */
