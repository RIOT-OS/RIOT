/**
 * Auto initialization for used modules
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup auto_init
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

#ifdef MODULE_IO1_XPLAINED
#include "io1_xplained.h"
#endif

#ifdef MODULE_SHT11
#include "sht11.h"
#endif

#ifdef MODULE_MCI
#include "diskio.h"
#endif

#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif

#ifdef MODULE_RTC
#include "periph/rtc.h"
#endif

#ifdef MODULE_GNRC_SIXLOWPAN
#include "net/gnrc/sixlowpan.h"
#endif

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#ifdef MODULE_GNRC_IPV6_NETIF
#include "net/gnrc/ipv6/netif.h"
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

#ifdef MODULE_FIB
#include "net/fib.h"
#endif

#ifdef MODULE_TINYMT32
#include "random.h"
#endif

#ifdef MODULE_GCOAP
#include "net/gcoap.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

void auto_init(void)
{
#ifdef MODULE_TINYMT32
    random_init(0);
#endif
#ifdef MODULE_XTIMER
    DEBUG("Auto init xtimer module.\n");
    xtimer_init();
#endif
#ifdef MODULE_RTC
    DEBUG("Auto init rtc module.\n");
    rtc_init();
#endif
#ifdef MODULE_IO1_XPLAINED
    DEBUG("Auto init IO1 Xplained extension module.\n");
    io1_xplained_auto_init();
#endif
#ifdef MODULE_SHT11
    DEBUG("Auto init SHT11 module.\n");
    sht11_init();
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
#ifdef MODULE_GCOAP
    DEBUG("Auto init gcoap module.\n");
    gcoap_init();
#endif
#ifdef MODULE_DEVFS
    DEBUG("Mounting /dev\n");
    extern void auto_init_devfs(void);
    auto_init_devfs();
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

#ifdef MODULE_GNRC_SLIP
    extern void auto_init_slip(void);
    auto_init_slip();
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

#ifdef MODULE_NETDEV2_TAP
    extern void auto_init_netdev2_tap(void);
    auto_init_netdev2_tap();
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

#endif /* MODULE_AUTO_INIT_GNRC_NETIF */

#ifdef MODULE_GNRC_IPV6_NETIF
    gnrc_ipv6_netif_init_by_dev();
#endif

#ifdef MODULE_GNRC_UHCPC
    extern void auto_init_gnrc_uhcpc(void);
    auto_init_gnrc_uhcpc();
#endif

/* initialize sensors and actuators */
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
#ifdef MODULE_MMA8X5X
    extern void auto_init_mma8x5x(void);
    auto_init_mma8x5x();
#endif
#ifdef MODULE_SI70XX
    extern void auto_init_si70xx(void);
    auto_init_si70xx();
#endif
#ifdef MODULE_BMP180
    extern void auto_init_bmp180(void);
    auto_init_bmp180();
#endif
#ifdef MODULE_BME280
    extern void auto_init_bme280(void);
    auto_init_bme280();
#endif
#ifdef MODULE_JC42
    extern void auto_init_jc42(void);
    auto_init_jc42();
#endif
#ifdef MODULE_TSL2561
    extern void auto_init_tsl2561(void);
    auto_init_tsl2561();
#endif
#ifdef MODULE_HDC1000
    extern void auto_init_hdc1000(void);
    auto_init_hdc1000();
#endif
#ifdef MODULE_DHT
    extern void auto_init_dht(void);
    auto_init_dht();
#endif
#ifdef MODULE_TCS37727
    extern void auto_init_tcs37727(void);
    auto_init_tcs37727();
#endif
#ifdef MODULE_VEML6070
    extern void auto_init_veml6070(void);
    auto_init_veml6070();
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
}
