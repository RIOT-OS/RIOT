/**
 * Auto initialization for used network interfaces
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 * @file
 * @brief   Auto initialization for GNRC network interfaces
 *
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

void gnrc_netif_init(void)
{
#ifdef MODULE_STM32_ETH
    extern void auto_init_stm32_eth(void);
    auto_init_stm32_eth();
#endif

#ifdef MODULE_AUTO_INIT_AT86RF2XX
    extern void auto_init_at86rf2xx(void);
    auto_init_at86rf2xx();
#endif

#ifdef MODULE_MRF24J40
    extern void auto_init_mrf24j40(void);
    auto_init_mrf24j40();
#endif

#ifdef MODULE_CC110X
    extern void auto_init_cc110x(void);
    auto_init_cc110x();
#endif

#ifdef MODULE_CC2420
    extern void auto_init_cc2420(void);
    auto_init_cc2420();
#endif

#ifdef MODULE_ENCX24J600
    extern void auto_init_encx24j600(void);
    auto_init_encx24j600();
#endif

#ifdef MODULE_ENC28J60
    extern void auto_init_enc28j60(void);
    auto_init_enc28j60();
#endif

#ifdef MODULE_ESP_ETH
    extern void auto_init_esp_eth(void);
    auto_init_esp_eth();
#endif

/* don't change the order of auto_init_esp_now and auto_init_esp_wifi */
#ifdef MODULE_ESP_NOW
    extern void auto_init_esp_now(void);
    auto_init_esp_now();
#endif

/* don't change the order of auto_init_esp_now and auto_init_esp_wifi */
#ifdef MODULE_ESP_WIFI
    extern void auto_init_esp_wifi(void);
    auto_init_esp_wifi();
#endif

#ifdef MODULE_ETHOS
    extern void auto_init_ethos(void);
    auto_init_ethos();
#endif

#ifdef MODULE_DOSE
    extern void auto_init_dose(void);
    auto_init_dose();
#endif

#ifdef MODULE_SLIPDEV
    extern void auto_init_slipdev(void);
    auto_init_slipdev();
#endif

#ifdef MODULE_CC2538_RF
    extern void auto_init_cc2538_rf(void);
    auto_init_cc2538_rf();
#endif

#ifdef MODULE_XBEE
    extern void auto_init_xbee(void);
    auto_init_xbee();
#endif

#ifdef MODULE_KW2XRF
    extern void auto_init_kw2xrf(void);
    auto_init_kw2xrf();
#endif

#ifdef MODULE_USBUS_CDC_ECM
    extern void auto_init_netdev_cdcecm(void);
    auto_init_netdev_cdcecm();
#endif

#ifdef MODULE_NETDEV_TAP
    extern void auto_init_netdev_tap(void);
    auto_init_netdev_tap();
#endif

#ifdef MODULE_SOCKET_ZEP
    extern void auto_init_socket_zep(void);
    auto_init_socket_zep();
#endif

#ifdef MODULE_NORDIC_SOFTDEVICE_BLE
    extern void gnrc_nordic_ble_6lowpan_init(void);
    gnrc_nordic_ble_6lowpan_init();
#endif

#ifdef MODULE_NRFMIN
    extern void gnrc_nrfmin_init(void);
    gnrc_nrfmin_init();
#endif

#ifdef MODULE_W5100
    extern void auto_init_w5100(void);
    auto_init_w5100();
#endif

#if defined(MODULE_SX127X) && !defined(MODULE_SEMTECH_LORAMAC)
    extern void auto_init_sx127x(void);
    auto_init_sx127x();
#endif

#ifdef MODULE_NRF802154
    extern void auto_init_nrf802154(void);
    auto_init_nrf802154();
#endif
}
