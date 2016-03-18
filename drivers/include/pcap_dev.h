/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_pcap_dev    PCAP device
 * @ingroup     drivers_netdev_netdev2
 * @brief       Network device that generates and parses PCAP data
 * @brief       https://wiki.wireshark.org/Development/LibpcapFileFormat
 * @{
 *
 * @file
 * @brief   PCAP device definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef PCAP_DEV_H_
#define PCAP_DEV_H_

#include <stdint.h>

#include "net/netdev2.h"
#include "net/netdev2/ieee802154.h"
#include "timex.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   PCAP magic number
 * @see https://wiki.wireshark.org/Development/LibpcapFileFormat#Global_Header
 *
 * This number is used to identify the byte-order of the PCAP file
 */
#define PCAP_MAGIC_NUMBER           (0xa1b2c3d4U)
#define PCAP_VERSION_MAJOR          (2U)    /**< major version used for @ref pcap_hdr_t */
#define PCAP_VERSION_MINOR          (4U)    /**< minor version used for @ref pcap_hdr_t */

/**
 * @name PCAP network types
 * @see http://www.tcpdump.org/linktypes.html
 * @{
 */
#define PCAP_NET_TYPE_ETHERNET      (1U)    /**< Ethernet */
#define PCAP_NET_TYPE_IPV6          (229U)  /**< Raw IPv6 traffic */
#define PCAP_NET_IEEE802154_NOFCS   (230U)  /**< IEEE 802.15.4 without trailing FCS */
/** @} */

/**
 * @brief   PCAP header type
 * @see https://wiki.wireshark.org/Development/LibpcapFileFormat#Global_Header
 */
typedef struct __attribute__((packed)) {
    uint32_t magic_number;  /**< magic number (always @ref PCAP_MAGIC_NUMBER) */
    uint16_t version_major; /**< major version number */
    uint16_t version_minor; /**< minor version number */
    int32_t this_zone;      /**< GMT to local correction */
    uint32_t sigfigs;       /**< accuracy of timestamps */
    uint32_t snaplen;       /**< max length of captured packets, in octets */
    uint32_t net;           /**< data link type */
} pcap_hdr_t;

/**
 * @brief   PCAP record (packet) header
 * @see https://wiki.wireshark.org/Development/LibpcapFileFormat#Record_.28Packet.29_Header
 */
typedef struct __attribute__((packed)) {
    timex_t timestamp;      /**< timestamp in seconds since 1970-01-01 00:00:00 GMT */
    uint32_t incl_len;      /**< number of octets of packet saved in file */
    uint32_t orig_len;      /**< actual length of packet */
} pcaprec_hdr_t;

/**
 * @brief   Forward definition for device type
 */
typedef struct pcap_dev pcap_dev_t;

/**
 * @brief   General sink/drain format for a PCAP device
 *
 * The @ref drivers_pcap_dev
 */
typedef struct {
    /**
     * @brief   Initializes the sink/drain for a PCAP device
     *
     * @param[in] dev       Device descriptor for the @ref drivers_pcap_dev.
     * @param[in] hdr       PCAP general header to write to sink.
     *
     * @return  0 on success.
     * @return  <=0 on error.
     */
    int (*init)(pcap_dev_t *dev, const pcap_hdr_t *hdr);

    /**
     * @brief   Called by the @ref drivers_pcap_dev to write data to a sink.
     *
     * @param[in] dev       Device descriptor for the @ref drivers_pcap_dev.
     * @param[in] data      Data to output to the sink (record header + data of
     *                      record afterwards).
     *
     * @return  bytes of @p data actually written to sink on success.
     * @return  0, if no sink was specified for @p dev.
     * @return  <=0 on error.
     */
    int (*output)(pcap_dev_t *dev, const void *data, size_t data_len);

    /**
     * @brief   Called by the @ref drivers_pcap_dev to get data from a drain.
     *
     * This function will block until data was received.
     *
     * @param[in] dev       Device descriptor for the @ref drivers_pcap_dev.
     * @param[out] data     Data gotten from drain (@ref pcaprec_hdr_t + data
     *                      of record afterwards).
     *                      May be NULL to get timestamp of first packet from
     *                      drain.
     * @param[in] max_data  Maximum number of bytes available at @p data.
     * @param[out] next     Timestamp of next frame. Will be of value
     *                      `{ 0U, 0U }` if next frame is not available (yet).
     *
     * @return  bytes of @p data gotten from drain on success.
     * @return  0, if no drain was specified for @p dev.
     * @return  <=0 on error in general.
     * @return  -EINVAL, if data from drain was not of the expected PCAP format.
     */
    int (*input)(pcap_dev_t *dev, pcaprec_hdr_t *data, size_t max_data,
                 timex_t *next);
} pcap_dev_sd_t;

/**
 * @brief   Function type for getting and setting device options
 */
typedef int (*pcap_dev_opt_fn_t)(netdev2_t *, netopt_t, void *, size_t);

/**
 * @brief @ref drivers_pcap_dev device descriptor.
 */
struct pcap_dev {
    /**
     * @brief   netdev2 super struct
     * @note    Always include biggest possible child of @ref netdev2.
     */
#ifdef MODULE_NETDEV2_IEEE802154
    netdev2_ieee802154_t netdev;
#else
    netdev2_t netdev;
#endif
    const pcap_dev_sd_t *sd;            /**< sink/drain */
    void *sd_state;                     /**< state of sink/drain */
    const pcap_dev_opt_fn_t *getters;   /**< Array of option getters (always of size
                                         *   @ref NETOPT_NUMOF */
    const pcap_dev_opt_fn_t *setters;   /**< Array of option setters (always of size
                                         *  @ref NETOPT_NUMOF */
    uint8_t *drain_buf;                 /**< buffer to temporarily store received
                                         *   frames */
    uint16_t drain_buf_size;            /**< size of pcap_dev_t::drain_buf */
    uint32_t net;                       /**< PCAP network type */
    xtimer_t drain_timer;               /**< timer for getting next frame from drain */
};

/**
 * @brief   Setup @ref drivers_pcap_dev.
 *
 * @pre getters and setters must be of length NETOPT_NUMOF.
 *
 * @param[in] dev               A new @ref drivers_pcap_dev.
 * @param[in] sd                sink/drain for @p dev.
 * @param[in] sd_state          state of sink/drain for @p dev.
 * @param[in] drain_delay       Time in microseconds to wait for reading the first
 *                              frame from drain.
 * @param[in] drain_buf         Buffer to temporarily store received frames.
 *                              Should be of maximum frame length for the chosen
 *                              PCAP network type.
 * @param[in] drain_buf_size    Size of @p drain_buf.
 * @param[in] net           PCAP network type for @p dev.
 * @param[in] getters       Array of option getters. Must be of length
 *                          @ref NETOPT_NUMOF.
 * @param[in] setters       Array of option getters. Must be of length
 *                          @ref NETOPT_NUMOF.
 */
void pcap_dev_setup(pcap_dev_t *dev, const pcap_dev_sd_t *sd, void *sd_state,
                    uint32_t drain_delay, uint8_t *drain_buf,
                    uint16_t drain_buf_size, uint32_t net,
                    const pcap_dev_opt_fn_t getters[],
                    const pcap_dev_opt_fn_t setters[]);

#ifdef __cplusplus
}
#endif

#endif /* PCAP_DEV_H_ */
/** @} */
