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
 * @brief     Functions to debug the NRF24L01P transceiver
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <stdio.h>
#include "nrf24l01p_registers.h"
#include "nrf24l01p_communication.h"
#include "nrf24l01p_conversion.h"
#include "nrf24l01p_util.h"
#include "nrf24l01p_diagnostics.h"

void nrf24l01p_diagnostics_print_all_regs(const nrf24l01p_t *dev)
{
    nrf24l01p_reg8_config_t config;

    nrf24l01p_read_reg(dev, NRF24L01P_REG_CONFIG, &config, 1);
    puts("");
    printf(
        "CONFIG         [MASK_RX_DR %u MASK_TX_DS %u MASK_MAX_RT %u EN_CRC %u "
                        "CRCO %u PWR_UP %u PRIM_RX %u]\n",
        (unsigned)NRF24L01P_VAL_MASK_RX_DR(config),
        (unsigned)NRF24L01P_VAL_MASK_TX_DS(config),
        (unsigned)NRF24L01P_VAL_MAX_RT(config),
        (unsigned)NRF24L01P_VAL_EN_CRC(config),
        (unsigned)NRF24L01P_VAL_CRCO(config),
        (unsigned)NRF24L01P_VAL_PWR_UP(config),
        (unsigned)NRF24L01P_VAL_PRIM_RX(config)
        );
    nrf24l01p_reg8_en_aa_t en_aa;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_EN_AA, &en_aa, 1);
    printf(
        "EN_AA          [ENAA_P5 %u ENAA_P4 %u ENAA_P3 %u ENAA_P2 %u "
                        "ENAA_P1 %u ENAA_P0 %u]\n",
        (unsigned)NRF24L01P_VAL_ENAA_P5(en_aa),
        (unsigned)NRF24L01P_VAL_ENAA_P4(en_aa),
        (unsigned)NRF24L01P_VAL_ENAA_P3(en_aa),
        (unsigned)NRF24L01P_VAL_ENAA_P2(en_aa),
        (unsigned)NRF24L01P_VAL_ENAA_P1(en_aa),
        (unsigned)NRF24L01P_VAL_ENAA_P0(en_aa)
        );
    nrf24l01p_reg8_en_rxaddr_t en_rxaddr;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_EN_RXADDR, &en_rxaddr, 1);
    printf(
        "EN_RXADDR      [ERX_P5 %u ERX_P4 %u ERX_P3 %u ERX_P2 %u ERX_P1 %u "
                        "ERX_P0 %u]\n",
        (unsigned)NRF24L01P_VAL_ERX_P5(en_rxaddr),
        (unsigned)NRF24L01P_VAL_ERX_P4(en_rxaddr),
        (unsigned)NRF24L01P_VAL_ERX_P3(en_rxaddr),
        (unsigned)NRF24L01P_VAL_ERX_P2(en_rxaddr),
        (unsigned)NRF24L01P_VAL_ERX_P1(en_rxaddr),
        (unsigned)NRF24L01P_VAL_ERX_P0(en_rxaddr)
        );
    nrf24l01p_reg8_setup_aw_t setup_aw;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_SETUP_AW, &setup_aw, 1);
    printf(
        "SETUP_AW       [AW %u]\n",
        (unsigned)NRF24L01P_VAL_AW(setup_aw)
        );
    nrf24l01p_reg8_setup_retr_t setup_retr;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_SETUP_RETR, &setup_retr, 1);
    printf(
        "SETUP_RETR     [ARD %u ARC %u]\n",
        (unsigned)NRF24L01P_VAL_ARD(setup_retr),
        (unsigned)NRF24L01P_VAL_ARC(setup_retr)
        );
    nrf24l01p_reg8_rf_ch_t rf_ch;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RF_CH, &rf_ch, 1);
    printf(
        "RF_CH          [RF_CH %u]\n",
        (unsigned)NRF24L01P_VAL_RF_CH(rf_ch)
        );
    nrf24l01p_reg8_rf_setup_t rf_setup;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RF_SETUP, &rf_setup, 1);
    printf(
        "RF_SETUP       [CONT_WAVE %u RF_DR_LOW %u PLL_LOCK %u RF_DR_HIGH %u "
                        "RF_PWR %u]\n",
        (unsigned)NRF24L01P_VAL_CONT_WAVE(rf_setup),
        (unsigned)NRF24L01P_VAL_RF_DR_LOW(rf_setup),
        (unsigned)NRF24L01P_VAL_PLL_LOCK(rf_setup),
        (unsigned)NRF24L01P_VAL_RF_DR_HIGH(rf_setup),
        (unsigned)NRF24L01P_VAL_RF_PWR(rf_setup)
        );
    nrf24l01p_reg8_status_t status;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_STATUS, &status, 1);
    printf(
        "STATUS         [RX_DR %u TX_DS %u MAX_RT %u RX_P_NO %u TX_FULL %u]\n",
        (unsigned)NRF24L01P_VAL_RX_DR(status),
        (unsigned)NRF24L01P_VAL_TX_DS(status),
        (unsigned)NRF24L01P_VAL_MAX_RT(status),
        (unsigned)NRF24L01P_VAL_RX_P_NO(status),
        (unsigned)NRF24L01P_VAL_TX_FULL(status)
        );
    nrf24l01p_reg8_observe_tx_t observe_tx;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_OBSERVE_TX, &observe_tx, 1);
    printf(
        "OBSERVE_TX     [PLOS_CNT %u ARC_CNT %u]\n",
        (unsigned)NRF24L01P_VAL_PLOS_CNT(observe_tx),
        (unsigned)NRF24L01P_VAL_ARC_CNT(observe_tx)
        );
    nrf24l01p_reg8_rpd_t rpd;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RPD, &rpd, 1);
    printf(
        "RPD            [RPD %u]\n",
        (unsigned)NRF24L01P_VAL_RPD(rpd)
        );
    nrf24l01p_reg40_rx_addr_px_t rx_addr_px_40[2]; //Pipe 0/1
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_ADDR_P0, rx_addr_px_40[0],
                       NRF24L01P_MAX_ADDR_WIDTH);
    printf(
        "RX_ADDR_P0     [RX_ADDR_P0 %x:%x:%x:%x:%x]\n",
        rx_addr_px_40[0][0],
        rx_addr_px_40[0][1],
        rx_addr_px_40[0][2],
        rx_addr_px_40[0][3],
        rx_addr_px_40[0][4]
        );
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_ADDR_P1, rx_addr_px_40[1],
                       NRF24L01P_MAX_ADDR_WIDTH);
    printf(
        "RX_ADDR_P1     [RX_ADDR_P1 %x:%x:%x:%x:%x]\n",
        rx_addr_px_40[1][0],
        rx_addr_px_40[1][1],
        rx_addr_px_40[1][2],
        rx_addr_px_40[1][3],
        rx_addr_px_40[1][4]
        );
    nrf24l01p_reg8_rx_addr_px_t rx_addr_px_8[4];  //Pipe 2/3/4/5
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_ADDR_P2, &rx_addr_px_8[0], 1);
    printf(
        "RX_ADDR_P2     [RX_ADDR_P2 %x]\n",
        rx_addr_px_8[0]
        );
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_ADDR_P3, &rx_addr_px_8[1], 1);
    printf(
        "RX_ADDR_P3     [RX_ADDR_P3 %x]\n",
        rx_addr_px_8[1]
        );
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_ADDR_P4, &rx_addr_px_8[2], 1);
    printf(
        "RX_ADDR_P4     [RX_ADDR_P4 %x]\n",
        rx_addr_px_8[2]
        );
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_ADDR_P5, &rx_addr_px_8[3], 1);
    printf(
        "RX_ADDR_P5     [RX_ADDR_P5 %x]\n",
        rx_addr_px_8[3]
        );
    nrf24l01p_reg40_tx_addr_t tx_addr;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_TX_ADDR, tx_addr,
                       NRF24L01P_MAX_ADDR_WIDTH);
    printf(
        "TX_ADDR        [TX_ADDR %x:%x:%x:%x:%x]\n",
        tx_addr[0],
        tx_addr[1],
        tx_addr[2],
        tx_addr[3],
        tx_addr[4]
        );
    nrf24l01p_reg8_rx_pw_px_t rx_pw_px[6];
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_PW_P0, &rx_pw_px[0], 1);
    printf(
        "RX_PW_P0       [RX_PW_P0 %u]\n",
        (unsigned)NRF24L01P_VAL_RX_PW_PX(rx_pw_px[0])
        );
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_PW_P1, &rx_pw_px[1], 1);
    printf(
        "RX_PW_P1       [RX_PW_P1 %u]\n",
        (unsigned)NRF24L01P_VAL_RX_PW_PX(rx_pw_px[1])
        );
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_PW_P2, &rx_pw_px[2], 1);
    printf(
        "RX_PW_P2       [RX_PW_P2 %u]\n",
        (unsigned)NRF24L01P_VAL_RX_PW_PX(rx_pw_px[2])
        );
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_PW_P3, &rx_pw_px[3], 1);
    printf(
        "RX_PW_P3       [RX_PW_P3 %u]\n",
        (unsigned)NRF24L01P_VAL_RX_PW_PX(rx_pw_px[3])
        );
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_PW_P4, &rx_pw_px[4], 1);
    printf(
        "RX_PW_P4       [RX_PW_P4 %u]\n",
        (unsigned)NRF24L01P_VAL_RX_PW_PX(rx_pw_px[4])
        );
    nrf24l01p_read_reg(dev, NRF24L01P_REG_RX_PW_P5, &rx_pw_px[5], 1);
    printf(
        "RX_PW_P5       [RX_PW_P5 %u]\n",
        (unsigned)NRF24L01P_VAL_RX_PW_PX(rx_pw_px[5])
        );
    nrf24l01p_reg8_fifo_status_t fifo_status;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_FIFO_STATUS, &fifo_status, 1);
    printf(
        "FIFO_STATUS    [TX_REUSE %u TX_FULL %u TX_EMPTY %u RX_FULL %u "
                        "RX_EMPTY %u]\n",
        (unsigned)NRF24L01P_VAL_TX_REUSE(fifo_status),
        (unsigned)NRF24L01P_VAL_TX_FULL_(fifo_status),
        (unsigned)NRF24L01P_VAL_TX_EMPTY(fifo_status),
        (unsigned)NRF24L01P_VAL_RX_FULL(fifo_status),
        NRF24L01P_VAL_RX_EMPTY(fifo_status)
        );
    nrf24l01p_reg8_dynpd_t dynpd;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_DYNPD, &dynpd, 1);
    printf(
        "DYNDP          [DPL_P5 %u DPL_P4 %u DPL_P3 %u DPL_P2 %u DPL_P1 %u "
                        "DPL_P0 %u]\n",
        (unsigned)NRF24L01P_VAL_DPL_P5(dynpd),
        (unsigned)NRF24L01P_VAL_DPL_P4(dynpd),
        (unsigned)NRF24L01P_VAL_DPL_P3(dynpd),
        (unsigned)NRF24L01P_VAL_DPL_P2(dynpd),
        (unsigned)NRF24L01P_VAL_DPL_P1(dynpd),
        (unsigned)NRF24L01P_VAL_DPL_P0(dynpd)
        );
    nrf24l01p_reg8_features_t features;
    nrf24l01p_read_reg(dev, NRF24L01P_REG_FEATURES, &features, 1);
    printf(
        "FEATURES       [EN_DPL %u EN_ACK_PAY %u DYN_ACK %u]\n",
        (unsigned)NRF24L01P_VAL_EN_DPL(features),
        (unsigned)NRF24L01P_VAL_EN_ACK_PAY(features),
        (unsigned)NRF24L01P_VAL_EN_DYN_ACK(features)
        );
}

void nrf24l01p_diagnostics_print_dev_info(const nrf24l01p_t *dev)
{
    printf("device:                 %p\n", dev);
    printf("device params:\n");
    printf("\tProtocol:             %s\n",
           dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB
           ? "ShockBurst"
           : "Enhanced ShockBurst");
    printf("\tRx address length:    %u bytes\n",
           (unsigned)nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width));
    printf("\tChannel:              %u\n",
           dev->params.config.cfg_channel);
    printf("\tCRC length:           %u bytes\n",
           (unsigned)nrf24l01p_etoval_crc(dev->params.config.cfg_crc));
    printf("\tData rate:            %u kbps\n",
           (unsigned)nrf24l01p_etoval_rfdr(dev->params.config.cfg_data_rate));
    printf("\tMax. retransmissions: %u\n",
           dev->params.config.cfg_max_retr);
    printf("\tRetransmission delay: %u us\n",
           (unsigned)nrf24l01p_etoval_ard(dev->params.config.cfg_retr_delay));
    printf("\tTx power:             %d dbm\n",
           nrf24l01p_etoval_tx_power(dev->params.config.cfg_tx_power));
    printf("\tPayload padding p0:   %u bytes\n",
           (unsigned)dev->params.config.cfg_plw_padd_p0);
    printf("\tPayload padding p1:   %u bytes\n",
           (unsigned)dev->params.config.cfg_plw_padd_p1);
    printf("\tPayload padding p2:   %u bytes\n",
           (unsigned)dev->params.config.cfg_plw_padd_p2);
    printf("\tPayload padding p3:   %u bytes\n",
           (unsigned)dev->params.config.cfg_plw_padd_p3);
    printf("\tPayload padding p4:   %u bytes\n",
           (unsigned)dev->params.config.cfg_plw_padd_p4);
    printf("\tPayload padding p5:   %u bytes\n",
           (unsigned)dev->params.config.cfg_plw_padd_p5);
    printf("\tRx address p0:        %2x:%2x:%2x:%2x:%2x\n",
           dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr[0],
           dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr[1],
           dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr[2],
           dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr[3],
           dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr[4]
           );
    printf("\tRX address p1:        %2x:%2x:%2x:%2x:%2x\n",
           dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr[0],
           dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr[1],
           dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr[2],
           dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr[3],
           dev->params.urxaddr.rxaddrpx.rx_pipe_1_addr[4]
           );
    printf("\tRX address p2:        %2x\n",
           dev->params.urxaddr.rxaddrpx.rx_pipe_2_addr);
    printf("\tRX address p3:        %2x\n",
           dev->params.urxaddr.rxaddrpx.rx_pipe_3_addr);
    printf("\tRX address p4:        %2x\n",
           dev->params.urxaddr.rxaddrpx.rx_pipe_4_addr);
    printf("\tRX address p5:        %2x\n",
           dev->params.urxaddr.rxaddrpx.rx_pipe_5_addr);

    printf("State:                  %s\n",
           nrf24l01p_state_to_string(dev->state));
#ifndef NDEBUG
    printf("Transitions:            %u\n", (unsigned)dev->transitions);
#endif
}

void nrf24l01p_diagnostics_print_frame(const nrf24l01p_t *dev,
                                       const void *frame, size_t len)
{
    (void)dev;
    printf("Rx frame\n");
    for (uint8_t i = 0; i < len; i++) {
        printf("0x%02X ", ((uint8_t *)frame)[i]);
    }
    puts("\n");
}
