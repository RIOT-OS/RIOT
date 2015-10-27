/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_csma_sender
 * @file
 * @brief       Implementation of the CSMA/CA helper
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdbool.h>

#include "kernel.h"
#include "xtimer.h"
#include "random.h"
#include "net/gnrc/csma_sender.h"
#include "net/gnrc.h"
#include "net/netopt.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif


/** @brief Current value for mac_min_be parameter */
static uint8_t mac_min_be = MAC_MIN_BE_DEFAULT;

/** @brief Current value for mac_max_be parameter */
static uint8_t mac_max_be = MAC_MAX_BE_DEFAULT;

/** @brief Current value for mac_max_csma_backoffs parameter */
static uint8_t mac_max_csma_backoffs = MAC_MAX_CSMA_BACKOFFS_DEFAULT;


/*--------------------- "INTERNAL" UTILITY FUNCTIONS ---------------------*/

/**
 * @brief choose an adequate random backoff period in microseconds,
 *        from the given Backoff Exponent
 *
 * @param[in] be        Backoff Exponent for the computation of period
 *
 * @return              An adequate random backoff exponent in microseconds
 */
static inline uint32_t choose_backoff_period(int be)
{
    if (be < mac_min_be) {
        be = mac_min_be;
    }
    if (be > mac_max_be) {
        be = mac_max_be;
    }
    uint32_t max_backoff = ((1 << be) - 1) * A_UNIT_BACKOFF_PERIOD_MICROSEC;

    uint32_t period = genrand_uint32() % max_backoff;
    if (period < A_UNIT_BACKOFF_PERIOD_MICROSEC) {
        period = A_UNIT_BACKOFF_PERIOD_MICROSEC;
    }

    return period;
}


/**
 * @brief Perform a CCA and send the given packet if medium is available
 *
 * @param[in] device    netdev device, needs to be already initialized
 * @param[in] data      pointer to the data in the packet buffer;
 *                      it must be a complete 802.15.4-compliant frame,
 *                      ready to be sent to the radio transceiver
 *
 * @return              the return value of device driver's @c send_data()
 *                      function if medium was avilable
 * @return              -ECANCELED if an internal driver error occured
 * @return              -EBUSY if radio medium was not available
 *                      to send the given data
 */
static int send_if_cca(gnrc_netdev_t *device, gnrc_pktsnip_t *data)
{
    netopt_enable_t hwfeat;

    /* perform a CCA */
    DEBUG("csma: Checking radio medium availability...\n");
    int res = device->driver->get(device,
                                  NETOPT_IS_CHANNEL_CLR,
                                  (void *) &hwfeat,
                                  sizeof(netopt_enable_t));
    if (res < 0) {
        /* normally impossible: we got a big internal problem! */
        DEBUG("csma: !!! DEVICE DRIVER FAILURE! TRANSMISSION ABORTED!\n");
        return -ECANCELED;
    }

    /* if medium is clear, send the packet and return */
    if (hwfeat == NETOPT_ENABLE) {
        DEBUG("csma: Radio medium available: sending packet.\n");
        return device->driver->send_data(device, data);
    }

    /* if we arrive here, medium was not available for transmission */
    DEBUG("csma: Radio medium busy.\n");
    return -EBUSY;
}

/*------------------------- "EXPORTED" FUNCTIONS -------------------------*/

void set_csma_mac_min_be(uint8_t val)
{
    mac_min_be = val;
}

void set_csma_mac_max_be(uint8_t val)
{
    mac_max_be = val;
}

void set_csma_mac_max_csma_backoffs(uint8_t val)
{
    mac_max_csma_backoffs = val;
}


int csma_ca_send(gnrc_netdev_t *dev, gnrc_pktsnip_t *pkt)
{
    netopt_enable_t hwfeat;

    /* Does the transceiver do automatic CSMA/CA when sending? */
    int res = dev->driver->get(dev,
                               NETOPT_CSMA,
                               (void *) &hwfeat,
                               sizeof(netopt_enable_t));
    bool ok = false;
    switch (res) {
    case -ENODEV:
        /* invalid device pointer given */
        return -ENODEV;
    case -ENOTSUP:
        /* device doesn't make auto-CSMA/CA */
        break;
    case -EOVERFLOW:  /* (normally impossible...*/
    case -ECANCELED:
        DEBUG("csma: !!! DEVICE DRIVER FAILURE! TRANSMISSION ABORTED!\n");
        /* internal driver error! */
        return -ECANCELED;
    default:
        ok = (hwfeat == NETOPT_ENABLE);
    }

    if (ok) {
        /* device does CSMA/CA all by itself: let it do its job */
        DEBUG("csma: Network device does hardware CSMA/CA\n");
        return dev->driver->send_data(dev, pkt);
    }

    /* if we arrive here, then we must perform the CSMA/CA procedure
       ourselves by software */
    genrand_init(xtimer_now());
    DEBUG("csma: Starting software CSMA/CA....\n");

    int nb = 0, be = mac_min_be;

    while (nb <= mac_max_csma_backoffs) {
        /* delay for an adequate random backoff period */
        uint32_t bp = choose_backoff_period(be);
        xtimer_usleep(bp);

        /* try to send after a CCA */
        res = send_if_cca(dev, pkt);
        if (res >= 0) {
            /* TX done */
            return res;
        } else if (res != -EBUSY) {
            /* something has gone wrong, return the error code */
            return res;
        }

        /* medium is busy: increment CSMA counters */
        DEBUG("csma: Radio medium busy.\n");
        be++;
        if (be > mac_max_be) {
            be = mac_max_be;
        }
        nb++;
        /* ... and try again if we have no exceeded the retry limit */
    }

    /* if we arrive here, medium was never available for transmission */
    DEBUG("csma: Software CSMA/CA failure: medium never available.\n");
    return -EBUSY;
}


int cca_send(gnrc_netdev_t *dev, gnrc_pktsnip_t *pkt)
{
    netopt_enable_t hwfeat;

    /* Does the transceiver do automatic CCA before sending? */
    int res = dev->driver->get(dev,
                               NETOPT_AUTOCCA,
                               (void *) &hwfeat,
                               sizeof(netopt_enable_t));
    bool ok = false;
    switch (res) {
    case -ENODEV:
        /* invalid device pointer given */
        return -ENODEV;
    case -ENOTSUP:
        /* device doesn't make auto-CCA */
        break;
    case -EOVERFLOW:  /* (normally impossible...*/
    case -ECANCELED:
        /* internal driver error! */
        DEBUG("csma: !!! DEVICE DRIVER FAILURE! TRANSMISSION ABORTED!\n");
        return -ECANCELED;
    default:
        ok = (hwfeat == NETOPT_ENABLE);
    }

    if (ok) {
        /* device does auto-CCA: let him do its job */
        DEBUG("csma: Network device does auto-CCA checking.\n");
        return dev->driver->send_data(dev, pkt);
    }

    /* if we arrive here, we must do CCA ourselves to see if radio medium
       is clear before sending */
    res = send_if_cca(dev, pkt);
    if (res == -EBUSY) {
        DEBUG("csma: Transmission cancelled!\n");
    }

    return res;
}
