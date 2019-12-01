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
 * @brief   Implementation of the public NRF24L01P device interface
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <assert.h>
#include <string.h>

#include "nrf24l01p_constants.h"
#include "nrf24l01p_internal.h"
#include "nrf24l01p_communication.h"
#include "nrf24l01p_custom_header.h"
#include "nrf24l01p_advanced.h"

int nrf24l01p_set_ack_payload(nrf24l01p_t *dev, const void *payload,
                              size_t payload_width, nrf24l01p_pipe_t pipe)
{
    assert(dev);
    assert(payload);
    if (payload_width > NRF24L01P_MTU) {
        return -EINVAL;
    }
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return -ENOTSUP;
    }
    if (pipe >= NRF24L01P_PX_NUM_OF) {
        return -ERANGE;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
#if NRF24L01P_CUSTOM_HEADER
    uint8_t aw = nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);
    uint8_t pl[1 + aw + payload_width];
    sb_hdr_init((shockburst_hdr_t *)pl);
    sb_hdr_set_src_addr_width((shockburst_hdr_t *)pl, aw);
    if (pipe == NRF24L01P_P0) {
        memcpy(pl + 1, dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr, aw);
    }
    else {
        memcpy(pl + 1, dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr, aw);
        if (pipe > NRF24L01P_P1) {
            pl[aw] = dev->params.urxaddr.arxaddr.rx_addr_short[pipe - 2];
        }
    }
    memcpy(pl + 1 + aw, payload, payload_width);
    payload = pl;
    payload_width = sizeof(pl);
#endif
    nrf24l01p_acquire(dev);
    nrf24l01p_write_ack_pl(dev, payload, payload_width, pipe);
    nrf24l01p_release(dev);
    return 0;
}
