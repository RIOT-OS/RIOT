/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_contikimac ContikiMAC compatible MAC layer
 * @ingroup     net
 * @brief       Duty cycling MAC protocol for low power communication over IEEE 802.15.4 networks
 *
 * @see Dunkels, A. (2011). The contikimac radio duty cycling protocol.
 *      http://soda.swedish-ict.se/5128/1/contikimac-report.pdf
 * @see Michel, M., & Quoitin, B. (2014). Technical report: ContikiMAC vs X-MAC
 *      performance analysis. arXiv preprint arXiv:1404.3589. https://arxiv.org/abs/1404.3589
 *
 * # Summary
 *
 * ContikiMAC is a duty cycling MAC layer protocol which uses strobing (repeated
 * transmissions) of actual data packets to ensure that sleeping nodes will
 * receive the transmission as they wake up. This is similar to the X-MAC
 * protocol, except X-MAC uses special strobe frames to signal incoming data
 * instead of the actual data frame.
 *
 * \note This implementation supports the ContikiMAC fast sleep optimization, and
 *       the ContikiMAC burst optimization (frame pending field)
 *
 * \todo Add support for the ContikiMAC phase lock optimization
 *
 * # Algorithm description
 *
 * ContikiMAC nodes wake the radio at a regular interval,
 * and performs a number of CCA checks to check the radio medium for energy. If
 * one of the CCA checks report that the medium is busy, the radio is switched
 * to listening mode and waiting for incoming packets. On the other hand, if all
 * of the CCA checks report channel idle, the radio is put back to sleep.
 * When listening, if no packet is received before a timeout is hit, the radio
 * is put back to sleep.
 * If a packet is received correctly, the radio is put back to sleep immediately,
 * unless more data is expected.
 *
 * ## Optimizations
 *
 * The MAC layer can use knowledge of the algorithm behavior to further reduce
 * the power usage.
 *
 * ### Fast sleep
 *
 * If a CCA check signals channel busy, but further CCA checks do not detect any
 * silence after the time it takes to transmit the longest possible packet, the
 * MAC layer will assume that the detected radio energy is noise from some other
 * source, e.g. WiFi networks or microwave ovens, and put the radio back to sleep.
 *
 * If a CCA check signals channel busy, and then some silence is detected, but
 * there is no reported incoming reception from the radio after the strobe
 * interval has passed, the radio is put back to sleep.
 *
 * ### Burst transmission
 *
 * If a sender has more data than fits in a single frame, a special flag is used
 * to tell the receiver to keep the radio turned on and listening after the
 * current frame has been received. For IEEE 802.15.4, the Frame Pending flag in
 * the Frame Control Field is used for this purpose. This optimization increases
 * network throughput if used correctly. The 6lowpan fragmentation module
 * automatically tells the MAC layer to set this flag on any fragmented packets,
 * but it can also be used from the application layer if the application knows
 * that there will be more data immediately after the current.
 *
 * ### Expecting immediate replies
 *
 * When a frame is successfully delivered, an extra channel check will be
 * scheduled by the sender at a short time ahead in an attempt to catch an
 * immediate reply from the other node. This interval is currently hard-coded at
 * approximately 8 ms, but should be made configurable in a future enhancement.
 * This method should in theory reduce communication latency and reduce energy
 * consumption of the remote node by reducing the number of transmission retries
 * required. Experiments seem to indicate that this is theory works in real
 * applications too, but this has not been statistically proven yet.
 *
 * ### Phase lock
 *
 * \note Not yet implemented in netdev contikimac
 *
 * If a unicast transmission succeeds with a correctly received Ack packet, the
 * sender can record the time of the last transmission start. The next time a
 * unicast packet is directed to the same destination, the registered phase will
 * be used as a reference, and the tranmission will be started right before the
 * receiver is expected to wake up.
 *
 * # Supported netopt settings
 *
 * The ContikiMAC layer will behave slightly differently when certain NETOPT_xxx
 * options are set on the interface. A summary of supported options is found
 * below.
 *
 * ## NETOPT_STATE
 *
 * NETOPT_STATE will affect the duty cycling of the radio and can be set
 * manually via the shell command `ifconfig x set state mystate`, where `x` is
 * the interface id.
 *
 * A short description of each state follows:
 *
 * - NETOPT_STATE_IDLE - Normal duty cycled operation, the radio is brought out
 *      of sleep mode at regular intervals to perform channel checks and will go
 *      back to sleep after the channel check is done if no activity was detected.
 *
 * - NETOPT_STATE_SLEEP, NETOPT_STATE_STANDBY, NETOPT_STATE_OFF - Suspended
 *      radio duty cycling, the radio will remain in the chosen state and will
 *      not perform channel checks. Sending frames will wake the radio and
 *      the extra channel checks introduced by the immediate reply optimization
 *      will still be performed. This allows the node to send a message and
 *      hopefully receive an immediate reply.
 *
 * ## NETOPT_MAC_NO_SLEEP
 *
 * The NETOPT_MAC_NO_SLEEP option will disable any duty cycling and cause the
 * radio to remain on between frame receptions. Sending will still use strobing
 * to be able to communicate with other ContikiMAC nodes with radio duty cycling
 * enabled. This option is useful on border routers or other permanently powered
 * devices because it can let other nodes in the network conserve power by
 * reducing the number of retransmissions.
 *
 * ## NETOPT_TX_START_IRQ, NETOPT_TX_END_IRQ, NETOPT_RX_START_IRQ, NETOPT_RX_END_IRQ
 *
 * The ContikiMAC layer will filter IRQ events from the underlying device and
 * only pass events to the upper layers when these flags are enabled. The events
 * @ref NETDEV_EVENT_RX_STARTED, @ref NETDEV_EVENT_RX_COMPLETE are passed on
 * whenever received if the @ref NETOPT_RX_START_IRQ and @ref NETOPT_RX_END_IRQ
 * options are set. @ref NETDEV_EVENT_TX_STARTED, @ref NETDEV_EVENT_TX_COMPLETE
 * are sent before the start of TX strobing and after the TX strobe has
 * completed, respectively. This means that there will only be one
 * @ref NETDEV_EVENT_TX_STARTED, and one @ref NETDEV_EVENT_TX_COMPLETE event,
 * regardless of the number of MAC layer retransmissions, since retransmissions
 * are an integral part of the ContikiMAC algorithm.
 *
 * # Implementation details
 *
 * This section gives some extra information regarding this specific
 * implementation. This implementation was inspired by Contiki code, but written
 * from scratch to fit RIOT and the GNRC stack. All algorithmic details are
 * taken from [dunkels11].
 *
 * The timing is handled by the xtimer system, this means that the platform
 * needs to use a low power timer for xtimer in order to use the low power modes
 * of the MCU. At the time of writing (2018-07-02), most boards' default
 * configurations are using high precision timers for xtimer (one notable
 * exception is the frdm-kw41z board), but many boards can be easily
 * reconfigured to use low power timers via board.h. Check your CPU manual for
 * the proper timer to use.
 *
 * All radio state switching, CCA checks, etc. are called via the netdev get/set
 * API. This makes the code platform independent, but the radio drivers need to
 * support the options used by the implementation. The options required for full
 * functionality are:
 *
 * - @ref NETOPT_PRELOADING, for loading the TX frame once and transmitting many times
 * - @ref NETOPT_STATE_TX, for triggering retransmission
 * - @ref NETOPT_STATE_STANDBY, for radio low power CCA checking
 * - @ref NETOPT_STATE_SLEEP, for radio low power mode
 * - @ref NETOPT_STATE_IDLE, for radio RX listen
 * - @ref NETOPT_CSMA, to disable hardware CSMA, not required if the radio does not support CSMA
 * - @ref NETOPT_RETRANS, to disable automatic retransmissions
 * - @ref NETOPT_TX_END_IRQ, to be alerted about end of TX
 * - @ref NETOPT_RX_START_IRQ, to be alerted about preamble detections
 * - @ref NETOPT_RX_END_IRQ, to be alerted about received frames
 * - @ref NETOPT_IS_CHANNEL_CLR, for performing CCA checks
 *
 * Additionally, the radio must allow the same frame to be transmitted multiple
 * times. The implementation will switch the radio to standby before any TX
 * preloading, to avoid corrupting the TX buffer with incoming RX packets on
 * single buffered devices, e.g. at86rf2xx. The device driver must allow
 * multiple calls to @ref NETOPT_STATE_TX after a single preload, for retransmissions
 * while strobing.
 * @ref NETOPT_IS_CHANNEL_CLR, @ref netdev_driver::send(), and
 * @ref NETOPT_STATE_TX are called while the radio is in @ref NETOPT_STATE_STANDBY.
 *
 * ## Collision avoidance
 *
 * The implementation does not perform CCA checks before every retransmission,
 * which may lead to collisions in crowded environments. The default timing
 * parameters need to be adjusted if CSMA/CA is going to be enabled for the
 * device.
 *
 * Before the first transmission of a strobe, the radio will perform CCA checks
 * in the same way as the channel check procedure does, in an attempt to avoid
 * collisions, but no automatic retry or random backoff is implemented.
 *
 * ## Fast sleep
 *
 * During a wake up, the fast sleep implementation will perform additional
 * periodic CCA checks after the first energy detection on the channel. The
 * periodic CCA checks continue until either an idle channel is detected, or a
 * timeout occurs. The timeout must be greater than the time it takes to
 * transmit the longest possible frame, or else it may time out before seeing
 * the end of a packet if the first ED occurred right at the beginning of the
 * frame. After silence is detected, the radio is switched to listening state,
 * and a new timeout is set. If an RX begin event occurs before the timeout, the
 * timeout is incremented to the length of the longest frame, to allow for the
 * complete reception. If the timeout is hit, the radio is put back to sleep. No
 * further CCA checks are performed after switching the radio to listening state,
 * to avoid interfering with the frame reception.
 *
 * ## Burst reception
 *
 * The frame pending field of the IEEE 802.15.4 Frame Control Field is checked
 * on all received frames to determine whether to keep the radio on or to put it
 * back to sleep.
 *
 * ## Expecting immediate replies after transmission
 *
 * An extra channel check is scheduled outside of the regular schedule after a
 * transmission is completed successfully (Ack received if acknowledgment was
 * requested). This allows the sender to catch any immediate replies to the
 * message which reduces latency in simple network scenarios at the cost of one
 * more channel check.
 *
 * # ContikiMAC Timing constraints
 *
 * In order to ensure reliable transmissions while duty cycling the receiver,
 * there are some constraints on the timings for the ContikiMAC algorithm.
 *
 * @note These constraints are also given in [dunkels11], but written with an
 *       implicit \f$n_c = 2\f$.
 *
 * To reliably detect Ack packets:
 *
 * \f[T_a + T_d < T_i\f]
 *
 * To reliably detect incoming packets during CCA cycles:
 *
 * \f[T_i < (n_c - 1) \cdot T_c + n_c \cdot T_r\f]
 *
 * and
 *
 * \f[(T_c + 2 T_r) < T_s\f]
 *
 * The variables in the above conditions are described below:
 *
 * \f$T_a\f$ is the time between reception end and Ack TX begin
 *
 * \f$T_d\f$ is the time it takes for the transceiver to receive the Ack packet
 *
 * \f$T_i\f$ is the time between the end of transmission and the start of retransmission
 *
 * \f$T_c\f$ is the time between CCA checks during CCA cycles
 *
 * \f$n_c\f$ is the maximum number of CCA checks to perform during the CCA cycle
 *
 * \f$T_r\f$ is the time it takes to perform one CCA check
 *
 * \f$T_s\f$ is the time it takes to transmit the shortest allowed frame
 *
 * The constraint on \f$T_s\f$ yields a minimum packet length in bytes:
 *
 * \f[T_s = n_s \cdot T_b \Leftrightarrow n_s = \frac{T_s}{T_b}\f]
 *
 * where \f$n_s\f$ is the number of bytes in the shortest packet, and \f$T_b\f$
 * is the time it takes to transmit one byte.
 *
 * For packets shorter than \f$n_s\f$ bytes, extra padding must be added to ensure
 * reliable transmission, or else the packet may fall between two CCA checks and
 * remain undetected.
 *
 * From the above equations it can be seen that using \f$n_c > 2\f$ relaxes the
 * constraint on minimum packet length, making it possible to eliminate the extra
 * frame padding completely, at the cost of additional CCA checks.
 *
 * Remember that the software and hardware may create additional delays which
 * need to be accounted for. In particular, the inter frame interval \f$T_i\f$
 * may become longer than specified due to hardware TX warm up and other delays
 * which means that the time that the software triggers a transmission is no the
 * same time that a transmission can be detected by a remote node. The default
 * timing configuration uses \f$n_c = 4\f$ to reduce the risk of failing to
 * detect a transmission. Using 3 CCA checks seems fine on paper, but empirical
 * results show that there is a significantly better rate of reception when
 * using 4 CCA checks for the channel check event.
 *
 * ## Fast sleep
 *
 * For fast sleep, some additional timing information is needed. \f$T_l\f$, the
 * time to transmit the longest possible frame is a lower limit for timeouts in
 * the fast sleep optimization.
 *
 * The interval between CCA checks during the fast sleep silence detection must
 * be less than \f$T_i\f$ in order to be able to reliably sample the silence
 * between two transmissions.
 *
 * ## Example timing parameters for O-QPSK 250 kbit/s
 *
 * O-QPSK 250 kbit/s is the most widely used mode for 802.15.4 radios in the 2.4 GHz band.
 *
 * \f[T_a = 12~\mathrm{symbols} = 192~\mathrm{\mu{}s}\f]
 *
 * \f$T_a\f$ is defined in IEEE 802.15.4 (AIFS = macSifsPeriod = aTurnaroundTime)
 *
 * \f[T_d = 5 + 1 + 5~\mathrm{bytes} = 352~\mathrm{\mu{}s}\f]
 *
 * A standard Ack packet is 5 bytes long, the preamble and start-of-frame
 * delimiter (SFD) is 5 bytes, and the PHY header (PHR) is 1 byte.
 *
 * \f[T_r = 8~\mathrm{symbols} = 128~\mathrm{\mu{}s}\f]
 *
 * \f$T_r\f$ is defined in IEEE 802.15.4 (aCcaTime)
 *
 * \f[T_b = 2~\mathrm{symbols} = 32~\mathrm{\mu{}s}\f]
 *
 * \f$T_b\f$ is derived from definitions in IEEE 802.15.4 (4 bits per symbol)
 *
 * \f[T_l = T_b \cdot (5 + 1 + 127) = 4320~\mathrm{\mu{}s}\f]
 *
 * The longest possible payload is 127 bytes, SFD+preamble is 5 bytes, PHR is 1
 * byte.
 *
 * Additionally, the hardware may have some timing constraints as well. For
 * example, the at86rf2xx transceiver has a fixed Ack timeout (when using
 * hardware Ack reception) of 54 symbols (\f$864~\mathrm{\mu{}s}\f$), this means
 * that the configuration must satisfy \f$T_i > 864~\mathrm{\mu{}s}\f$ if
 * using a at86rf2xx transceiver.
 *
 * Due to CPU processing constraints, there are lower limits on all timings. For
 * example, the reception and CCA check results need to be processed by the CPU
 * and passed to the ContikiMAC thread, which may not be an insignificant time
 * depending on the CPU speed and the radio interface bus speed (SPI, UART etc.).
 *
 * # Configuring timing parameters
 *
 * The timing parameters are set using an instance of contikimac_params_t.
 * Some parameters can be automatically derived from the other parameters. The
 * constants that must be configured manually for a minimum working configuration
 * are:
 *
 * - @c cca_count_max =\f$n_c\f$, the maximum number of CCA checks in each wake up
 * - @c inter_packet_interval =\f$T_i\f$, interval between retransmissions
 * - @c cca_cycle_period =\f$T_c\f$, the time between consecutive CCA checks
 * - @c after_ed_scan_timeout >\f$T_l\f$, time to keep checking for silence after detecting energy
 * - @c after_ed_scan_interval <\f$T_i\f$, interval between CCA checks after detecting energy
 * - @c rx_timeout =\f$T_l\f$, time to transmit the longest possible frame
 *
 * Default configurations are provided for some common 802.15.4 radio modes:
 * O-QPSK250, O-QPSK100, BPSK40, BPSK20
 * These parameters are automatically selected at run time based on the radio mode.
 *
 * The following parameters are not dependent on the radio bit rate or mode and
 * can be reconfigured at run time:
 *
 * - @c channel_check_period =\f$T_w\f$, the time between wake ups
 *
 * # Suggested topics for future improvements
 *
 * Some areas of improvement for this implementation are listed below.
 *
 * ## Collision avoidance
 *
 * Enabling CSMA/CA should be trivial, however, the timings need to account for
 * the added latency on every retransmission. In particular, the backoff
 * exponents (macMinBE, macMaxBE) should be reduced from their defaults (3, 5).
 *
 * The parameters @c cca_count_max, @c cca_cycle_period will need to be
 * incremented to allow for a longer \f$T_i\f$.
 * \f[T_i = \mathtt{inter\_packet\_interval} + T_{CSMA}\f]
 *
 * ## Phase lock optimization
 *
 * The phase of the Acked packet must be recorded and remembered for each
 * unicast destination. For each unicast packet, the send function will then
 * look up the registry for the correct timing to use for the particular
 * destination.
 *
 * ## Automatic noise level calibration
 *
 * The CCA process is critical for the correct operation of the ContikiMAC
 * protocol. If the CCA threshold is too low, then noise will trigger wake ups
 * all the time, but the fast sleep optimization will put the radio back to
 * sleep after the @c after_ed_scan_timeout interval has passed. If the CCA
 * threshold is too high, then real frames will fail to be detected and the
 * radio will be put back to sleep immediately. Both faults result in a failure
 * to receive incoming frames.
 *
 * By measuring the RSSI level periodically it should be possible to dynamically
 * adjust the CCA threshold to ensure the best possible chance of successfully
 * detecting real incoming traffic.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the ContikiMAC layer
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef NET_CONTIKIMAC_H
#define NET_CONTIKIMAC_H

#include <stdint.h>
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "event.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief (usec) Default time between wake ups, @ref contikimac_t::channel_check_period
 *
 * This is the default interval between periodic wake ups for checking the
 * channel for energy.
 *
 * This time period is also used as the maximum time to keep strobing
 * (retransmitting) outgoing packets.
 *
 * For unicast, this is the maximum time to keep retransmitting before
 * giving up. Strobing will stop earlier if an Ack packet arrives before
 * this time has passed.
 *
 * For broadcast/multicast, each packet will always be retransmitted until
 * this time has passed.
 *
 * This period can be changed at runtime by setting a new value for the
 * NETOPT_MAC_CHECK_PERIOD setting.
 */
#ifndef CONTIKIMAC_DEFAULT_CHANNEL_CHECK_PERIOD
#define CONTIKIMAC_DEFAULT_CHANNEL_CHECK_PERIOD (1000000ul / 8)
#endif

/**
 * @brief (usec) Default time for @ref contikimac_t::burst_timeout
 *
 * This is the default value for the RX frame detection timeout while receiving
 * transmission bursts. The burst timeout should be kept low to avoid wasting
 * energy, but it has to be set long enough to allow the remote node to prepare
 * the next frame and begin transmission.
 */
#ifndef CONTIKIMAC_DEFAULT_BURST_TIMEOUT
#define CONTIKIMAC_DEFAULT_BURST_TIMEOUT        (1000000ul / 64)
#endif

/**
 * @brief (usec) Default time for @ref contikimac_t::reply_delay
 *
 * This is the default waiting time for immediate unicast replies. When the
 * contikimac module has sent a unicast frame, an extra channel check is injected
 * to catch any immediate replies from the remote node. This may improve network
 * throughput.
 */
#ifndef CONTIKIMAC_DEFAULT_REPLY_DELAY
#define CONTIKIMAC_DEFAULT_REPLY_DELAY          (8192u)
#endif

/**
 * @brief ContikiMAC configuration parameters
 */
typedef struct {
    /**
     * @brief (usec) time between successive CCA checks during wake ups
     *
     * This setting, together with cca_count_max, defines the detection
     * window for incoming traffic
     */
    uint32_t cca_cycle_period;
    /**
     * @brief (usec) interval to wait between each TX packet while strobing
     *
     * This time counts from when the end of TX is signaled from the device
     * driver.
     *
     * \note This interval must be long enough to allow an Ack packet to arrive
     * after transmitting.
     */
    uint32_t inter_packet_interval;
    /**
     * @brief (usec) maximum time to scan for channel idle after an energy detection
     *
     * Fast sleep optimization: After energy has been detected on the channel,
     * the MAC layer will keep scanning the channel until it sees some silence.
     *
     * For reliable communication, this must be at least as long as the time it
     * takes to transmit the longest possible frame.
     */
    uint32_t after_ed_scan_timeout;
    /**
     * @brief (usec) interval between successive CCA checks after an energy detection
     */
    uint32_t after_ed_scan_interval;
    /**
     * @brief (usec) time to wait while listening for incoming packets before
     * turning off the radio
     *
     * For reliable communication, this must be at least inter_packet_interval +
     * the time it takes for the radio to detect and signal the RX begin interrupt.
     */
    uint32_t listen_timeout;
    /**
     * @brief (usec) time to wait after an RX begin event before turning off the
     * radio
     *
     * For reliable communication, this must be at least as long as the time it
     * takes to transmit the longest possible frame.
     */
    uint32_t rx_timeout;
    /**
     * @brief Maximum number of times to perform CCA checks during a wake up
     * window
     *
     * This setting, together with cca_cycle_period, defines the detection
     * window for incoming traffic
     */
    uint8_t cca_count_max;
} contikimac_params_t;

/**
 * @brief   Event type for asynchronous events used internally
 */
typedef struct {
    event_t super;      /**< parent event class */
    void *ctx;          /**< pointer to ContikiMAC context */
} event_contikimac_t;

/**
 * @brief   ContikiMAC device descriptor
 */
typedef struct {
    /**
     * @brief   netdev parent struct
     */
    netdev_ieee802154_t dev;
    /**
     * @brief   Pointer to timing parameters
     */
    const contikimac_params_t *params;
    /**
     * @brief   Pointer to event queue in the upper layer for scheduling our events
     *
     * While listening, ContikiMAC will post events into this queue and the
     * upper layer will drain the queue.
     * While sending, ContikiMAC will post events but also run a small event
     * loop to be able to perform retransmissions. This means that ContikiMAC
     * will process all events in this queue, but without processing any IPC
     * messages from higher layers.
     */
    event_queue_t *evq;
    /**
     * @brief   (usec) Channel check period
     */
    uint32_t channel_check_period;
    /**
     * @brief   (usec) Timeout for RX frame detection while waiting for the next
     *          frame of a burst
     */
    uint32_t burst_timeout;
    /**
     * @brief   (usec) Unicast reply delay
     *
     * An extra channel check event outside of the normal schedule is inserted
     * at this interval after a unicast transmission has completed successfully.
     */
    uint32_t reply_delay;
    /**
     * @brief   Timers used internally to drive the radio duty cycle
     */
    struct {
        /**
         * @brief   Regular channel check interval timer
         */
        xtimer_t channel_check;
        /**
         * @brief   Extra channel check events outside of the normal schedule
         *
         * Set by the TX algorithm to catch replies to unicast transmissions
         * quicker than the regular channel check interval.
         */
        xtimer_t extra_channel_check;
        /**
         * @brief   Periodic updates after energy detection
         */
        xtimer_t periodic;
        /**
         * @brief   Timeouts used in various places in the algorithm
         */
        xtimer_t timeout;
    } timers;
    /**
     * @brief   Timestamp of last IRQ
     *
     * Used for timing inside the transmit loop
     */
    xtimer_ticks32_t last_irq;
    /**
     * @brief   Events used internally
     */
    struct {
        /**
         * @brief   Channel check event
         */
        event_contikimac_t channel_check;
        /**
         * @brief   Extra channel check event outside of the normal schedule
         */
        event_contikimac_t extra_channel_check;
        /**
         * @brief   Periodic state updates part of the RX algorithm
         */
        event_contikimac_t periodic;
        /**
         * @brief   Radio driver triggered interrupt requests
         */
        event_contikimac_t isr;
    } events;
    /**
     * @brief   flags for some netopt options
     */
    uint8_t flags;
    /**
     * @brief   Silence detected flag, part of the RX state machine
     */
    uint8_t seen_silence;
    /**
     * @brief   RX in progress flag
     */
    uint8_t rx_in_progress;
    /**
     * @brief   Timeout flag, set by cb_timeout
     */
    uint8_t timeout_flag;
    /**
     * @brief   TX progress, part of the TX algorithm state machine
     */
    uint8_t tx_status;
    /**
     * @brief   Emulated netdev state
     *
     * This is used when getting/setting the NETOPT_STATE value.
     * Provides a way to disable the radio completely and suspend the channel
     * check events when we need to conserve power.
     */
    uint8_t state;
} contikimac_t;

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t contikimac_driver;

/**
 * @brief   Setup the ContikiMAC descriptor
 *
 * @param[out]  ctx     ContikiMAC descriptor to initialize
 * @param[in]   lower   device descriptor for the underlying device
 */
void contikimac_setup(contikimac_t *ctx, netdev_t *lower);

#ifdef __cplusplus
}
#endif

#endif /* NET_CONTIKIMAC_H */
/** @} */
