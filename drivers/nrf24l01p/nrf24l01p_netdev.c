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
 * @brief   Implementation of RIOT's netdev_driver API
 *          for the NRF24L01P transceiver
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <errno.h>
#include <string.h>
#include <assert.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "iolist.h"
#include "irq.h"
#include "luid.h"
#include "mutex.h"
#include "net/eui64.h"
#include "net/netdev.h"
#include "xtimer.h"

#include "gnrc_netif_nrf24l01p.h"
#include "nrf24l01p_constants.h"
#include "nrf24l01p_channels.h"
#include "nrf24l01p_registers.h"
#include "nrf24l01p_communication.h"
#include "nrf24l01p_states.h"
#include "nrf24l01p_diagnostics.h"
#include "nrf24l01p_custom_header.h"
#include "nrf24l01p_isr.h"
#include "nrf24l01p_internal.h"
#include "nrf24l01p_conversion.h"
#include "nrf24l01p_util.h"
#include "nrf24l01p_netdev.h"

#define NRF24L01P_FLG_IRQ \
    (NRF24L01P_FLG_MAX_RT | NRF24L01P_FLG_TX_DS | NRF24L01P_FLG_RX_DR)

static int nrf24l01p_init(netdev_t *netdev);
static int nrf24l01p_recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int nrf24l01p_send(netdev_t *netdev, const iolist_t *iolist);
static void nrf24l01p_isr(netdev_t *netdev);
static int nrf24l01p_get(netdev_t *netdev, netopt_t opt, void *val,
                         size_t max_len);
static int nrf24l01p_set(netdev_t *netdev, netopt_t opt, const void *val,
                         size_t max_len);

const netdev_driver_t nrf24l01p_driver = {
    .init = nrf24l01p_init,
    .recv = nrf24l01p_recv,
    .send = nrf24l01p_send,
    .isr = nrf24l01p_isr,
    .get = nrf24l01p_get,
    .set = nrf24l01p_set
};

static inline void nrf24l01p_trigger_send(const nrf24l01p_t *dev)
{
    gpio_set(dev->params.pin_ce);
    xtimer_usleep(NRF24L01P_DELAY_US_CE_HIGH_PULSE);
    gpio_clear(dev->params.pin_ce);
    xtimer_usleep(NRF24L01P_DELAY_US_TX_SETTLING);
}

static void nrf24l01p_irq_handler(void *_dev)
{
    nrf24l01p_t *dev = (nrf24l01p_t *)_dev;

    /* Once the IRQ pin has triggered,
       do not congest the thread´s
       message queue with IRQ events */

    gpio_irq_disable(dev->params.pin_irq);
    DEBUG("[nrf24l01p] IRQ\n");
    netdev_trigger_event_isr((netdev_t *)dev);
}

static int nrf24l01p_init(netdev_t *netdev)
{
    nrf24l01p_t *dev = (nrf24l01p_t *)netdev;

    assert(dev->params.config.cfg_data_rate < NRF24L01P_RF_DR_NUM_OF);

    if (spi_init_cs(dev->params.spi, dev->params.pin_cs) != SPI_OK) {
        DEBUG("[nrf24l01p] nrf24l01p_init(): spi_init_cs() failed\n");
        return -EIO;
    }
    if (gpio_init(dev->params.pin_ce, GPIO_OUT) < 0) {
        DEBUG("[nrf24l01p] nrf24l01p_init(): gpio_init() failed\n");
        return -EIO;
    }
    gpio_clear(dev->params.pin_ce);
    if (nrf24l01p_acquire(dev) < 0) {
        DEBUG("[nrf24l01p] nrf24l01p_init(): nrf24l01p_acquire() failed\n");
        return -EIO;
    }
    nrf24l01p_transition_to_power_down(dev);
    nrf24l01p_flush_tx(dev);
    nrf24l01p_flush_rx(dev);
    uint8_t l2addr_size =
        nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);
    uint8_t pipes = 0;
    uint8_t ls_addr_byte = 1;
    uint8_t l2addr_addr_p0[] = NRF24L01P_L2ADDR_AUTO;
    if (memcmp(dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr,
               l2addr_addr_p0, l2addr_size) == 0) {
        luid_base(dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr, l2addr_size);
        dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr[l2addr_size - 1]
            = ls_addr_byte++;
    }
    memcpy(l2addr_addr_p0, dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr,
           l2addr_size);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_ADDR_P0, l2addr_addr_p0,
                        l2addr_size);
    pipes |= (1 << NRF24L01P_P0);
    uint8_t l2addr_addr_p1[] = NRF24L01P_L2ADDR_AUTO;
    if ((memcmp(dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr,
                l2addr_addr_p1, l2addr_size) == 0) ||
        (l2addr_addr_p0[l2addr_size - 1] == l2addr_addr_p1[l2addr_size - 1])) {
        luid_base(dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr, l2addr_size);
        dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr[l2addr_size - 1]
            = ls_addr_byte++;
    }
    memcpy(l2addr_addr_p1, dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr,
           l2addr_size);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_ADDR_P1, l2addr_addr_p1,
                        l2addr_size);
    pipes |= (1 << NRF24L01P_P1);
    if (dev->params.urxaddr.rxaddrpx.rx_pipe_2_addr
        != NRF24L01P_L2ADDR_UNDEF) {
        dev->params.urxaddr.rxaddrpx.rx_pipe_2_addr = ls_addr_byte++;
        nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_ADDR_P2,
                            &dev->params.urxaddr.rxaddrpx.rx_pipe_2_addr, 1);
        pipes |= (1 << NRF24L01P_P2);
    }
    if (dev->params.urxaddr.rxaddrpx.rx_pipe_3_addr
        != NRF24L01P_L2ADDR_UNDEF) {
        dev->params.urxaddr.rxaddrpx.rx_pipe_3_addr = ls_addr_byte++;
        nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_ADDR_P3,
                            &dev->params.urxaddr.rxaddrpx.rx_pipe_3_addr, 1);
        pipes |= (1 << NRF24L01P_P3);
    }
    if (dev->params.urxaddr.rxaddrpx.rx_pipe_4_addr
        != NRF24L01P_L2ADDR_UNDEF) {
        dev->params.urxaddr.rxaddrpx.rx_pipe_4_addr = ls_addr_byte++;
        nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_ADDR_P4,
                            &dev->params.urxaddr.rxaddrpx.rx_pipe_4_addr, 1);
        pipes |= (1 << NRF24L01P_P4);
    }
    if (dev->params.urxaddr.rxaddrpx.rx_pipe_5_addr
        != NRF24L01P_L2ADDR_UNDEF) {
        dev->params.urxaddr.rxaddrpx.rx_pipe_5_addr = ls_addr_byte++;
        nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_ADDR_P5,
                            &dev->params.urxaddr.rxaddrpx.rx_pipe_5_addr, 1);
        pipes |= (1 << NRF24L01P_P5);
    }
    nrf24l01p_reg8_setup_aw_t setup_aw =
        NRF24L01P_FLG_AW(dev->params.config.cfg_addr_width);
    nrf24l01p_reg8_rf_setup_t rf_setup =
        (((dev->params.config.cfg_data_rate & 2)
          ? NRF24L01P_FLG_RF_DR_HIGH : 0) |
         ((dev->params.config.cfg_data_rate & 1)
          ? NRF24L01P_FLG_RF_DR_LOW : 0)) |
        NRF24L01P_FLG_RF_PWR(dev->params.config.cfg_tx_power);
    nrf24l01p_reg8_setup_retr_t setup_retr =
        NRF24L01P_FLG_ARD(dev->params.config.cfg_retr_delay) |
        NRF24L01P_FLG_ARC(dev->params.config.cfg_max_retr);
    nrf24l01p_reg8_rf_ch_t rf_ch =
        NRF24L01P_FLG_RF_CH(vchanmap[dev->params.config.cfg_channel]);
    nrf24l01p_reg8_config_t config =
        (((dev->params.config.cfg_crc & 1)
          ? NRF24L01P_FLG_CRCO_2_BYTE : NRF24L01P_FLG_CRCO_1_BYTE) |
         ((dev->params.config.cfg_crc & 2)
          ? NRF24L01P_FLG_EN_CRC : 0));
    nrf24l01p_reg8_features_t features = NRF24L01P_FLG_EN_DYN_ACK;
    nrf24l01p_reg8_en_aa_t en_aa = pipes;
    nrf24l01p_reg8_dynpd_t endp = pipes;

    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        en_aa = endp = 0;
        uint8_t plw;
        if (pipes & (1 << NRF24L01P_P0)) {
            plw = NRF24L01P_MAX_PAYLOAD_WIDTH -
                  dev->params.config.cfg_plw_padd_p0;
            nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_PW_P0, &plw, 1);
        }
        if (pipes & (1 << NRF24L01P_P1)) {
            plw = NRF24L01P_MAX_PAYLOAD_WIDTH -
                  dev->params.config.cfg_plw_padd_p1;
            nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_PW_P1, &plw, 1);
        }
        if (pipes & (1 << NRF24L01P_P2)) {
            plw = NRF24L01P_MAX_PAYLOAD_WIDTH -
                  dev->params.config.cfg_plw_padd_p2;
            nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_PW_P2, &plw, 1);
        }
        if (pipes & (1 << NRF24L01P_P3)) {
            plw = NRF24L01P_MAX_PAYLOAD_WIDTH -
                  dev->params.config.cfg_plw_padd_p3;
            nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_PW_P3, &plw, 1);
        }
        if (pipes & (1 << NRF24L01P_P4)) {
            plw = NRF24L01P_MAX_PAYLOAD_WIDTH -
                  dev->params.config.cfg_plw_padd_p4;
            nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_PW_P4, &plw, 1);
        }
        if (pipes & (1 << NRF24L01P_P5)) {
            plw = NRF24L01P_MAX_PAYLOAD_WIDTH -
                  dev->params.config.cfg_plw_padd_p5;
            nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_PW_P5, &plw, 1);
        }
    }
    else {
        features |= (NRF24L01P_FLG_EN_DPL | NRF24L01P_FLG_EN_ACK_PAY);
    }
    uint8_t tx_addr[] = NRF24L01P_DEFAULT_TX_ADDR;
    nrf24l01p_reg8_status_t status = NRF24L01P_FLG_IRQ;
    nrf24l01p_write_reg(dev, NRF24L01P_REG_SETUP_AW, &setup_aw, 1);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_RF_SETUP, &rf_setup, 1);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_SETUP_RETR, &setup_retr, 1);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_RF_CH, &rf_ch, 1);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_EN_RXADDR, &pipes, 1);
    /* enable all pipes with addresses configured */
    nrf24l01p_write_reg(dev, NRF24L01P_REG_CONFIG, &config, 1);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_FEATURES, &features, 1);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_EN_AA, &en_aa, 1);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_DYNPD, &endp, 1);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_TX_ADDR, tx_addr, sizeof(tx_addr));
    /* clear interrupts */
    nrf24l01p_write_reg(dev, NRF24L01P_REG_STATUS, &status, 1);
    nrf24l01p_transition_to_standby_1(dev);
#if ENABLE_DEBUG
    nrf24l01p_diagnostics_print_all_regs(dev);
    nrf24l01p_diagnostics_print_dev_info(dev);
#endif
    if (!nrf24l01p_assert_awake(dev)) {
        nrf24l01p_release(dev);
        return -ENODEV;
    }
    nrf24l01p_transition_to_rx_mode(dev);
    nrf24l01p_release(dev);
    if (gpio_init_int(dev->params.pin_irq, GPIO_IN, GPIO_FALLING,
                      nrf24l01p_irq_handler, dev) < 0) {
        DEBUG("[nrf24l01p] nrf24l01p_init(): gpio_init_int() failed\n");
        return -EIO;
    }
    return 0;
}

/**
 * @brief   NRF24L01P @ref netdev_driver_t::recv routine
 *
 * @pre @see nrf24l01p_acquire must have been called before.
 * @pre Interrupts should be disabled
 *
 * The SPI bus acquirement in this function because it is called from
 * @ref netdev_driver_t::isr, possibly for multiple times. If another
 * device acquired the SPI bus within the ISR, the ISR would block
 * until that device releases the bus.
 *
 * @param[in] netdev        Abstract network device handle
 * @param[out] buf          Rx buffer
 * @param[in] len           Size of Rx buffer
 * @param[out]              LQI and RSSI information (unused)
 *
 * @return                  Size of received frame in @p buf
 * @return                  Upper estimation of the frame width,
 *                          if @p buf == NULL and len == 0
 * @return                  Actual frame width,
 *                          if @p buf == NULL and @p len != 0
 *                          (frame is dropped)
 * @retval -ENOBUFS         @p buf != NULL and @p len < actual frame width
 *                          (frame is dropped)
 * @retval -EINVAL          @p buf == NULL
 *                          (and none of the above cases are true)
 * @retval -ENOTSUP         Malformed header
 * @retval 0                No data to read from Rx FIFO
 */
static int nrf24l01p_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)info; /* nrf24l01p supports neither lqi nor rssi */
/*
    Returned format of constructed frame in buf:
 +--------------------------+
 | address lengths (1 Byte)  ....
 +--------------------------+
 +----------------------------------------+
    ....  destination address (3 Bytes - 5 Bytes)  ...
 +----------------------------------------+

 Only if NRF24L01P_CUSTOM_HEADER == 1
        (NRF24L01P_CUSTOM_HEADER limits payload length to
        (32 - (1 + 5)) to (32 - (1 + 3) bytes)
 +----------------------------------------+
    ....  source address (3 Bytes - 5 Bytes)      ....
 +----------------------------------------+

 +---------------------------------------------------------------+
    ....  payload (1 Byte - 32 Bytes)                            |
 +---------------------------------------------------------------+
 */
    /* return upper estaimation bound of frame size */
    if (!buf && !len) {
        DEBUG("[nrf24l01p] Return upper frame estimation");
        return 1 + NRF24L01P_MAX_ADDR_WIDTH + NRF24L01P_MAX_PAYLOAD_WIDTH;
    }
    nrf24l01p_t *dev = (nrf24l01p_t *)netdev;
    assert(dev->have_spi_access);
    uint8_t pl_width;
    nrf24l01p_reg8_status_t status
        = nrf24l01p_read_rx_pl_width(dev, &pl_width);
    uint8_t pno = NRF24L01P_VAL_RX_P_NO(status);
    if (!pl_width ||
        pl_width > NRF24L01P_MAX_PAYLOAD_WIDTH ||
        pno >= NRF24L01P_PX_NUM_OF) {
        DEBUG("[nrf24l01p] RX error, flush RX FIFO\n");
        /* In some rare cases the RX payload width (R_RX_PL_WID) exceeds
           the maximum of 32 bytes. In that case it must be flushed.
           See https://devzone.nordicsemi.com/f/nordic-q-a/26489/nrf24l01-the-length-of-received-data-exceed-32
           and https://www.mikrocontroller.net/articles/NRF24L01_Tutorial */
        nrf24l01p_flush_rx(dev);
        return 0;
    }
    uint8_t aw = nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);
    uint8_t dst_pipe_addr[aw];
    if (pno == NRF24L01P_P0) {
        memcpy(dst_pipe_addr, dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr, aw);
    }
    else {
        memcpy(dst_pipe_addr, dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr, aw);
        if (pno > NRF24L01P_P1) {
            dst_pipe_addr[aw - 1]
                = dev->params.urxaddr.arxaddr.rx_addr_short[pno - 2];
        }
    }
    uint8_t frame_len = 1U + sizeof(dst_pipe_addr) + pl_width;

    /* drop frame and return exact frame size */
    if (!buf) {
        if (len) {
            uint8_t garbage[pl_width];
            nrf24l01p_read_rx_payload(dev, garbage, pl_width);
            DEBUG("[nrf24l01p] Return exact frame length\n");
            return frame_len;
        }
        DEBUG("[nrf24l01p] No buffer provided\n");
        return -EINVAL;
    }

    /* drop frame, content in buf becomes invalid and return -ENOBUFS */
    if (len < frame_len) {
        DEBUG("[nrf24l01p] Buffer too small: %u < %u, dropping frame\n", len,
              frame_len);
        uint8_t garbage[pl_width];
        nrf24l01p_read_rx_payload(dev, garbage, pl_width);
        return -ENOBUFS;
    }

    /* get received frame */
    DEBUG("[nrf24l01p] Handle received frame\n");
    uint8_t *frame = (uint8_t *)buf;
    sb_hdr_init((shockburst_hdr_t *)frame);
    sb_hdr_set_dst_addr_width((shockburst_hdr_t *)frame,
                              sizeof(dst_pipe_addr));
#if NRF24L01P_CUSTOM_HEADER
    uint8_t payload[NRF24L01P_MAX_PAYLOAD_WIDTH];
    nrf24l01p_read_rx_payload(dev, payload, pl_width);
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        /* remove padding */
        for (uint8_t hdr_index = 0; hdr_index < sizeof(payload); hdr_index++) {
            if (payload[hdr_index] & (NRF24L01P_CSTM_HDR_PREEMBLE)) {
                memcpy(payload, payload + hdr_index,
                       sizeof(payload) - hdr_index);
                pl_width -= hdr_index;
                frame_len -= hdr_index;
                break;
            }
        }
    }
    uint8_t src_aw = sb_hdr_get_src_addr_width((shockburst_hdr_t *)payload);
    if (src_aw < NRF24L01P_MIN_ADDR_WIDTH ||
        src_aw > NRF24L01P_MAX_ADDR_WIDTH) {
        DEBUG("[nrf24l01p] Invalid header\n");
        return -ENOTSUP;
    }
    sb_hdr_set_src_addr_width((shockburst_hdr_t *)frame, payload[0]);
    frame += 1;
    pl_width -= 1; /* first byte was source address width */
    frame_len -= 1;
    memcpy(frame, dst_pipe_addr, sizeof(dst_pipe_addr));
    frame += sizeof(dst_pipe_addr);
    /* skip source address length field in payload */
    memcpy(frame, payload + 1, pl_width);
#else
    frame += 1;
    memcpy(frame, dst_pipe_addr, sizeof(dst_pipe_addr));
    frame += sizeof(dst_pipe_addr);
    nrf24l01p_read_rx_payload(dev, frame, pl_width);
#endif

#if ENABLE_DEBUG
    nrf24l01p_diagnostics_print_all_regs(dev);
    nrf24l01p_diagnostics_print_dev_info(dev);
    nrf24l01p_diagnostics_print_frame(dev, (uint8_t *)buf, frame_len);
#endif
    DEBUG("[nrf24l01p] Received frame length: %u\n", frame_len);
    return (int)frame_len;
}

/**
 * @brief   NRF24L01P @ref netdev_driver_t::send routine
 *
 * @param[in] netdev        Abstract network device handle
 * @param[in] iolist        Linked list of data to be sent, where
 *                          the base must be the destination address
 *
 * @return                  Size of sent payload
 * @retval -ENOTSUP         @p iolist had no base and no next link,
 *                          or address was too big, or too short
 * @retval -EAGAIN          Pending date has been sent first
 * @retval -E2BIG           Resulting frame from iolist was too big to be sent
 */
static int nrf24l01p_send(netdev_t *netdev, const iolist_t *iolist)
{
    assert(netdev && iolist);
    if (!(iolist->iol_base) || !(iolist->iol_next)) {
        DEBUG("[nrf24l01p] No Tx address or no payload\n");
        return -ENOTSUP;
    }
    nrf24l01p_t *dev = (nrf24l01p_t *)netdev;
    uint8_t pl_width = 0;
    uint8_t bcast_addr[] = NRF24L01P_BROADCAST_ADDR;
    uint8_t payload[(dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB)
                    ? (NRF24L01P_MAX_PAYLOAD_WIDTH -
                       dev->params.config.cfg_plw_padd_p0)
                    : NRF24L01P_MAX_PAYLOAD_WIDTH];
    memset(payload, 0, sizeof(payload));
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_fifo_status_t fifo_status;
    nrf24l01p_reg8_status_t status =
        nrf24l01p_read_reg(dev, NRF24L01P_REG_FIFO_STATUS, &fifo_status, 1);
    if (status & NRF24L01P_FLG_IRQ) {
        DEBUG("[nrf24l01p] Handle pending IRQ, before sending new data\n");
        nrf24l01p_release(dev);
        nrf24l01p_isr(&dev->netdev);
        return -EAGAIN;
    }
    if (fifo_status & NRF24L01P_FLG_TX_FULL_) {
        nrf24l01p_flush_tx(dev);
    }
    shockburst_hdr_t hdr = *(((shockburst_hdr_t *)iolist->iol_base));
    uint8_t dst_addr_len = sb_hdr_get_dst_addr_width(&hdr);
    if (dst_addr_len > NRF24L01P_MAX_ADDR_WIDTH ||
        dst_addr_len < NRF24L01P_MIN_ADDR_WIDTH) {
        nrf24l01p_release(dev);
        DEBUG("[nrf24l01p] Destination address has an invalid length: %u\n",
              dst_addr_len);
        return -ENOTSUP;
    }
    uint8_t* tx_addr = hdr.dst_addr;
    uint8_t tx_addr_len = dst_addr_len;
#if NRF24L01P_CUSTOM_HEADER
    uint8_t src_addr_len = sb_hdr_get_src_addr_width(&hdr);
    if (src_addr_len > NRF24L01P_MAX_ADDR_WIDTH ||
        src_addr_len < NRF24L01P_MIN_ADDR_WIDTH) {
        nrf24l01p_release(dev);
        DEBUG("[nrf24l01p] Source address has an invalid length: %u\n",
              src_addr_len);
        return -ENOTSUP;
    }
    sb_hdr_init((shockburst_hdr_t *)payload);
    sb_hdr_set_src_addr_width((shockburst_hdr_t *)payload, src_addr_len);
    pl_width++;
    memcpy(payload + pl_width, hdr.src_addr, src_addr_len);
    pl_width += src_addr_len;
#endif
    for (const iolist_t *iol = iolist->iol_next; iol; iol = iol->iol_next) {
        if (pl_width + iol->iol_len > sizeof(payload)) {
            nrf24l01p_release(dev);
            DEBUG("[nrf24l01p] packet too big\n");
            return -E2BIG;
        }
        memcpy(payload + pl_width, iol->iol_base, iol->iol_len);
        pl_width += iol->iol_len;
    }
#if NRF24L01P_CUSTOM_HEADER
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB
        && sizeof(payload) != pl_width) {
        /* frame: [ ... padding ... |  header | data ] */
        for (uint8_t i = 1; i <= pl_width; i++) {
            payload[sizeof(payload) - i] = payload[pl_width - i];
        }
        memset(payload, NRF24L01P_CSTM_HDR_PADDING,
               (sizeof(payload) - pl_width));
        pl_width = sizeof(payload);
    }
#endif
    nrf24l01p_write_reg(dev, NRF24L01P_REG_TX_ADDR, tx_addr, tx_addr_len);
    if (memcmp(tx_addr, bcast_addr, tx_addr_len) == 0) {
        /* do not expect ACK for broadcast */
        nrf24l01p_write_tx_pl_no_ack(dev, payload, pl_width);
    }
    else {
        nrf24l01p_write_tx_payload(dev, payload, pl_width);
        /* A PTX node must change pipe 0 Rx address to Tx address
         * in order to receive ACKs.
         * If node switches back to Rx mode, pipe 0 Rx address
         * must be restored from params. */
        if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_ESB) {
            nrf24l01p_reg8_setup_aw_t setup_aw =
                NRF24L01P_FLG_AW(nrf24l01p_valtoe_aw(tx_addr_len));
            nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_ADDR_P0,
                                tx_addr, tx_addr_len);
            nrf24l01p_write_reg(dev, NRF24L01P_REG_SETUP_AW, &setup_aw, 1);
        }
    }
    if (dev->state != NRF24L01P_STATE_TX_MODE &&
        dev->state != NRF24L01P_STATE_STANDBY_2) {
        dev->idle_state = dev->state;
        if (dev->state != NRF24L01P_STATE_STANDBY_1) {
            nrf24l01p_transition_to_standby_1(dev);
        }
        nrf24l01p_transition_to_tx_mode(dev);
    }
#if ENABLE_DEBUG
    nrf24l01p_diagnostics_print_all_regs(dev);
    nrf24l01p_diagnostics_print_dev_info(dev);
#endif
    nrf24l01p_release(dev);
    nrf24l01p_trigger_send(dev);
    DEBUG("[nrf24l01p] Sending %u bytes\n", pl_width);
    return (int)pl_width;
}

/**
 * @brief   NRF24L01P @ref netdev_driver_t::isr
 *
 * @param[in] netdev        Abstract network device
 */
static void nrf24l01p_isr(netdev_t *netdev)
{
    nrf24l01p_t *dev = (nrf24l01p_t *)netdev;

    gpio_irq_enable(dev->params.pin_irq);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_status_t status = nrf24l01p_get_status(dev);
    netdev_event_t event;
    if (status & NRF24L01P_FLG_MAX_RT) {
        if ((event = nrf24l01p_isr_max_rt(dev)) != NETDEV_EVENT_NONE) {
            netdev->event_callback(netdev, event);
        }
    }
    if (status & NRF24L01P_FLG_TX_DS) {
        if ((event = nrf24l01p_isr_tx_ds(dev)) != NETDEV_EVENT_NONE) {
            netdev->event_callback(netdev, event);
        }
    }
    if (status & NRF24L01P_FLG_RX_DR) {
        if ((event = nrf24l01p_isr_rx_dr(dev)) != NETDEV_EVENT_NONE) {
            netdev->event_callback(netdev, event);
        }
    }
    /* clear interrupt flags */
    nrf24l01p_write_reg(dev, NRF24L01P_REG_STATUS, &status, 1);
    nrf24l01p_reg8_fifo_status_t fifo_status;
    status =
        nrf24l01p_read_reg(dev, NRF24L01P_REG_FIFO_STATUS, &fifo_status, 1);
    /* read all RX data */
    if (dev->state == NRF24L01P_STATE_RX_MODE) {
        while (!(fifo_status & NRF24L01P_FLG_RX_EMPTY)) {
            DEBUG("[nrf24l01p] ISR: read pending Rx frames\n");
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
            status = NRF24L01P_FLG_RX_DR;
            nrf24l01p_write_reg(dev, NRF24L01P_REG_STATUS, &status, 1);
            status = nrf24l01p_read_reg(dev, NRF24L01P_REG_FIFO_STATUS,
                                        &fifo_status, 1);
        }
        nrf24l01p_release(dev);
    }
    else {
        if (!(fifo_status & NRF24L01P_FLG_TX_EMPTY)) {
            /* frame in FIFO is not an ACK */
            if (dev->state == NRF24L01P_STATE_TX_MODE) {
                nrf24l01p_release(dev);
                nrf24l01p_trigger_send(dev);
            }
            else {
                nrf24l01p_release(dev);
            }
        }
        else {
            if (dev->state != NRF24L01P_STATE_STANDBY_1) {
                nrf24l01p_transition_to_standby_1(dev);
            }
            /* go to idle state */
            if (dev->idle_state != NRF24L01P_STATE_STANDBY_1) {
                if (dev->idle_state == NRF24L01P_STATE_POWER_DOWN) {
                    nrf24l01p_transition_to_power_down(dev);
                }
                else {
                    dev->idle_state = NRF24L01P_STATE_RX_MODE;
                    nrf24l01p_transition_to_rx_mode(dev);
                }
            }
            nrf24l01p_release(dev);
        }
    }
}

/**
 * @brief   Construct an EUI64 from an NRF24L01P layer-2 address
 *
 *          l2a: [a1, ..., an] | 3 <= n <= 5
 *          EUI64:
 *          if n = 3:
 *          [3 << 5, 0, 0, ff, fe, a1, a2, a3]
 *          if n = 4:
 *          [4 << 5, 0, a1, ff, fe, a2, a3, a4]
 *          if n = 5:
 *          [5 << 5, a1, a2, ff, fe, a3, a4, a5]
 */
static int nrf24l01p_get_iid(const nrf24l01p_t *dev, eui64_t *iid)
{
    *iid = (eui64_t){
        .uint8 = { 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x00 }
    };
    uint8_t aw = nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);
    iid->uint8[0] = aw << 5;
    if (aw > 3) {
        memcpy(&iid->uint8[1 + (5 - aw)],
               dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr, aw - 3);
    }
    memcpy(&iid->uint8[5],
           dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr + (aw - 3), 3);
    return sizeof(eui64_t);
}

/**
 * @brief   @ref netdev_driver_t::get
 *
 * @param[in] netdev        Abstract network device
 * @param[in] opt           netdev option type
 * @param[out] val          Option value
 * @param[in] max_len       Maximum option length
 *
 * @return                  Size of written option value
 * @retval -ENOTSUP         Unsupported netdev option @p opt
 */
static int nrf24l01p_get(netdev_t *netdev, netopt_t opt, void *val,
                         size_t max_len)
{
    nrf24l01p_t *dev = (nrf24l01p_t *)netdev;

    switch (opt) {
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN: {
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = nrf24l01p_etoval_aw(
                dev->params.config.cfg_addr_width);
            return sizeof(uint16_t);
        } break;
        case NETOPT_ADDRESS: {
            assert(max_len >= NRF24L01P_MAX_ADDR_WIDTH);
            uint8_t aw =
                nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);
            memcpy(val, dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr, aw);
            return aw;
        } break;
        case NETOPT_AUTOACK: {
            assert(max_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)val) =
                (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB)
                ? NETOPT_ENABLE
                : NETOPT_DISABLE;
            return sizeof(netopt_enable_t);
        } break;
        case NETOPT_CHANNEL: {
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = (uint16_t)nrf24l01p_get_channel(dev);
            return sizeof(uint16_t);
        } break;
        case NETOPT_CHECKSUM:
        case NETOPT_INTEGRITY_CHECK: {
            assert(max_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)val) = nrf24l01p_get_crc(dev, NULL)
                                        ? NETOPT_ENABLE
                                        : NETOPT_DISABLE;
            return sizeof(netopt_enable_t);
        } break;
        case NETOPT_DEVICE_TYPE: {
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = NETDEV_TYPE_NRF24L01P;
            return sizeof(uint16_t);
        } break;
        case NETOPT_IPV6_IID: {
            if (max_len < sizeof(eui64_t)) {
                return -EOVERFLOW;
            }
            return nrf24l01p_get_iid(dev, val);
        } break;
        /* upper layer PDU? (needed by 6LoWPAN)*/
        case NETOPT_MAX_PDU_SIZE: {
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = (uint16_t)nrf24l01p_get_mtu(dev,
                                                             NRF24L01P_P0);
            return sizeof(uint16_t);
        } break;
        case NETOPT_PROTO: {
            assert(max_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)val) = NRF24L01P_UPPER_LAYER_PROTOCOL;
            return sizeof(gnrc_nettype_t);
        } break;
        case NETOPT_RETRANS: {
            assert(max_len == sizeof(uint8_t));
            *((uint8_t *)val) = nrf24l01p_get_max_retransm(dev);
            return sizeof(uint8_t);
        }
        case NETOPT_RX_TIMEOUT: {
            assert(max_len == sizeof(uint32_t));
            *((uint32_t *)val) = (uint32_t)nrf24l01p_get_retransm_delay(dev,
                                                                        NULL);
            return sizeof(uint32_t);
        } break;
        case NETOPT_STATE: {
            assert(max_len == sizeof(netopt_state_t));
            *((netopt_state_t *)val) = nrf24l01p_state_to_netif(dev->state);
            return sizeof(netopt_state_t);
        } break;
        case NETOPT_TX_POWER: {
            assert(max_len == sizeof(int16_t));
            *((int16_t *)val) = (int16_t)nrf24l01p_get_tx_power(dev, NULL);
            return sizeof(uint16_t);
        } break;
        default:
            DEBUG("[nrf24l01p] Unsupported netdev option %d\n", opt);
            return -ENOTSUP;
    }
}

/**
 * @brief   @ref netdev_driver_t::set
 *
 * @param[in] netdev            Abstract network device handle
 * @param[in] opt               netdev option type
 * @param[in] val               Option value
 * @param[in] len               Size of option value
 *
 * @return                      Size of written option value
 * @return                      negative number, on failure
 * @retval -ENOTSUP             Unsupported netdev option @p opt
 */
static int nrf24l01p_set(netdev_t *netdev, netopt_t opt, const void *val,
                         size_t len)
{
    nrf24l01p_t *dev = (nrf24l01p_t *)netdev;

    switch (opt) {
        case NETOPT_ADDRESS: {
            /* common address length for all pipes */
            assert(len ==
                   nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width));
            int ret = nrf24l01p_set_rx_address(dev, val, len, NRF24L01P_P0);
            return ret ? ret : (int)len;
        } break;
        case NETOPT_CHANNEL: {
            assert(len == sizeof(uint16_t));
            uint16_t ch = *((uint16_t *)val);
            int ret = nrf24l01p_set_channel(dev, (uint8_t)ch);
            return ret ? ret : (int)sizeof(uint16_t);
        } break;
        case NETOPT_CHECKSUM:
        case NETOPT_INTEGRITY_CHECK: {
            assert(len == sizeof(netopt_enable_t));
            nrf24l01p_crc_t crc = (*((netopt_enable_t *)val) == NETOPT_ENABLE)
                                   ? NRF24L01P_CRC_2BYTE : NRF24L01P_CRC_0BYTE;
            int ret = nrf24l01p_set_crc(dev, crc);
            return ret ? ret : (int)sizeof(netopt_enable_t);
        } break;
        case NETOPT_RETRANS: {
            assert(len == sizeof(uint8_t));
            uint8_t n = *((uint8_t *)val);
            int ret = nrf24l01p_set_max_retransm(dev, n);
            return ret ? ret : (int)sizeof(uint8_t);
        } break;
        case NETOPT_RX_TIMEOUT: {
            assert(len == sizeof(uint32_t));
            uint32_t us = *((uint32_t *)val);
            int ret =
                nrf24l01p_set_retransm_delay(dev, nrf24l01p_valtoe_ard(us));
            return ret ? ret : (int)sizeof(uint32_t);
        } break;
        case NETOPT_STATE: {
            assert(len == sizeof(netopt_state_t));
            nrf24l01p_state_t s =
                nrf24l01p_state_from_netif(*((netopt_state_t *)val));
            int ret = nrf24l01p_set_state(dev, s);
            return ret < 0 ? ret : (int)sizeof(netopt_state_t);
        } break;
        case NETOPT_TX_POWER: {
            assert(len == sizeof(int16_t));
            int16_t dbm = *((int16_t *)val);
            int ret = nrf24l01p_set_tx_power(dev,
                                             nrf24l01p_valtoe_tx_power(dbm));
            return ret ? ret : (int)sizeof(int16_t);
        } break;
        default:
            DEBUG("[nrf24l01p] Unsupported netdev option %d\n", opt);
            return -ENOTSUP;
    }
}
