/**
 * Auto initialization for used modules
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
 * @brief   initializes any used module that has a trivial init function
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 * @}
 */
#include <stdint.h>
#include <stdio.h>

#include "auto_init.h"
#include "kernel_defines.h"
#include "log.h"

/**
 * @brief   Initializes network devices
 */
static void _auto_init_gnrc_netif(void)
{
    if (IS_USED(MODULE_STM32_ETH)) {
        extern void auto_init_stm32_eth(void);
        auto_init_stm32_eth();
    }

    if (IS_USED(MODULE_AUTO_INIT_AT86RF2XX)) {
        extern void auto_init_at86rf2xx(void);
        auto_init_at86rf2xx();
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

    if (IS_USED(MODULE_NORDIC_SOFTDEVICE_BLE)) {
        extern void gnrc_nordic_ble_6lowpan_init(void);
        gnrc_nordic_ble_6lowpan_init();
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

/**
 * @brief   Initializes sensors and actuators for SAUL
 */
void _auto_init_saul(void)
{
    if (IS_USED(MODULE_SAUL_ADC)) {
        extern void auto_init_adc(void);
        auto_init_adc();
    }
    if (IS_USED(MODULE_SAUL_GPIO)) {
        extern void auto_init_gpio(void);
        auto_init_gpio();
    }
    if (IS_USED(MODULE_SAUL_NRF_TEMPERATURE)) {
        extern void auto_init_nrf_temperature(void);
        auto_init_nrf_temperature();
    }
    if (IS_USED(MODULE_AD7746)) {
        extern void auto_init_ad7746(void);
        auto_init_ad7746();
    }
    if (IS_USED(MODULE_ADCXX1C)) {
        extern void auto_init_adcxx1c(void);
        auto_init_adcxx1c();
    }
    if (IS_USED(MODULE_ADS101X)) {
        extern void auto_init_ads101x(void);
        auto_init_ads101x();
    }
    if (IS_USED(MODULE_ADXL345)) {
        extern void auto_init_adxl345(void);
        auto_init_adxl345();
    }
    if (IS_USED(MODULE_BMP180)) {
        extern void auto_init_bmp180(void);
        auto_init_bmp180();
    }
    if (IS_USED(MODULE_BMX280)) {
        extern void auto_init_bmx280(void);
        auto_init_bmx280();
    }
    if (IS_USED(MODULE_BMX055)) {
        extern void auto_init_bmx055(void);
        auto_init_bmx055();
    }
    if (IS_USED(MODULE_CCS811)) {
        extern void auto_init_ccs811(void);
        auto_init_ccs811();
    }
    if (IS_USED(MODULE_DHT)) {
        extern void auto_init_dht(void);
        auto_init_dht();
    }
    if (IS_USED(MODULE_DS18)) {
        extern void auto_init_ds18(void);
        auto_init_ds18();
    }
    if (IS_USED(MODULE_DS75LX)) {
        extern void auto_init_ds75lx(void);
        auto_init_ds75lx();
    }
    if (IS_USED(MODULE_FXOS8700)) {
        extern void auto_init_fxos8700(void);
        auto_init_fxos8700();
    }
    if (IS_USED(MODULE_GROVE_LEDBAR)) {
        extern void auto_init_grove_ledbar(void);
        auto_init_grove_ledbar();
    }
    if (IS_USED(MODULE_HDC1000)) {
        extern void auto_init_hdc1000(void);
        auto_init_hdc1000();
    }
    if (IS_USED(MODULE_HTS221)) {
        extern void auto_init_hts221(void);
        auto_init_hts221();
    }
    if (IS_USED(MODULE_INA2XX)) {
        extern void auto_init_ina2xx(void);
        auto_init_ina2xx();
    }
    if (IS_USED(MODULE_INA3221)) {
        extern void auto_init_ina3221(void);
        auto_init_ina3221();
    }
    if (IS_USED(MODULE_IO1_XPLAINED)) {
        extern void auto_init_io1_xplained(void);
        auto_init_io1_xplained();
    }
    if (IS_USED(MODULE_ISL29020)) {
        extern void auto_init_isl29020(void);
        auto_init_isl29020();
    }
    if (IS_USED(MODULE_ITG320X)) {
        extern void auto_init_itg320x(void);
        auto_init_itg320x();
    }
    if (IS_USED(MODULE_JC42)) {
        extern void auto_init_jc42(void);
        auto_init_jc42();
    }
    if (IS_USED(MODULE_L3G4200D)) {
        extern void auto_init_l3g4200d(void);
        auto_init_l3g4200d();
    }
    if (IS_USED(MODULE_LIS2DH12)) {
        extern void auto_init_lis2dh12(void);
        auto_init_lis2dh12();
    }
    if (IS_USED(MODULE_LIS3DH)) {
        extern void auto_init_lis3dh(void);
        auto_init_lis3dh();
    }
    if (IS_USED(MODULE_LIS3MDL)) {
        extern void auto_init_lis3mdl(void);
        auto_init_lis3mdl();
    }
    if (IS_USED(MODULE_LPSXXX)) {
        extern void auto_init_lpsxxx(void);
        auto_init_lpsxxx();
    }
    if (IS_USED(MODULE_LSM303DLHC)) {
        extern void auto_init_lsm303dlhc(void);
        auto_init_lsm303dlhc();
    }
    if (IS_USED(MODULE_LSM6DSL)) {
        extern void auto_init_lsm6dsl(void);
        auto_init_lsm6dsl();
    }
    if (IS_USED(MODULE_LTC4150)) {
        extern void auto_init_ltc4150(void);
        auto_init_ltc4150();
    }
    if (IS_USED(MODULE_MAG3110)) {
        extern void auto_init_mag3110(void);
        auto_init_mag3110();
    }
    if (IS_USED(MODULE_MMA7660)) {
        extern void auto_init_mma7660(void);
        auto_init_mma7660();
    }
    if (IS_USED(MODULE_MMA8X5X)) {
        extern void auto_init_mma8x5x(void);
        auto_init_mma8x5x();
    }
    if (IS_USED(MODULE_MPL3115A2)) {
        extern void auto_init_mpl3115a2(void);
        auto_init_mpl3115a2();
    }
    if (IS_USED(MODULE_MPU9X50)) {
        extern void auto_init_mpu9x50(void);
        auto_init_mpu9x50();
    }
    if (IS_USED(MODULE_OPT3001)) {
        extern void auto_init_opt3001(void);
        auto_init_opt3001();
    }
    if (IS_USED(MODULE_PCA9685)) {
        extern void auto_init_pca9685(void);
        auto_init_pca9685();
    }
    if (IS_USED(MODULE_PH_OEM)) {
        extern void auto_init_ph_oem(void);
        auto_init_ph_oem();
    }
    if (IS_USED(MODULE_PIR)) {
        extern void auto_init_pir(void);
        auto_init_pir();
    }
    if (IS_USED(MODULE_PULSE_COUNTER)) {
        extern void auto_init_pulse_counter(void);
        auto_init_pulse_counter();
    }
    if (IS_USED(MODULE_QMC5883L)) {
        extern void auto_init_qmc5883l(void);
        auto_init_qmc5883l();
    }
    if (IS_USED(MODULE_SHT2X)) {
        extern void auto_init_sht2x(void);
        auto_init_sht2x();
    }
    if (IS_USED(MODULE_SHT3X)) {
        extern void auto_init_sht3x(void);
        auto_init_sht3x();
    }
    if (IS_USED(MODULE_SHTC1)) {
        extern void auto_init_shtc1(void);
        auto_init_shtc1();
    }
    if (IS_USED(MODULE_SDS011)) {
        extern void auto_init_sds011(void);
        auto_init_sds011();
    }
    if (IS_USED(MODULE_SI114X)) {
        extern void auto_init_si114x(void);
        auto_init_si114x();
    }
    if (IS_USED(MODULE_SI70XX)) {
        extern void auto_init_si70xx(void);
        auto_init_si70xx();
    }
    if (IS_USED(MODULE_SPS30)) {
        extern void auto_init_sps30(void);
        auto_init_sps30();
    }
    if (IS_USED(MODULE_TCS37727)) {
        extern void auto_init_tcs37727(void);
        auto_init_tcs37727();
    }
    if (IS_USED(MODULE_TMP006)) {
        extern void auto_init_tmp00x(void);
        auto_init_tmp00x();
    }
    if (IS_USED(MODULE_TSL2561)) {
        extern void auto_init_tsl2561(void);
        auto_init_tsl2561();
    }
    if (IS_USED(MODULE_TSL4531X)) {
        extern void auto_init_tsl4531x(void);
        auto_init_tsl4531x();
    }
    if (IS_USED(MODULE_VCNL40X0)) {
        extern void auto_init_vcnl40x0(void);
        auto_init_vcnl40x0();
    }
    if (IS_USED(MODULE_VEML6070)) {
        extern void auto_init_veml6070(void);
        auto_init_veml6070();
    }
}

void auto_init(void)
{
    if (IS_USED(MODULE_AUTO_INIT_RANDOM)) {
        LOG_DEBUG("Auto init random.\n");
        extern void auto_init_random(void);
        auto_init_random();
    }
    if (IS_USED(MODULE_AUTO_INIT_XTIMER)) {
        LOG_DEBUG("Auto init xtimer.\n");
        extern void xtimer_init(void);
        xtimer_init();
    }
    if (IS_USED(MODULE_SCHEDSTATISTICS)) {
        LOG_DEBUG("Auto init schedstatistics.\n");
        extern void init_schedstatistics(void);
        init_schedstatistics();
    }
    if (IS_USED(MODULE_EVENT_THREAD)) {
        LOG_DEBUG("Auto init event threads.\n");
        extern void auto_init_event_thread(void);
        auto_init_event_thread();
    }
    if (IS_USED(MODULE_MCI)) {
        LOG_DEBUG("Auto init mci.\n");
        extern void mci_initialize(void);
        mci_initialize();
    }
    if (IS_USED(MODULE_PROFILING)) {
        LOG_DEBUG("Auto init profiling.\n");
        extern void profiling_init(void);
        profiling_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_PKTBUF)) {
        LOG_DEBUG("Auto init gnrc_pktbuf.\n");
        extern void gnrc_pktbuf_init(void);
        gnrc_pktbuf_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_PKTDUMP)) {
        LOG_DEBUG("Auto init gnrc_pktdump.\n");
        extern void gnrc_pktdump_init(void);
        gnrc_pktdump_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_SIXLOWPAN)) {
        LOG_DEBUG("Auto init gnrc_sixlowpan.\n");
        extern void gnrc_sixlowpan_init(void);
        gnrc_sixlowpan_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_IPV6)) {
        LOG_DEBUG("Auto init gnrc_ipv6.\n");
        extern void gnrc_ipv6_init(void);
        gnrc_ipv6_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_UDP)) {
        LOG_DEBUG("Auto init gnrc_udp.\n");
        extern void gnrc_udp_init(void);
        gnrc_udp_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_TCP)) {
        LOG_DEBUG("Auto init gnrc_tcp.\n");
        extern void gnrc_tcp_init(void);
        gnrc_tcp_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_LWIP)) {
        LOG_DEBUG("Bootstraping lwIP.\n");
        extern void lwip_bootstrap(void);
        lwip_bootstrap();
    }
    if (IS_USED(MODULE_OPENTHREAD)) {
        LOG_DEBUG("Bootstrapping openthread.\n");
        extern void openthread_bootstrap(void);
        openthread_bootstrap();
    }
    if (IS_USED(MODULE_GCOAP) &&
        !IS_ACTIVE(CONFIG_GCOAP_NO_AUTO_INIT)) {
        LOG_DEBUG("Auto init gcoap.\n");
        extern void gcoap_init(void);
        gcoap_init();
    }
    if (IS_USED(MODULE_DEVFS)) {
        LOG_DEBUG("Mounting /dev.\n");
        extern void auto_init_devfs(void);
        auto_init_devfs();
    }
    if (IS_USED(MODULE_AUTO_INIT_GNRC_IPV6_NIB)) {
        LOG_DEBUG("Auto init gnrc_ipv6_nib.\n");
        extern void gnrc_ipv6_nib_init(void);
        gnrc_ipv6_nib_init();
    }
    if (IS_USED(MODULE_SKALD)) {
        LOG_DEBUG("Auto init Skald.\n");
        extern void skald_init(void);
        skald_init();
    }
    if (IS_USED(MODULE_CORD_COMMON)) {
        LOG_DEBUG("Auto init cord_common.\n");
        extern void cord_common_init(void);
        cord_common_init();
    }
    if (IS_USED(MODULE_CORD_EP_STANDALONE)) {
        LOG_DEBUG("Auto init cord_ep_standalone.\n");
        extern void cord_ep_standalone_run(void);
        cord_ep_standalone_run();
    }
    if (IS_USED(MODULE_ASYMCUTE)) {
        LOG_DEBUG("Auto init Asymcute.\n");
        extern void asymcute_handler_run(void);
        asymcute_handler_run();
    }
    if (IS_USED(MODULE_NIMBLE)) {
        LOG_DEBUG("Auto init NimBLE.\n");
        extern void nimble_riot_init(void);
        nimble_riot_init();
    }
    if (IS_USED(MODULE_AUTO_INIT_LORAMAC)) {
        LOG_DEBUG("Auto init loramac.\n");
        extern void auto_init_loramac(void);
        auto_init_loramac();
    }
    if (IS_USED(MODULE_SOCK_DTLS)) {
        LOG_DEBUG("Auto init sock_dtls.\n");
        extern void sock_dtls_init(void);
        sock_dtls_init();
    }

    /* initialize USB devices */
    if (IS_USED(MODULE_AUTO_INIT_USBUS)) {
        LOG_DEBUG("Auto init USB.\n");
        extern void auto_init_usb(void);
        auto_init_usb();
    }

    /* initialize network devices */
    if (IS_USED(MODULE_AUTO_INIT_GNRC_NETIF)) {
        LOG_DEBUG("Auto init gnrc_netif.\n");
        _auto_init_gnrc_netif();
    }

    if (IS_USED(MODULE_AUTO_INIT_GNRC_UHCPC)) {
        LOG_DEBUG("Auto init gnrc_uhcpc.\n");
        extern void auto_init_gnrc_uhcpc(void);
        auto_init_gnrc_uhcpc();
    }

    /* initialize NDN module after the network devices are initialized */
    if (IS_USED(MODULE_NDN_RIOT)) {
        LOG_DEBUG("Auto init NDN.\n");
        extern void ndn_init(void);
        ndn_init();
    }

    /* initialize sensors and actuators */
    if (IS_USED(MODULE_SHT1X)) {
        /* The sht1x module needs to be initialized regardless of SAUL being used,
         * as the shell commands rely on auto-initialization. auto_init_sht1x also
         * performs SAUL registration, but only if module auto_init_saul is used.
         */
        LOG_DEBUG("Auto init sht1x.\n");
        extern void auto_init_sht1x(void);
        auto_init_sht1x();
    }

    if (IS_USED(MODULE_AUTO_INIT_SAUL)) {
        LOG_DEBUG("Auto init SAUL.\n");
        _auto_init_saul();
    }

    if (IS_USED(MODULE_AUTO_INIT_GNRC_RPL)) {
        LOG_DEBUG("Auto init gnrc_rpl.\n");
        extern void auto_init_gnrc_rpl(void);
        auto_init_gnrc_rpl();
    }

    /* initialize storage devices */
    if (IS_USED(MODULE_AUTO_INIT_STORAGE)) {
        LOG_DEBUG("Auto init STORAGE.\n");

        if (IS_USED(MODULE_SDCARD_SPI)) {
            extern void auto_init_sdcard_spi(void);
            auto_init_sdcard_spi();
        }
    }


    if (IS_USED(MODULE_AUTO_INIT_CAN)) {
        LOG_DEBUG("Auto init CAN.\n");

        extern void auto_init_candev(void);
        auto_init_candev();
    }

    if (IS_USED(MODULE_SUIT)) {
        LOG_DEBUG("Auto init SUIT conditions.\n");
        extern void suit_init_conditions(void);
        suit_init_conditions();
    }

    if (IS_USED(MODULE_AUTO_INIT_SECURITY)) {
        if (IS_USED(MODULE_CRYPTOAUTHLIB)) {
            LOG_DEBUG("Auto init cryptoauthlib.\n");
            extern void auto_init_atca(void);
            auto_init_atca();
        }
    }

    if (IS_USED(MODULE_TEST_UTILS_INTERACTIVE_SYNC) &&
        (!IS_USED(MODULE_SHELL_COMMANDS) || !IS_USED(MODULE_SHELL))) {
        extern void test_utils_interactive_sync(void);
        test_utils_interactive_sync();
    }

    if (IS_USED(MODULE_AUTO_INIT_DHCPV6_CLIENT)) {
        LOG_DEBUG("Auto init DHCPv6 client.\n");
        extern void dhcpv6_client_auto_init(void);
        dhcpv6_client_auto_init();
    }

    if (IS_USED(MODULE_GNRC_DHCPV6_CLIENT_6LBR)) {
        LOG_DEBUG("Auto init 6LoWPAN border router DHCPv6 client\n");
        extern void gnrc_dhcpv6_client_6lbr_init(void);
        gnrc_dhcpv6_client_6lbr_init();
    }
}
