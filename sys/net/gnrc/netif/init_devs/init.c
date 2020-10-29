/**
 * Auto initialization for network devices
 *
 * Copyright (C) 2020 Freie Universität Berlin
 *               2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_auto_init
 * @{
 * @file
 * @brief   initializes any used network interface that has a trivial init function
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include "kernel_defines.h"

/**
 * @brief   Initializes network devices
 */
void gnrc_netif_init_devs(void)
{
    if (IS_USED(MODULE_STM32_ETH)) {
        extern void auto_init_stm32_eth(void);
        auto_init_stm32_eth();
    }

    if (IS_USED(MODULE_AUTO_INIT_AT86RF215)) {
        extern void auto_init_at86rf215(void);
        auto_init_at86rf215();
    }

    if (IS_USED(MODULE_AUTO_INIT_AT86RF2XX)) {
        extern void auto_init_at86rf2xx(void);
        auto_init_at86rf2xx();
    }

    if (IS_USED(MODULE_ATWINC15X0)) {
        extern void auto_init_atwinc15x0(void);
        auto_init_atwinc15x0();
    }

    if (IS_USED(MODULE_MRF24J40)) {
        extern void auto_init_mrf24j40(void);
        auto_init_mrf24j40();
    }

    if (IS_USED(MODULE_CC110X)) {
        extern void auto_init_cc110x(void);
        auto_init_cc110x();
    }

    if (IS_USED(MODULE_CC2420)) {
        extern void auto_init_cc2420(void);
        auto_init_cc2420();
    }

    if (IS_USED(MODULE_ENCX24J600)) {
        extern void auto_init_encx24j600(void);
        auto_init_encx24j600();
    }

    if (IS_USED(MODULE_ENC28J60)) {
        extern void auto_init_enc28j60(void);
        auto_init_enc28j60();
    }

    if (IS_USED(MODULE_ESP_ETH)) {
        extern void auto_init_esp_eth(void);
        auto_init_esp_eth();
    }

    /* don't change the order of auto_init_esp_now and auto_init_esp_wifi */
    if (IS_USED(MODULE_ESP_NOW)) {
        extern void auto_init_esp_now(void);
        auto_init_esp_now();
    }

    /* don't change the order of auto_init_esp_now and auto_init_esp_wifi */
    if (IS_USED(MODULE_ESP_WIFI)) {
        extern void auto_init_esp_wifi(void);
        auto_init_esp_wifi();
    }

    if (IS_USED(MODULE_ETHOS)) {
        extern void auto_init_ethos(void);
        auto_init_ethos();
    }

    if (IS_USED(MODULE_DOSE)) {
        extern void auto_init_dose(void);
        auto_init_dose();
    }

    if (IS_USED(MODULE_SLIPDEV)) {
        extern void auto_init_slipdev(void);
        auto_init_slipdev();
    }

    if (IS_USED(MODULE_CC2538_RF)) {
        extern void auto_init_cc2538_rf(void);
        auto_init_cc2538_rf();
    }

    if (IS_USED(MODULE_XBEE)) {
        extern void auto_init_xbee(void);
        auto_init_xbee();
    }

    if (IS_USED(MODULE_KW2XRF)) {
        extern void auto_init_kw2xrf(void);
        auto_init_kw2xrf();
    }

    if (IS_USED(MODULE_KW41ZRF)) {
        extern void auto_init_kw41zrf(void);
        auto_init_kw41zrf();
    }

    if (IS_USED(MODULE_USBUS_CDC_ECM)) {
        extern void auto_init_netdev_cdcecm(void);
        auto_init_netdev_cdcecm();
    }

    if (IS_USED(MODULE_NETDEV_TAP)) {
        extern void auto_init_netdev_tap(void);
        auto_init_netdev_tap();
    }

    if (IS_USED(MODULE_SOCKET_ZEP)) {
        extern void auto_init_socket_zep(void);
        auto_init_socket_zep();
    }

    if (IS_USED(MODULE_NRFMIN)) {
        extern void gnrc_nrfmin_init(void);
        gnrc_nrfmin_init();
    }

    if (IS_USED(MODULE_W5100)) {
        extern void auto_init_w5100(void);
        auto_init_w5100();
    }

    if (IS_USED(MODULE_SX127X) && !IS_USED(MODULE_SEMTECH_LORAMAC)) {
        extern void auto_init_sx127x(void);
        auto_init_sx127x();
    }

    if (IS_USED(MODULE_NRF802154)) {
        extern void auto_init_nrf802154(void);
        auto_init_nrf802154();
    }
}
