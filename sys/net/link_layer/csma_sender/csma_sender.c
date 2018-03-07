/*
 * Copyright (C) 2015 INRIA
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include "xtimer.h"
#include "random.h"
#include "net/netdev.h"
#include "net/netopt.h"

#include "net/csma_sender.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

const csma_sender_conf_t CSMA_SENDER_CONF_DEFAULT = {
    CSMA_SENDER_MIN_BE_DEFAULT,
    CSMA_SENDER_MAX_BE_DEFAULT,
    CSMA_SENDER_MAX_BACKOFFS_DEFAULT,
    CSMA_SENDER_BACKOFF_PERIOD_UNIT
};

/*--------------------- "INTERNAL" UTILITY FUNCTIONS ---------------------*/

/**
 * @brief choose an adequate random backoff period in microseconds,
 *        from the given Backoff Exponent
 *
 * @param[in] be        Backoff Exponent for the computation of period
 *
 * @return              An adequate random backoff exponent in microseconds
 */
static inline uint32_t choose_backoff_period(int be,
                                             const csma_sender_conf_t *conf)
{
    if (be < conf->min_be) {
        be = conf->min_be;
    }
    if (be > conf->max_be) {
        be = conf->max_be;
    }
    uint32_t max_backoff = ((1 << be) - 1) * CSMA_SENDER_BACKOFF_PERIOD_UNIT;

    uint32_t period = random_uint32() % max_backoff;
    if (period < CSMA_SENDER_BACKOFF_PERIOD_UNIT) {
        period = CSMA_SENDER_BACKOFF_PERIOD_UNIT;
    }

    return period;
}


/**
 * @brief Perform a CCA and send the given packet if medium is available
 *
 * @param[in] device    netdev device, needs to be already initialized
 * @param[in] iolist    pointer to the data
 *
 * @return              the return value of device driver's
 *                      netdev_driver_t::send() function if medium was
 *                      available
 * @return              -ECANCELED if an internal driver error occurred
 * @return              -EBUSY if radio medium was not available
 *                      to send the given data
 */
static int send_if_cca(netdev_t *device, iolist_t *iolist)
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
        return device->driver->send(device, iolist);
    }

    /* if we arrive here, medium was not available for transmission */
    DEBUG("csma: Radio medium busy.\n");
    return -EBUSY;
}

/*------------------------- "EXPORTED" FUNCTIONS -------------------------*/

int csma_sender_csma_ca_send(netdev_t *dev, iolist_t *iolist,
                             const csma_sender_conf_t *conf)
{
    netopt_enable_t hwfeat;

    assert(dev);
    /* choose default configuration if none is given */
    if (conf == NULL) {
        conf = &CSMA_SENDER_CONF_DEFAULT;
    }
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
        case -EOVERFLOW: /* (normally impossible...*/
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
        return dev->driver->send(dev, iolist);
    }

    /* if we arrive here, then we must perform the CSMA/CA procedure
       ourselves by software */
    random_init(_xtimer_now());
    DEBUG("csma: Starting software CSMA/CA....\n");

    int nb = 0, be = conf->min_be;

    while (nb <= conf->max_be) {
        /* delay for an adequate random backoff period */
        uint32_t bp = choose_backoff_period(be, conf);
        xtimer_usleep(bp);

        /* try to send after a CCA */
        res = send_if_cca(dev, iolist);
        if (res >= 0) {
            /* TX done */
            return res;
        }
        else if (res != -EBUSY) {
            /* something has gone wrong, return the error code */
            return res;
        }

        /* medium is busy: increment CSMA counters */
        DEBUG("csma: Radio medium busy.\n");
        be++;
        if (be > conf->max_be) {
            be = conf->max_be;
        }
        nb++;
        /* ... and try again if we have no exceeded the retry limit */
    }

    /* if we arrive here, medium was never available for transmission */
    DEBUG("csma: Software CSMA/CA failure: medium never available.\n");
    return -EBUSY;
}


int csma_sender_cca_send(netdev_t *dev, iolist_t *iolist)
{
    netopt_enable_t hwfeat;

    assert(dev);
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
        case -EOVERFLOW: /* (normally impossible...*/
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
        return dev->driver->send(dev, iolist);
    }

    /* if we arrive here, we must do CCA ourselves to see if radio medium
       is clear before sending */
    res = send_if_cca(dev, iolist);
    if (res == -EBUSY) {
        DEBUG("csma: Transmission cancelled!\n");
    }

    return res;
}
