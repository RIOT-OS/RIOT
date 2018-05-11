/*
 * Copyright (C) 2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_rail Silabs EFR32 radio driver
 * @ingroup     drivers_netdev
 *
 * This module contains drivers for the embedded radio transceivers in
 * Silabs EFR32 SoCs using the probritary librail API.
 * The driver is aimed to work with all devices of this series.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for Silabs EFR32 based drivers
 *
 * @author      Kai Beckmann <kai.beckmann@hs-rm.de>
 */


#ifndef RAIL_DRV_H
#define RAIL_DRV_H

#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/gnrc/nettype.h"
#include "net/eui64.h"

#include "ringbuffer.h"

#include "rail.h"

#include "rail_helper.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name    Channel configuration
 * @{
 */
/* TODO replace with netdev_ieee802154 rework */
#define RAIL_868MHZ_MIN_CHANNEL           (IEEE802154_CHANNEL_MIN_SUBGHZ)
#define RAIL_868MHZ_MAX_CHANNEL           (IEEE802154_CHANNEL_MIN_SUBGHZ) /* 868MHz has only ONE channel! */
#define RAIL_868MHZ_DEFAULT_CHANNEL       (IEEE802154_CHANNEL_MIN_SUBGHZ)
#define RAIL_912MHZ_MIN_CHANNEL           (IEEE802154_CHANNEL_MIN_SUBGHZ)
#define RAIL_912MHZ_MAX_CHANNEL           (IEEE802154_CHANNEL_MAX_SUBGHZ)
#define RAIL_912MHZ_DEFAULT_CHANNEL       (IEEE802154_DEFAULT_SUBGHZ_CHANNEL)
/* Page 2 is O-QPSK 100 kbit/s (channel 0), or 250 kbit/s (channels 1-10) */
#define RAIL_SUBGHZ_DEFAULT_PAGE          (IEEE802154_DEFAULT_SUBGHZ_PAGE)
#define RAIL_2P4GH_MIN_CHANNEL            (IEEE802154_CHANNEL_MIN)
#define RAIL_2P4GH_MAX_CHANNEL            (IEEE802154_CHANNEL_MAX)
#define RAIL_2P4GH_DEFAULT_CHANNEL        (IEEE802154_DEFAULT_CHANNEL)
/* Only page 0 is supported in the 2.4 GHz band */

/** @} */

/**
 * @name radio transceiver states
 * @{
 */
/* TODO
    - use enum?
    - use !
 */
#define RAIL_TRANSCEIVER_STATE_UNINITIALIZED    0x00
#define RAIL_TRANSCEIVER_STATE_IDLE             0x01
#define RAIL_TRANSCEIVER_STATE_SLEEP            0x02
#define RAIL_TRANSCEIVER_STATE_OFF              0x03
#define RAIL_TRANSCEIVER_STATE_RX               0x04
#define RAIL_TRANSCEIVER_STATE_TX               0x05

/** @} */


/**
 * @brief default panid for rail
 */
#define RAIL_DEFAULT_PANID         (IEEE802154_DEFAULT_PANID)

/**
 * @brief default tx power
 */
#define RAIL_DEFAULT_TXPOWER       (IEEE802154_DEFAULT_TXPOWER)
/**
 * @brief default CSMA retries
 */
#define RAIL_DEFAULT_CSMA_TRIES     5


/**
 * @brief enum for the three supported radio bands
 */
enum rail_transceiver_config_frequency {
    RAIL_TRANSCEIVER_FREQUENCY_2P4GHZ,  /**< 2.4GHz */
    RAIL_TRANSCEIVER_FREQUENCY_868MHZ,  /**< EU 868 MHz band */
    RAIL_TRANSCEIVER_FREQUENCY_912MHZ   /** US 912 MHz band */
};

/**
 * @brief   struct holding all params needed for device initialization
 */
typedef struct rail_params {
    enum rail_transceiver_config_frequency freq;    /**< radio band to operate */
    /* RAIL_RADIO_PA_VOLTAGE */
    /* RAIL_DEFAULT_TXPOWER */
    /* RAIL_DEFAULT_PANID */
    /* IEEE802154_DEFAULT_CHANNEL */
    /* isPanCoordinator */
    /* auto ack on / off */
    /* auto ack timeout */

} rail_params_t;



/**
 * @brief   Device descriptor for Silabs EFR32 rail radio devices
 *
 * @extends netdev_ieee802154_t
 */
typedef struct {
    netdev_ieee802154_t netdev;     /**< inherited from netdev_ieee802154_t */
    rail_params_t params;           /**< the params for the driver */

    RAIL_Handle_t rhandle;          /**< handle of the RAIL driver blob instance */
    RAIL_Config_t rconfig;          /**< config of the RAIL driver blob */
    RAIL_CsmaConfig_t csma_config;  /**< config for CSMA */

    uint8_t state;                  /**< state of the radio transceiver */

    rail_event_queue_t event_queue; /**< event queue for the netdev layer */

    uint32_t event_count;           /**< stat / debug info, how many rail events have occured */

    bool promiscuousMode;           /**< flag if set to promiscuous mode */
} rail_t;


/**
 * @brief   Setup an Silabs EFR32 rail based device
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       parameters for device initialization
 */
void rail_setup(rail_t *dev, const rail_params_t *params);


/**
 * @brief   configures radio with default values
 *
 * @param[in,out] dev       device to reset
 */
int rail_init(rail_t *dev);

/**
 * @brief set the radio into receive mode
 *
 * @param[in] dev       device
 */
int rail_start_rx(rail_t *dev);

/**
 * @brief transmit a packet
 *
 * @param[in] dev           radio device
 * @param[in] data_ptr      pointer to packet to transmit
 * @param[in] data_length   size of packet
 */
int rail_transmit_frame(rail_t *dev, uint8_t *data_ptr, size_t data_length);

#ifdef __cplusplus
}
#endif

#endif /* RAIL_DRV_H */

/** @} */
