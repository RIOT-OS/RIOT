/*
 * Copyright (C) 2018 University of California, Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       CSMA Implementation for AT86RF2xx without Deaf Listening
 *
 * @author      Sam Kumar <samkumar@cs.berkeley.edu>
 */

#ifndef AT86RF2XX_CSMA_H
#define AT86RF2XX_CSMA_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the Software CSMA module for AT86RF2XX.
 *
 * @param[in] dev       device
 */
void at86rf2xx_csma_init(at86rf2xx_t* dev);

/**
 * @brief   Load data into the software CSMA buffer.
 *
 * @param[in] dev       device to use
 * @param[in] frame     pointer to buffer containing data
 * @param[in] frame_len length of the buffer containing data
 * @param[in] offset    offset at which to write to the buffer
 *
 * @return              returns 0 on success or a negative error code on failure
 */
int at86rf2xx_csma_load(at86rf2xx_t* dev, uint8_t* frame, size_t frame_len, size_t offset);

/**
 * @brief   Initiate a send transaction link retries and CSMA for each link
 *          retry
 *
 * @param[in] dev            device to use
 * @param[in] num_link_tries number of link-level tries to perform (0 means use
 *                           default value, AT86RF2XX_SOFTWARE_LINK_MAX_TRIES)
 */
void at86rf2xx_csma_send(at86rf2xx_t* dev, uint8_t num_link_tries);

/**
 * @brief   Informs the CSMA module that a CSMA probe was successful: the
 *          channel was clear and the frame was transmitted.
 *
 * @param[in] dev       device to use
 */
void at86rf2xx_csma_csma_try_succeeded(at86rf2xx_t* dev);

/**
 * @brief   Informs the CSMA module that a CSMA probe was unsuccessful: the
 *          channel was not clear, and therefore the frame was not transmtted.
 *
 * @param[in] dev       device to use
 *
 * @return              returns true if the CSMA module will perform another
 *                      CSMA probe, or false if we have run out of CSMA attempts
 */
bool at86rf2xx_csma_csma_try_failed(at86rf2xx_t* dev);

/**
 * @brief   Informs the CSMA module that a link transmission was successful:
 *          a link-layer acknowledgment was received for the transmitted packet.
 *
 * @param[in] dev       device to use
 */
void at86rf2xx_csma_link_tx_try_succeeded(at86rf2xx_t* dev);

/**
 * @brief   Informs the CSMA module that a link transmission was unsuccessful:
 *          a link-layer acknowledgment was not received for the transmitted
 *          packet.
 *
 * @param[in] dev       device to use
 *
 * @return              returns true if the CSMA module will perform another
 *                      link-layer attempt, or false if we have run out of
 *                      link-layer attempts. Note that each link-layer attempt
 *                      performs a separate round of CSMA
 */
bool at86rf2xx_csma_link_tx_try_failed(at86rf2xx_t* dev);

/**
 * @brief   Requests the CSMA module to send a frame, if a CSMA attempt is
 *          pending. When the CSMA timer expires, the netdev event callback will
 *          be triggered, so that is a good place to call this function. This
 *          may trigger the radio to send a packet, so it should not be called
 *          in interrupt context.
 *
 * @param[in] dev       device to use
 *
 * @return              returns false if a packet was pending, but could not be
 *                      sent because the radio was busy.
 *
 */
bool at86rf2xx_csma_probe_and_send_if_pending(at86rf2xx_t* dev);

/**
 * @brief   Cancels a pending CSMA attempt. This is only effective if the CSMA
 *          timer has expired, but the CSMA attempt has not been made.
 *
 * @param[in] dev       device to use
 *
 * @return              returns true if a CSMA attempt was actually pending, or
 *                      false if there was no CSMA attempt pending (in which
 *                      case, nothing happened).
 */
bool at86rf2xx_csma_clear_pending(at86rf2xx_t* dev);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_CSMA_H */
/** @} */
