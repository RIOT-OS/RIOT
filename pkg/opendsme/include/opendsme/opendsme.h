/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_opendsme openDSME - IEEE 802.15.4 DSME
 * @ingroup     pkg
 * @ingroup     net
 * @brief       Support for IEEE 802.15.4 Deterministic and Synchronous Multi-channel Extension.
 *
 * @author      José I. Álamos <jose.alamos@haw-hamburg.de>
 *
 * @{
 *
 * # About
 *
 * The IEEE 802.15.4 standard with its widespread usage in wireless sensor and
 * actuator networks was extended by several techniques that allow reliable data
 * transmission for critical applications, such as industrial plants. This
 * includes the Deterministic and Synchronous Multi-channel Extension (DSME) that
 * allows for distributed assignment of time slots on multiple channels.
 *
 * RIOT provides DSME support via the open source implementation
 * [openDSME](https://opendsme.org/).
 *
 * # Features
 * - Topology agnostic (peer-to-peer, star, mesh, etc).
 * - By design, a DSME network can be extended just by adding more coordinators near the edge.
 * - Automatic resolution of beacon collision
 * - Frame transmission using either CSMA-CA or multi-channel GTS (guaranteed time slot)
 * - Built-in slot negotiation (dynamic allocation) and static allocation.
 *
 * ## Network formation
 *
 * A PAN coordinator device initializes a DSME superframe structure which consists
 * of a series of superframes that repeat indefinitely. A superframe consists
 * of a Beacon Slot, a Contention Access Period and Contention Free Period.
 *
 *      BS                 CAP                      CFP        BS
 *     +------------------------------------------------------+----
 *     |   |                                |--|--|--|--|--|--|   | ...
 *     |   |                                |--|--|--|--|--|--|   |
 *     |   |                                |--|--|--|--|--|--|   | ...
 *     |   |                                |--|--|--|--|--|--|   |
 *     +---+--------------------------------+--+--+--+--+--+--+---+
 *     <------------------- Superframe ----------------------->
 *
 * Each period of the superframe serves a dedicated purpose:
 * - Beacon Slot: Used for beacon transmission. PAN Coordinators and Coordinators
 *   transmit beacons periodically in these beacon slots. All devices (except the
 *   PAN coordinator) listen to their parent coordinator beacons and synchronize
 *   their superframe structure accordingly. Each coordinator (including the PAN
 *   Coordinator) transmits beacon periocally. To avoid beacon collisions, DSME
 *   allows to set the beacon interval of all devices in multiples of the
 *   superframe structure. This allows other coordinator devices to transmit
 *   beacons in a different superframe offset and therefore avoid beacon
 *   collisions. The beacon slot has a duration of one slot.
 * - Contention Access Period: During CAP, devices transmit using slotted
 *   CSMA-CA. This means, the CAP is divided in a series of CCA slots. These
 *   slots are aligned with the beginning of the CAP. A device can transmit
 *   only if CCA assess clear channel on three consecutive slots. If there's
 *   not enough CAP time left, the frame will be postponed until the next CAP
 *   period. On CSMA-CA transmissions, the MAC put the message in a CAP queue
 *   and transmits as soon as possible. By default, the CAP queue size is 8
 *   frames. The CAP has a duration of 8 slots
 * - Contention Free Period: During CFP devices transmit using a dedicated
 *   Guaranteed Time Slot. GTS are multi-channel, which allows concurrent
 *   communication in the same GTS. On transmission schedule, the MAC puts
 *   the message in a CFP queue and transmit in the next available GTS. By
 *   default, the CFP queue size is 22. To (de)allocate a GTS, the upper layer
 *   triggers a GTS Request to negotiate slots with a neighbour device. The GTS
 *   Request is sent during CAP, as all MAC commands. The CFP has 7
 *   multichannel GTS (each one with a duration of one slot) and the number of
 *   channels depends on the underlying PHY (16 for O-QPSK). Note that slots
 *   are unidirectional (TX or RX only) and cannot transmit neither broadcast
 *   frames nor MAC commands.
 *
 * DSME allows to extend the number of GTS resources by combining superframes
 * into a multisuperframe structure. The number of superframes per multisuperframes
 * is configurable. This extend the number of GTS to seven times the number of
 * superframes per multisuperframe.
 *
 * Joining devices scan for beacons and perform the association procedure with
 * the the (PAN) coordinator. On success, the device is ready to communicate with
 * other devices in the DSME network.
 * To extend the network, coordinator devices can associate to any other
 * coordinator (including the PAN coordinator) and start emitting beacons in a
 * different Beacon Slot offset. Devices can then join the network via the new
 * coordinator. DSME manages beacon scheduling automatically and handles beacon
 * collisions accordingly.
 *
 * ## DSME superframe structure
 *
 * The DSME superframe structure relies on three configuration parameters:
 * Superframe Order (SO), Multisuperframe Order (MO) and Beacon Order (BO).
 * These parameters affect the slot duration (and therefore the superframe duration),
 * the number of superframes per multisuperframe and the beacon interval.
 *
 * The DSME standard defines the slot duration as
 * `slot_duration = aBaseSlotDuration * aSymbolDuration * 2^SO`. and the superframe
 * duration as "sf_duration = 16 * slot_duration".
 * `aBaseSlotDuration=60` as per standard and `aSymbolDuration` (us) depends on the
 * underlying PHY (16 for O-QPSK).
 *
 * The number of superframes per multisuperframe is given by
 * `sf_per_msf=2^(MO-SO)`.
 *
 * The beacon interval (in number of multisuperframes) is given by
 * `msf_per_bi=2^(BO-SO)`
 *
 * For example, an O-QPSK PHY with SO=3, MO=4 and BO=5 renders a slot duration
 * of 7.68 ms, a superframe duration of 122.8 ms, 2 superframes per multisuperframe
 * (msf_duration=245.7 ms) and a beacon interval of 2 multisuperframes
*  (beacon_interval=492 ms).
 *
 * The slot duration trades off maximum frame length and transmission delay during
 * GTS transmissions. A value equal or higher than 3 allows transmission of 127 bytes
 * frames.
 * Increasing the number of superframes per multisuperframe increases
 * on one hand transmission delay, because the period of a single GTS is the duration of the
 * multisuperframe. On the other hand it decreases energy consumption, because a
 * device transmit less often.
 * Increasing the beacon interval extends the number of coordinators in wireless
 * reach and reduces energy consumption. However, it slows the scanning and joining
 * procedure.
 *
 * The configuration of SO, MO and BO must be compliant with:
 * ```
 * 0 <= SO <= MO <= BO < 15
 * ```
 *
 * ## CAP Reduction
 *
 * With the purpose of reducing energy consumption and extending the GTS
 * resources even more, the MAC defines a CAP Reduction mode that turns
 * all CAP (except the first one in a multisuperframe) into CFP. This adds
 * 8 GTS per superframe that reduces CAP.
 *
 * With CAP Reduction on, increasing the number of superframes per multisuperframe
 * improves energy efficiency, because nodes spend less time on CAP. However,
 * note that this renders less CAP time which is required for slot negotiation.
 *
 * ## GTS coordinates
 *
 * A slot is defined by a superframe ID, a superframe slot and
 * a channel offset. The superframe ID is contained in `{0..n-1}`, where
 * `n` is the number of superframes per multisuperframe. The ID of the first superframe
 * (the one aligned with the start of a multisuperframe) is 0.
 *
 * The superframe slot values depend on the superframe ID.
 * ```
 * sf_slot = {0..6} if superframe_ID==0
 * sf_slot = {8..14} if superframe_ID!=0 && cap_reduction=0
 * sf_slot = {0..14} if superframe_ID!=0 && cap_reduction=1
 * ```
 *
 * The channel offset is `{0..m-1}`, with `m` the number of channels.
 *
 * # Usage in RIOT
 *
 * The RIOT implementation uses the `DSMEAdaptionLayer` wrapper internally (provided by
 * openDSME) to manage scanning/joining and automatic slot allocation. The MAC
 * implementation defines several scheduling strategies, but the default is
 * used (Traffic-aware and Predictive Scheduling).
 * The port introduces an openDSME GNRC Network Interface, that can be used via
 * @ref net_gnrc_netapi and @ref net_gnrc_netreg.
 *
 * The following network options (@ref net_netopt) are used for DSME setup.
 *
 * Configuration:
 * - @ref NETOPT_PAN_COORD. Set PAN coordinator or Child role
 * - @ref NETOPT_GTS_ALLOC. Allocate a GTS slot manually (only available via
 *   @ref CONFIG_IEEE802154_DSME_STATIC_GTS).
 * - @ref NETOPT_GTS_TX. Set the next transmission to either GTS or CSMA-CA.
 * - @ref NETOPT_ACK_REQ. Set the ACK Request bit to enable confirmed
 *   transmissions.
 * - @ref NETOPT_LINK. Start the DSME network (PAN Coordinator) or join an
 *   existing network (Child).
 *
 * ## MAC configuration
 *
 */
#ifndef OPENDSME_OPENDSME_H
#define OPENDSME_OPENDSME_H

#include <stdbool.h>
#include "byteorder.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/pktbuf.h"

/**
 * @brief default GNRC Pktsnip type for openDSME packets
 */
#ifndef CONFIG_OPENDSME_GNRC_PKTSNIP_TYPE
#define CONFIG_OPENDSME_GNRC_PKTSNIP_TYPE   GNRC_NETTYPE_UNDEF
#endif

/**
 * @brief maximum number of DSME neighbours
 */
#ifndef CONFIG_OPENDSME_MAX_NEIGHBOURS
#define CONFIG_OPENDSME_MAX_NEIGHBOURS      (20U)
#endif

/**
 * @brief maximum number of lost beacons before assuming the device desynchronized.
 *
 * Sets the maximum number of lost beacons before the MAC triggers a
 * de-association procedure. Higher values are beneficial in noisy
 * environments, because the MAC will keep synchronization despite losing some
 * beacons. However, lower values are better for mobile nodes, because devices
 * may sinchronize faster to a new coordinator.
 */
#ifndef CONFIG_OPENDSME_MAX_LOST_BEACONS
#define CONFIG_OPENDSME_MAX_LOST_BEACONS    (8U)
#endif

/**
 * @brief DSME CAP queue size
 *
 * The CAP queue stores frames to be sent during the Contention Access Period
 * using CSMA-CA. Because the transmission delay of CSMA-CA is lower compared to
 * GTS transmissions, small values are preferred to reduce memory requirements.
 */
#ifndef CONFIG_OPENDSME_CAP_QUEUE_SIZE
#define CONFIG_OPENDSME_CAP_QUEUE_SIZE    (8U)
#endif

/**
 * @brief DSME CFP queue size (for GTS transmissions)
 *
 * The CFP queue stores frames to be sent during the Contention Free Period
 * using a dedicated GTS. In contrast to CSMA-CA transmissions, GTS transmission
 * take longer as a result of slot schedules. Therefore, the GTS queue should have
 * more capacity than the CAP queue (@ref CONFIG_OPENDSME_CAP_QUEUE_SIZE)
 */
#ifndef CONFIG_OPENDSME_CFP_QUEUE_SIZE
#define CONFIG_OPENDSME_CFP_QUEUE_SIZE    (22U)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DSME Allocation descriptor
 */
typedef struct {
    network_uint16_t addr;  /**< neighbour address */
    uint8_t superframe_id;  /**< superframe ID */
    uint8_t slot_id;        /**< slot ID */
    uint8_t channel_id;     /**< channel ID */
    bool tx;                /**< whether the GTS is TX */
} ieee802154_dsme_alloc_t;

/**
 * @brief   Creates an openDSME network interface
 *
 * @param[out] netif    The interface. May not be `NULL`.
 * @param[in] stack     The stack for the network interface's thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the network interface's thread.
 * @param[in] name      Name for the network interface. May be NULL.
 * @param[in] dev       Device for the interface
 *
 * @see @ref gnrc_netif_create()
 *
 * @return  0 on success
 * @return  negative number on error
 */
int gnrc_netif_opendsme_create(gnrc_netif_t *netif, char *stack, int stacksize,
                               char priority, const char *name, netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* OPENDSME_OPENDSME_H */

/** @} */
