#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "periph/uart.h"
#include "iolist.h"
#include "net/netdev.h"
#include "net/netopt.h"
#include "net/ethernet.h"   /* for ETHERNET_* constants & 6-byte MAC */

/*
 * A lightweight optical-modem netdev. It treats the optical hardware as a
 * byte stream (UART). Each L2 frame is sent/received with a 2-byte length
 * prefix (host order). The modem/PHY is responsible for actual modulation
 * (OOK, QAM, …), timing, and optional integrity/FEC.
 *
 * By default we present as an Ethernet-like link to reuse GNRC/lwIP adapters.
 * If you prefer raw L2, set OPTMODEM_PRESENTS_AS_ETHERNET to 0 and attach
 * with gnrc_netif_raw_create().
 *
 * See:
 *   - netdev API: send/confirm_send/recv/isr/get/set
 *   - netopt options (ADDRESS, MAX_PDU_SIZE, DEVICE_TYPE, IS_WIRED, STATE)
 */

#ifndef OPTMODEM_PRESENTS_AS_ETHERNET
#define OPTMODEM_PRESENTS_AS_ETHERNET   (1)
#endif

#ifndef OPTMODEM_UART_DEV
#define OPTMODEM_UART_DEV               UART_DEV(1)
#endif

#ifndef OPTMODEM_UART_BAUD
#define OPTMODEM_UART_BAUD              (115200U)
#endif

/* Maximum frame length we allow up the stack (including L2 header).
 * Choose Ethernet maxima for widest compatibility with GNRC/lwIP adapters. */
#if OPTMODEM_PRESENTS_AS_ETHERNET
#define OPTMODEM_MAX_FRAME              (ETHERNET_MAX_LEN)   /* 1518 incl. FCS */
#define OPTMODEM_ADDR_LEN               (ETHERNET_ADDR_LEN)  /* 6 */
#else
/* Raw mode: pick something sane; adjust to your optical modem constraints */
#define OPTMODEM_MAX_FRAME              (1536)
#define OPTMODEM_ADDR_LEN               (0)                  /* if address-less */
#endif

/* Optional: driver-local “mode” to reflect modem modulation the *hardware*
 * performs. These are not exposed via netopt (to avoid inventing public ABI)
 * but can be set from board code or apps that know this driver. */
typedef enum {
    OPTMODEM_MODE_UNKNOWN = 0,
    OPTMODEM_MODE_OOK,
    OPTMODEM_MODE_QAM16,
    OPTMODEM_MODE_QAM64,
    OPTMODEM_MODE_QPSK,
} optmodem_mode_t;

/* Minimal parameterization (expand to SPI, GPIOs, flow control as needed) */
typedef struct {
    uart_t      uart;
    uint32_t    baud;
    uint8_t     mac[ETHERNET_ADDR_LEN];   /* used when presenting as Ethernet */
} optmodem_params_t;

typedef struct {
    /* netdev “base” must be first */
    netdev_t                netdev;

    /* static parameters */
    optmodem_params_t       params;

    /* RX staging buffer (single in-flight frame) */
    uint8_t                 rx_buf[OPTMODEM_MAX_FRAME];
    size_t                  rx_expected;      /* frame length we’re collecting */
    size_t                  rx_have;          /* bytes currently in rx_buf */
    bool                    rx_ready;         /* true when a full frame is buffered */

    /* TX (we send directly from iolist; this can remain empty) */

    /* driver-local */
    optmodem_mode_t         mode;
    netopt_state_t          state;
} optmodem_t;

/* Public driver vtable */
extern const netdev_driver_t optmodem_driver;

/* Convenience init: fill dev->params first, then call this. */
int optmodem_init(optmodem_t *dev);

/* Optional driver-local helpers to steer the hardware modulation mode. */
static inline void optmodem_set_mode(optmodem_t *dev, optmodem_mode_t m) { dev->mode = m; }
static inline optmodem_mode_t optmodem_get_mode(const optmodem_t *dev)   { return dev->mode; }

