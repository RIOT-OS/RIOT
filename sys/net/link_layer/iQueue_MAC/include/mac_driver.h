/*
 * Copyright (C) 2015  Zhejiang University, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup MAC
 * @{
 * @brief   Definition (API) for MAC/RDC drivers implemented in RIOT OS
 * @author  Shuguo Zhuo <zhuosg@zju.edu.cn>
 */

#ifndef MAC_DRIVER_H
#define MAC_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Return values for MAC/RDC-related functions
 */
typedef enum
{
    /** @brief No problem encountered */
    MAC_OK = 0,

    /** @brief Packet transmission deferred; callback function
               will indicate actual TX outcome in time */
    MAC_TX_DEFERRED,

    /** @brief A collision has prevented successful operation;
               user should retry later */
    MAC_COLLISION = 10,

    /** @brief No ACK packet was returned for the TXed packet;
               user should retry later */
    MAC_TX_NO_ACK,

    /** @brief All slots for outgoing packets (for TX) are already taken;
               user should retry later */
    MAC_TX_QUEUE_FULL,

    /** @brief The MAC/RDC driver is off */
    MAC_DRIVER_OFF,

    /** @brief There was not enough memory to perform the wanted operation */
    MAC_OUT_OF_MEM,

    /** @brief Another, unexpected error has prevented successful operation */
    MAC_ERROR = 100,

} mac_return_t;


/**
 * @brief Callback function definition for TX status notification
 *
 * @param[in] packet_seq_num  Sequence number of the TXed packet concerned
 * @param[in] callback_param  Arbitrary parameter provided for this callback
 * @param[in] status          Status of packet transmission
 * @param[in] num_tx          Number of attempted transmissions for that packet
 */
typedef void (* mac_tx_status_callback_t)(uint8_t packet_seq_num,
        void *callback_param,
        mac_return_t status,
        unsigned int num_tx);

/**
 * @brief Callback function definition for packet RX notification
 *
 * @param[in] buf Buffer containing the received packet
 *                in a raw format (untyped pointer to data).
 * @param[in] len Length, in bytes, of the raw data (packet)
 *                to transmit from @c buf @c.
 * @param[in] rssi Receive Signal Strength Indicator, in dB.
 * @param[in] lqi Linq Quality Indication.
 * @param[in] crc_ok @c true @c if this packet's CRC was successfully checked;
 *                   @c false @c otherwise.
 */
typedef void (* mac_rx_packet_notif_t)(void *buf, unsigned int len,
                                       uint8_t rssi, uint8_t lqi,
                                       bool crc_ok);


/**
 * @brief Definition of the structure of MAC & RDC protocol drivers for RIOT OS
 */
typedef struct MAC_driver
{
    /** @brief (Human-readable) Name of the MAC/RDC protocol */
    char * name;

    /**
     * @brief Initialization function for the driver.
     * @details To be called once at system boot time
     *          (e.g. by @c auto-init @c module).
     * @return a status value indicating how the operation proceeded.
     */
    mac_return_t (* init)(void);

    /**
     * @brief Turn driver on.
     * @details Call it when you want to actually use the corresponding
     *          MAC/RDC protocol. You can turn on and off MAC/RDC protocols
     *          anytime and as much as you want during system execution.
     * @return a status value indicating how the operation proceeded.
     */
    mac_return_t (* on)(void);

    /**
     * @brief Turn driver off.
     * @details Call it when you want to stop using the corresponding
     *          MAC/RDC protocol. You can turn on and off MAC/RDC protocols
     *          anytime and as much as you want during system execution.
     * @return a status value indicating how the operation proceeded.
     */
    mac_return_t (* off)(void);

    /**
     * @brief Indicates whether the driver is active (on) or not (off).
     * @return A @c true @c value if the MAC driver is currently active,
     *         a @c false @c value otherwise.
     */
    bool (* is_active)(void);

    /**
     * @brief Send a packet using this MAC/RDC protocol.
     * @details Call this function when you want to enqueue
     *          a packet for sending. Note that the protocol has
     *          full control as to when to actually send that packet.
     *
     * @param[in] buf Buffer containing the packet to transmit
     *                in a raw format (untyped pointer to data).
     * @param[in] len Length, in bytes, of the raw data (packet)
     *                to transmit from @c buf @c.
     * @param[out] tx_status_callback Callback function that the MAC/RDC
     *                                protocol invokes to notify the user
     *                                about the on-going status this packet's
     *                                transmission; since MAC/RDC protocols
     *                                normally delay packets transmission,
     *                                this callback function is the way
     *                                information about TX unfolding can
     *                                be eventually given back to user code.
     *                                If you don't care about TX outcome,
     *                                you can pass NULL as a value to this
     *                                parameter.
     * @param[out] tx_callback_param Arbitrary parameter to be passed to
     *                               the @c tx_callback_function @c when
     *                               this packet is concerned; can be NULL.
     *
     * @return a status value indicating how the operation proceeded
     *         in the instant. For information about long-term status
     *         of the transmission, use the @c tx_status_callback @c
     *         parameter.
     */
    mac_return_t (* send)(void *buf,
                          unsigned int len,
                          mac_tx_status_callback_t tx_status_callback,
                          void *tx_callback_param);

    /**
     * @brief Receive a packet from radio transceiver
     *        using this MAC/RDC protocol.
     *
     * @see mac_rx_packet_notif_t
     */
    void (* receive)(void *buf,
                     unsigned int len,
                     uint8_t rssi,
                     uint8_t lqi,
                     bool crc_ok);

    /**
     * @brief Define incoming packet callback function.
     * @details Define the callback function that the MAC/RDC protocol
     *          invokes when it has received a packet from network interface.
     */
    void (* set_input_func)(mac_rx_packet_notif_t rx_func);

} mac_driver_t;

/**
 * @}
 */
#endif /* MAC_DRIVER_H */

