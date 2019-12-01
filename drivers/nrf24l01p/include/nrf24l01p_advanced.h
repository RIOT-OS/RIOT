/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief   Advanced API for NRF24L01P transceivers
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef NRF24L01P_ADVANCED_H
#define NRF24L01P_ADVANCED_H

#include <stdlib.h>

#include "nrf24l01p_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Write payload to be transmitted in an ACK frame
 *
 *          The ACK payload is flushed if a MAX_RT interrupt occurs.
 *          The ACK payload must be set in advance of the reception of a frame.
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 * @param[in] pipe          Pipe index
 *
 * @retval 0                Success
 * @retval -ENOTSUP         Protocol is SB
 * @retval -EINVAL          Payload too big
 * @retval -ERANGE          Bad pipe index
 * @retval -EAGAIN          Current state does not permit setting ACK payload
 */
int nrf24l01p_set_ack_payload(nrf24l01p_t *dev, const void *payload,
                              size_t payload_width, nrf24l01p_pipe_t pipe);

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 0
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p0(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P0);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 1
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p1(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P1);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 2
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p2(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P2);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 3
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p3(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P3);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 4
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p4(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P4);
}

/**
 * @brief   Wrapper around @see nrf24l01p_set_ack_payload to write ACK paylaod
 *          for pipe 5
 *
 * @param[in] dev           NRF24L01P device handle
 * @param[in] payload       Payload
 * @param[in] payload_width Payload width
 *
 * @return                  @see nrf24l01p_set_ack_payload
 */
static inline int nrf24l01p_set_ack_payload_p5(nrf24l01p_t *dev,
                                               const void *payload,
                                               size_t payload_width)
{
    return nrf24l01p_set_ack_payload(dev, payload, payload_width, NRF24L01P_P5);
}

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_ADVANCED_H */
/** @} */
