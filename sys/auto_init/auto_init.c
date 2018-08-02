/**
 * Auto initialization for used modules
 *
 * Copyright (C) 2013  INRIA.
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
 * @}
 */
#include <stdint.h>
#include <stdio.h>

#include "auto_init.h"

#ifdef MODULE_MCI
#include "diskio.h"
#endif

#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif

#ifdef MODULE_GNRC_SIXLOWPAN
#include "net/gnrc/sixlowpan.h"
#endif

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#ifdef MODULE_L2_PING
#include "l2_ping.h"
#endif

#ifdef MODULE_GNRC_PKTBUF
#include "net/gnrc/pktbuf.h"
#endif

#ifdef MODULE_GNRC_PKTDUMP
#include "net/gnrc/pktdump.h"
#endif

#ifdef MODULE_GNRC_UDP
#include "net/gnrc/udp.h"
#endif

#ifdef MODULE_GNRC_TCP
#include "net/gnrc/tcp.h"
#endif

#ifdef MODULE_LWIP
#include "lwip.h"
#endif

#ifdef MODULE_OPENTHREAD
#include "ot.h"
#endif

#ifdef MODULE_FIB
#include "net/fib.h"
#endif

#ifdef MODULE_GCOAP
#include "net/gcoap.h"
#endif

#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib.h"
#endif

#ifdef MODULE_SKALD
#include "net/skald.h"
#endif

#ifdef MODULE_NDN_RIOT
#include "ndn-riot/ndn.h"
#endif

#ifdef MODULE_ASYMCUTE
#include "net/asymcute.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

void auto_init(void)
{
#ifdef MODULE_PRNG
    void auto_init_random(void);
    auto_init_random();
#endif
#ifdef MODULE_XTIMER
    DEBUG("Auto init xtimer module.\n");
    xtimer_init();
#endif
#ifdef MODULE_MCI
    DEBUG("Auto init mci module.\n");
    mci_initialize();
#endif
#ifdef MODULE_PROFILING
    extern void profiling_init(void);
    profiling_init();
#endif
#ifdef MODULE_GNRC_PKTBUF
    DEBUG("Auto init gnrc_pktbuf module\n");
    gnrc_pktbuf_init();
#endif
#ifdef MODULE_GNRC_PKTDUMP
    DEBUG("Auto init gnrc_pktdump module.\n");
    gnrc_pktdump_init();
#endif
#ifdef MODULE_GNRC_SIXLOWPAN
    DEBUG("Auto init gnrc_sixlowpan module.\n");
    gnrc_sixlowpan_init();
#endif
#ifdef MODULE_GNRC_IPV6
    DEBUG("Auto init gnrc_ipv6 module.\n");
    gnrc_ipv6_init();
#endif
#ifdef MODULE_GNRC_UDP
    DEBUG("Auto init UDP module.\n");
    gnrc_udp_init();
#endif
#ifdef MODULE_GNRC_TCP
    DEBUG("Auto init TCP module\n");
    gnrc_tcp_init();
#endif
#ifdef MODULE_LWIP
    DEBUG("Bootstraping lwIP.\n");
    lwip_bootstrap();
#endif
#ifdef MODULE_OPENTHREAD
    extern void openthread_bootstrap(void);
    openthread_bootstrap();
#endif
#ifdef MODULE_GCOAP
    DEBUG("Auto init gcoap module.\n");
    gcoap_init();
#endif
#ifdef MODULE_DEVFS
    DEBUG("Mounting /dev\n");
    extern void auto_init_devfs(void);
    auto_init_devfs();
#endif
#ifdef MODULE_GNRC_IPV6_NIB
    DEBUG("Auto init gnrc_ipv6_nib module.\n");
    gnrc_ipv6_nib_init();
#endif
#ifdef MODULE_SKALD
    DEBUG("Auto init Skald\n");
    skald_init();
#endif
#ifdef MODULE_RDCLI_COMMON
    DEBUG("Auto init rdcli_common module\n");
    extern void rdcli_common_init(void);
    rdcli_common_init();
#endif
#ifdef MODULE_RDCLI_SIMPLE_STANDALONE
    DEBUG("Auto init rdcli_simple module\n");
    extern void rdcli_simple_run(void);
    rdcli_simple_run();
#endif
#ifdef MODULE_ASYMCUTE
    DEBUG("Auto init Asymcute\n");
    asymcute_handler_run();
#endif

/* initialize network devices */
#ifdef MODULE_AUTO_INIT_GNRC_NETIF

#ifdef MODULE_AT86RF2XX
    extern void auto_init_at86rf2xx(void);
    auto_init_at86rf2xx();
#endif

#ifdef MODULE_MRF24J40
    extern void auto_init_mrf24j40(void);
    auto_init_mrf24j40();
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

#ifdef MODULE_ETHOS
    extern void auto_init_ethos(void);
    auto_init_ethos();
#endif

#ifdef MODULE_SLIPDEV
    extern void auto_init_slipdev(void);
    auto_init_slipdev();
#endif

#ifdef MODULE_CC110X
    extern void auto_init_cc110x(void);
    auto_init_cc110x();
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

#ifdef MODULE_SX127X
    extern void auto_init_sx127x(void);
    auto_init_sx127x();
#endif

#endif /* MODULE_AUTO_INIT_GNRC_NETIF */

#ifdef MODULE_GNRC_UHCPC
    extern void auto_init_gnrc_uhcpc(void);
    auto_init_gnrc_uhcpc();
#endif

/* initialize NDN module after the network devices are initialized */
#ifdef MODULE_NDN_RIOT
    DEBUG("Auto init NDN module.\n");
    ndn_init();
#endif

/* initialize sensors and actuators */
#ifdef MODULE_SHT1X
    DEBUG("Auto init SHT1X module (SHT10/SHT11/SHT15 sensor driver).\n");
    extern void auto_init_sht1x(void);
    auto_init_sht1x();
#endif

#ifdef MODULE_AUTO_INIT_SAUL
    DEBUG("auto_init SAUL\n");

#ifdef MODULE_SAUL_GPIO
    extern void auto_init_gpio(void);
    auto_init_gpio();
#endif
#ifdef MODULE_SAUL_ADC
    extern void auto_init_adc(void);
    auto_init_adc();
#endif
#ifdef MODULE_LSM303DLHC
    extern void auto_init_lsm303dlhc(void);
    auto_init_lsm303dlhc();
#endif
#ifdef MODULE_LPS331AP
    extern void auto_init_lps331ap(void);
    auto_init_lps331ap();
#endif
#ifdef MODULE_ISL29020
    extern void auto_init_isl29020(void);
    auto_init_isl29020();
#endif
#ifdef MODULE_L3G4200D
    extern void auto_init_l3g4200d(void);
    auto_init_l3g4200d();
#endif
#ifdef MODULE_LIS3DH
    extern void auto_init_lis3dh(void);
    auto_init_lis3dh();
#endif
#ifdef MODULE_LIS3MDL
extern void auto_init_lis3mdl(void);
auto_init_lis3mdl();
#endif
#ifdef MODULE_MAG3110
    extern void auto_init_mag3110(void);
    auto_init_mag3110();
#endif
#ifdef MODULE_MMA8X5X
    extern void auto_init_mma8x5x(void);
    auto_init_mma8x5x();
#endif
#ifdef MODULE_MPL3115A2
    extern void auto_init_mpl3115a2(void);
    auto_init_mpl3115a2();
#endif
#ifdef MODULE_MPU9150
extern void auto_init_mpu9150(void);
auto_init_mpu9150();
#endif
#ifdef MODULE_GROVE_LEDBAR
    extern void auto_init_grove_ledbar(void);
    auto_init_grove_ledbar();
#endif
#ifdef MODULE_PIR
    extern void auto_init_pir(void);
    auto_init_pir();
#endif
#ifdef MODULE_SI70XX
    extern void auto_init_si70xx(void);
    auto_init_si70xx();
#endif
#ifdef MODULE_BMP180
    extern void auto_init_bmp180(void);
    auto_init_bmp180();
#endif
#ifdef MODULE_BMX055
    extern void auto_init_bmx055(void);
    auto_init_bmx055();
#endif
#if defined(MODULE_BME280) || defined(MODULE_BMP280)
    extern void auto_init_bmx280(void);
    auto_init_bmx280();
#endif
#ifdef MODULE_JC42
    extern void auto_init_jc42(void);
    auto_init_jc42();
#endif
#ifdef MODULE_TSL2561
    extern void auto_init_tsl2561(void);
    auto_init_tsl2561();
#endif
#ifdef MODULE_PULSE_COUNTER
    extern void auto_init_pulse_counter(void);
    auto_init_pulse_counter();
#endif
#ifdef MODULE_HDC1000
    extern void auto_init_hdc1000(void);
    auto_init_hdc1000();
#endif
#ifdef MODULE_FXOS8700
    extern void auto_init_fxos8700(void);
    auto_init_fxos8700();
#endif
#ifdef MODULE_HTS221
    extern void auto_init_hts221(void);
    auto_init_hts221();
#endif
#ifdef MODULE_DHT
    extern void auto_init_dht(void);
    auto_init_dht();
#endif
#ifdef MODULE_TMP006
    extern void auto_init_tmp006(void);
    auto_init_tmp006();
#endif
#ifdef MODULE_TCS37727
    extern void auto_init_tcs37727(void);
    auto_init_tcs37727();
#endif
#ifdef MODULE_VEML6070
    extern void auto_init_veml6070(void);
    auto_init_veml6070();
#endif
#ifdef MODULE_IO1_XPLAINED
    extern void auto_init_io1_xplained(void);
    auto_init_io1_xplained();
#endif
#ifdef MODULE_ADXL345
    extern void auto_init_adxl345(void);
    auto_init_adxl345();
#endif
#ifdef MODULE_LSM6DSL
    extern void auto_init_lsm6dsl(void);
    auto_init_lsm6dsl();
#endif
#ifdef MODULE_ADCXX1C
    extern void auto_init_adcxx1c(void);
    auto_init_adcxx1c();
#endif
#ifdef MODULE_ADS101X
    extern void auto_init_ads101x(void);
    auto_init_ads101x();
#endif
#ifdef MODULE_LIS2DH12
    extern void auto_init_lis2dh12(void);
    auto_init_lis2dh12();
#endif
#ifdef MODULE_SI114X
    extern void auto_init_si114x(void);
    auto_init_si114x();
#endif
#ifdef MODULE_MMA7660
    extern void auto_init_mma7660(void);
    auto_init_mma7660();
#endif

#endif /* MODULE_AUTO_INIT_SAUL */

#ifdef MODULE_AUTO_INIT_GNRC_RPL

#ifdef MODULE_GNRC_RPL
    extern void auto_init_gnrc_rpl(void);
    auto_init_gnrc_rpl();
#endif

#endif /* MODULE_AUTO_INIT_GNRC_RPL */

/* initialize storage devices */
#ifdef MODULE_AUTO_INIT_STORAGE
    DEBUG("auto_init STORAGE\n");

#ifdef MODULE_SDCARD_SPI
    extern void auto_init_sdcard_spi(void);
    auto_init_sdcard_spi();
#endif

#endif /* MODULE_AUTO_INIT_STORAGE */

#ifdef MODULE_AUTO_INIT_CAN
    DEBUG("auto_init CAN\n");

    extern void auto_init_candev(void);
    auto_init_candev();

#endif /* MODULE_AUTO_INIT_CAN */
}
