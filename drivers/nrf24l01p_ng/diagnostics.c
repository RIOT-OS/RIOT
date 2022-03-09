/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief     Functions to debug the NRF24L01+ (NG) transceiver
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <stdio.h>
#include <string.h>
#include "nrf24l01p_ng_registers.h"
#include "nrf24l01p_ng_communication.h"
#include "nrf24l01p_ng_diagnostics.h"

#if NRF24L01P_NG_ADDR_WIDTH == 3
#define ADDR_FMT        "%02x%02x%02x"
#define ADDR_P0(dev)                    \
    NRF24L01P_NG_ADDR_P0(dev)[0],       \
    NRF24L01P_NG_ADDR_P0(dev)[1],       \
    NRF24L01P_NG_ADDR_P0(dev)[2]
#define ADDR_P1(dev)                    \
    NRF24L01P_NG_ADDR_P1(dev)[0],       \
    NRF24L01P_NG_ADDR_P1(dev)[1],       \
    NRF24L01P_NG_ADDR_P1(dev)[2]
#elif NRF24L01P_NG_ADDR_WIDTH == 4
#define ADDR_FMT    "%02x%02x%02x%02x"
#define ADDR_P0(dev)                    \
    NRF24L01P_NG_ADDR_P0(dev)[0],       \
    NRF24L01P_NG_ADDR_P0(dev)[1],       \
    NRF24L01P_NG_ADDR_P0(dev)[2],       \
    NRF24L01P_NG_ADDR_P0(dev)[3]
#define ADDR_P1(dev)                    \
    NRF24L01P_NG_ADDR_P1(dev)[0],       \
    NRF24L01P_NG_ADDR_P1(dev)[1],       \
    NRF24L01P_NG_ADDR_P1(dev)[2],       \
    NRF24L01P_NG_ADDR_P1(dev)[3]
#else
#define ADDR_FMT    "%02x%02x%02x%02x%02x"
#define ADDR_P0(dev)                    \
    NRF24L01P_NG_ADDR_P0(dev)[0],       \
    NRF24L01P_NG_ADDR_P0(dev)[1],       \
    NRF24L01P_NG_ADDR_P0(dev)[2],       \
    NRF24L01P_NG_ADDR_P0(dev)[3],       \
    NRF24L01P_NG_ADDR_P0(dev)[4]
#define ADDR_P1(dev)                    \
    NRF24L01P_NG_ADDR_P1(dev)[0],       \
    NRF24L01P_NG_ADDR_P1(dev)[1],       \
    NRF24L01P_NG_ADDR_P1(dev)[2],       \
    NRF24L01P_NG_ADDR_P1(dev)[3],       \
    NRF24L01P_NG_ADDR_P1(dev)[4]
#endif

const char *
nrf24l01p_ng_diagnostics_state_to_string(nrf24l01p_ng_state_t state)
{
    if (state == NRF24L01P_NG_STATE_POWER_DOWN) {
        return "POWER_DOWN";
    }
    if (state == NRF24L01P_NG_STATE_STANDBY_1) {
        return "STANDBY_1";
    }
    if (state == NRF24L01P_NG_STATE_STANDBY_2) {
        return "STANDBY_2";
    }
    if (state == NRF24L01P_NG_STATE_TX_MODE) {
        return "TX_MODE";
    }
    if (state == NRF24L01P_NG_STATE_RX_MODE) {
        return "RX_MODE";
    }
    return "UNDEFINED";
}

nrf24l01p_ng_state_t
nrf24l01p_ng_diagnostics_string_to_state(const char *sstate)
{
    if (!strcmp(sstate, "POWER_DOWN")) {
        return NRF24L01P_NG_STATE_POWER_DOWN;
    }
    if (!strcmp(sstate, "STANDBY_1")) {
        return NRF24L01P_NG_STATE_STANDBY_1;
    }
    if (!strcmp(sstate, "STANDBY_2")) {
        return NRF24L01P_NG_STATE_STANDBY_2;
    }
    if (!strcmp(sstate, "TX_MODE")) {
        return NRF24L01P_NG_STATE_TX_MODE;
    }
    if (!strcmp(sstate, "RX_MODE")) {
        return NRF24L01P_NG_STATE_RX_MODE;
    }
    return NRF24L01P_NG_STATE_UNDEFINED;
}

void nrf24l01p_ng_diagnostics_print_all_regs(const nrf24l01p_ng_t *dev)
{
    uint8_t config;

    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_CONFIG, &config, 1);
    puts("");
    printf(
        "CONFIG         [MASK_RX_DR %u "
                        "MASK_TX_DS %u "
                        "MASK_MAX_RT %u "
                        "EN_CRC %u "
                        "CRCO %u "
                        "PWR_UP %u "
                        "PRIM_RX %u]\n",
        (unsigned)NRF24L01P_NG_VAL_MASK_RX_DR(config),
        (unsigned)NRF24L01P_NG_VAL_MASK_TX_DS(config),
        (unsigned)NRF24L01P_NG_VAL_MAX_RT(config),
        (unsigned)NRF24L01P_NG_VAL_EN_CRC(config),
        (unsigned)NRF24L01P_NG_VAL_CRCO(config),
        (unsigned)NRF24L01P_NG_VAL_PWR_UP(config),
        (unsigned)NRF24L01P_NG_VAL_PRIM_RX(config)
        );
    uint8_t en_aa;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_EN_AA, &en_aa, 1);
    printf(
        "EN_AA          [ENAA_P5 %u "
                        "ENAA_P4 %u "
                        "ENAA_P3 %u "
                        "ENAA_P2 %u "
                        "ENAA_P1 %u "
                        "ENAA_P0 %u]\n",
        (unsigned)NRF24L01P_NG_VAL_ENAA_P5(en_aa),
        (unsigned)NRF24L01P_NG_VAL_ENAA_P4(en_aa),
        (unsigned)NRF24L01P_NG_VAL_ENAA_P3(en_aa),
        (unsigned)NRF24L01P_NG_VAL_ENAA_P2(en_aa),
        (unsigned)NRF24L01P_NG_VAL_ENAA_P1(en_aa),
        (unsigned)NRF24L01P_NG_VAL_ENAA_P0(en_aa)
        );
    uint8_t en_rxaddr;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_EN_RXADDR, &en_rxaddr, 1);
    printf(
        "EN_RXADDR      [ERX_P5 %u "
                        "ERX_P4 %u "
                        "ERX_P3 %u "
                        "ERX_P2 %u "
                        "ERX_P1 %u "
                        "ERX_P0 %u]\n",
        (unsigned)NRF24L01P_NG_VAL_ERX_P5(en_rxaddr),
        (unsigned)NRF24L01P_NG_VAL_ERX_P4(en_rxaddr),
        (unsigned)NRF24L01P_NG_VAL_ERX_P3(en_rxaddr),
        (unsigned)NRF24L01P_NG_VAL_ERX_P2(en_rxaddr),
        (unsigned)NRF24L01P_NG_VAL_ERX_P1(en_rxaddr),
        (unsigned)NRF24L01P_NG_VAL_ERX_P0(en_rxaddr)
        );
    uint8_t setup_aw;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_SETUP_AW, &setup_aw, 1);
    printf(
        "SETUP_AW       [AW %u]\n",
        (unsigned)NRF24L01P_NG_VAL_AW(setup_aw)
        );
    uint8_t setup_retr;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_SETUP_RETR, &setup_retr, 1);
    printf(
        "SETUP_RETR     [ARD %u ARC %u]\n",
        (unsigned)NRF24L01P_NG_VAL_ARD(setup_retr),
        (unsigned)NRF24L01P_NG_VAL_ARC(setup_retr)
        );
    uint8_t rf_ch;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RF_CH, &rf_ch, 1);
    printf(
        "RF_CH          [RF_CH %u]\n",
        (unsigned)NRF24L01P_NG_VAL_RF_CH(rf_ch)
        );
    uint8_t rf_setup;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RF_SETUP, &rf_setup, 1);
    printf(
        "RF_SETUP       [CONT_WAVE %u "
                        "RF_DR_LOW %u "
                        "PLL_LOCK %u "
                        "RF_DR_HIGH %u "
                        "RF_PWR %u]\n",
        (unsigned)NRF24L01P_NG_VAL_CONT_WAVE(rf_setup),
        (unsigned)NRF24L01P_NG_VAL_RF_DR_LOW(rf_setup),
        (unsigned)NRF24L01P_NG_VAL_PLL_LOCK(rf_setup),
        (unsigned)NRF24L01P_NG_VAL_RF_DR_HIGH(rf_setup),
        (unsigned)NRF24L01P_NG_VAL_RF_PWR(rf_setup)
        );
    uint8_t status;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_STATUS, &status, 1);
    printf(
        "STATUS         [RX_DR %u "
                        "TX_DS %u "
                        "MAX_RT %u "
                        "RX_P_NO %u "
                        "TX_FULL %u]\n",
        (unsigned)NRF24L01P_NG_VAL_RX_DR(status),
        (unsigned)NRF24L01P_NG_VAL_TX_DS(status),
        (unsigned)NRF24L01P_NG_VAL_MAX_RT(status),
        (unsigned)NRF24L01P_NG_VAL_RX_P_NO(status),
        (unsigned)NRF24L01P_NG_VAL_TX_FULL(status)
        );
    uint8_t observe_tx;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_OBSERVE_TX, &observe_tx, 1);
    printf(
        "OBSERVE_TX     [PLOS_CNT %u ARC_CNT %u]\n",
        (unsigned)NRF24L01P_NG_VAL_PLOS_CNT(observe_tx),
        (unsigned)NRF24L01P_NG_VAL_ARC_CNT(observe_tx)
        );
    uint8_t rpd;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RPD, &rpd, 1);
    printf(
        "RPD            [RPD %u]\n",
        (unsigned)NRF24L01P_NG_VAL_RPD(rpd)
        );
    uint8_t rx_addr_px_40[2][NRF24L01P_NG_MAX_ADDR_WIDTH]; /* Pipe 0/1 */
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P0, rx_addr_px_40[0],
                          NRF24L01P_NG_MAX_ADDR_WIDTH);
    printf(
        "RX_ADDR_P0     [RX_ADDR_P0 %02x:%02x:%02x:%02x:%02x]\n",
        rx_addr_px_40[0][0],
        rx_addr_px_40[0][1],
        rx_addr_px_40[0][2],
        rx_addr_px_40[0][3],
        rx_addr_px_40[0][4]
        );
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P1, rx_addr_px_40[1],
                          NRF24L01P_NG_MAX_ADDR_WIDTH);
    printf(
        "RX_ADDR_P1     [RX_ADDR_P1 %02x:%02x:%02x:%02x:%02x]\n",
        rx_addr_px_40[1][0],
        rx_addr_px_40[1][1],
        rx_addr_px_40[1][2],
        rx_addr_px_40[1][3],
        rx_addr_px_40[1][4]
        );
    uint8_t rx_addr_px_8[4];  /* Pipe 2/3/4/5 */
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P2,
                          &rx_addr_px_8[0], 1);
    printf(
        "RX_ADDR_P2     [RX_ADDR_P2 %02x]\n",
        rx_addr_px_8[0]
        );
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P3,
                          &rx_addr_px_8[1], 1);
    printf(
        "RX_ADDR_P3     [RX_ADDR_P3 %02x]\n",
        rx_addr_px_8[1]
        );
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P4,
                          &rx_addr_px_8[2], 1);
    printf(
        "RX_ADDR_P4     [RX_ADDR_P4 %02x]\n",
        rx_addr_px_8[2]
        );
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P5,
                          &rx_addr_px_8[3], 1);
    printf(
        "RX_ADDR_P5     [RX_ADDR_P5 %02x]\n",
        rx_addr_px_8[3]
        );
    uint8_t tx_addr[NRF24L01P_NG_MAX_ADDR_WIDTH];
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_TX_ADDR, tx_addr,
                          NRF24L01P_NG_MAX_ADDR_WIDTH);
    printf(
        "TX_ADDR        [TX_ADDR %02x:%02x:%02x:%02x:%02x]\n",
        tx_addr[0],
        tx_addr[1],
        tx_addr[2],
        tx_addr[3],
        tx_addr[4]
        );
    uint8_t rx_pw_px[6];
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_PW_P0, &rx_pw_px[0], 1);
    printf(
        "RX_PW_P0       [RX_PW_P0 %u]\n",
        (unsigned)NRF24L01P_NG_VAL_RX_PW_PX(rx_pw_px[0])
        );
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_PW_P1, &rx_pw_px[1], 1);
    printf(
        "RX_PW_P1       [RX_PW_P1 %u]\n",
        (unsigned)NRF24L01P_NG_VAL_RX_PW_PX(rx_pw_px[1])
        );
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_PW_P2, &rx_pw_px[2], 1);
    printf(
        "RX_PW_P2       [RX_PW_P2 %u]\n",
        (unsigned)NRF24L01P_NG_VAL_RX_PW_PX(rx_pw_px[2])
        );
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_PW_P3, &rx_pw_px[3], 1);
    printf(
        "RX_PW_P3       [RX_PW_P3 %u]\n",
        (unsigned)NRF24L01P_NG_VAL_RX_PW_PX(rx_pw_px[3])
        );
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_PW_P4, &rx_pw_px[4], 1);
    printf(
        "RX_PW_P4       [RX_PW_P4 %u]\n",
        (unsigned)NRF24L01P_NG_VAL_RX_PW_PX(rx_pw_px[4])
        );
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_RX_PW_P5, &rx_pw_px[5], 1);
    printf(
        "RX_PW_P5       [RX_PW_P5 %u]\n",
        (unsigned)NRF24L01P_NG_VAL_RX_PW_PX(rx_pw_px[5])
        );
    uint8_t fifo_status;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_FIFO_STATUS, &fifo_status, 1);
    printf(
        "FIFO_STATUS    [TX_REUSE %u "
                        "TX_FULL %u "
                        "TX_EMPTY %u "
                        "RX_FULL %u "
                        "RX_EMPTY %u]\n",
        (unsigned)NRF24L01P_NG_VAL_TX_REUSE(fifo_status),
        (unsigned)NRF24L01P_NG_VAL_TX_FULL_(fifo_status),
        (unsigned)NRF24L01P_NG_VAL_TX_EMPTY(fifo_status),
        (unsigned)NRF24L01P_NG_VAL_RX_FULL(fifo_status),
        NRF24L01P_NG_VAL_RX_EMPTY(fifo_status)
        );
    uint8_t dynpd;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_DYNPD, &dynpd, 1);
    printf(
        "DYNDP          [DPL_P5 %u "
                        "DPL_P4 %u "
                        "DPL_P3 %u "
                        "DPL_P2 %u "
                        "DPL_P1 %u "
                        "DPL_P0 %u]\n",
        (unsigned)NRF24L01P_NG_VAL_DPL_P5(dynpd),
        (unsigned)NRF24L01P_NG_VAL_DPL_P4(dynpd),
        (unsigned)NRF24L01P_NG_VAL_DPL_P3(dynpd),
        (unsigned)NRF24L01P_NG_VAL_DPL_P2(dynpd),
        (unsigned)NRF24L01P_NG_VAL_DPL_P1(dynpd),
        (unsigned)NRF24L01P_NG_VAL_DPL_P0(dynpd)
        );
    uint8_t features;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_FEATURES, &features, 1);
    printf(
        "FEATURES       [EN_DPL %u EN_ACK_PAY %u DYN_ACK %u]\n",
        (unsigned)NRF24L01P_NG_VAL_EN_DPL(features),
        (unsigned)NRF24L01P_NG_VAL_EN_ACK_PAY(features),
        (unsigned)NRF24L01P_NG_VAL_EN_DYN_ACK(features)
        );
}

void nrf24l01p_ng_diagnostics_print_dev_info(const nrf24l01p_ng_t *dev)
{
    printf("device:                 %p\n", (const void *)dev);
    printf("address length:         %u\n", (unsigned)NRF24L01P_NG_ADDR_WIDTH);
    printf("device params:\n");
    printf("\tChannel:              %u\n",
           dev->params.config.cfg_channel);
    printf("\tCRC length:           %u bytes\n",
           (unsigned)nrf24l01p_ng_etoval_crc(dev->params.config.cfg_crc));
    printf("\tData rate:            %u kbps\n",
           (unsigned)
           nrf24l01p_ng_etoval_rfdr(dev->params.config.cfg_data_rate));
    printf("\tMax. retransmissions: %u\n",
           dev->params.config.cfg_max_retr);
    printf("\tRetransmission delay: %u us\n",
           (unsigned)
           nrf24l01p_ng_etoval_ard(dev->params.config.cfg_retr_delay));
    printf("\tTx power:             %d dbm\n",
           nrf24l01p_ng_etoval_tx_power(dev->params.config.cfg_tx_power));
    printf("\tRx address p0:       "ADDR_FMT"\n",
           ADDR_P0(dev));
    printf("\tRX address p1:       "ADDR_FMT"\n",
           ADDR_P1(dev));
    printf("\tRX address p2:        %02x\n",
           NRF24L01P_NG_ADDR_PX_LSB(dev, NRF24L01P_NG_P2));
    printf("\tRX address p3:        %02x\n",
           NRF24L01P_NG_ADDR_PX_LSB(dev, NRF24L01P_NG_P3));
    printf("\tRX address p4:        %02x\n",
           NRF24L01P_NG_ADDR_PX_LSB(dev, NRF24L01P_NG_P4));
    printf("\tRX address p5:        %02x\n",
           NRF24L01P_NG_ADDR_PX_LSB(dev, NRF24L01P_NG_P5));

    printf("State:                  %s\n",
           nrf24l01p_ng_diagnostics_state_to_string(dev->state));
}

void nrf24l01p_ng_diagnostics_print_frame(const nrf24l01p_ng_t *dev,
                                          const void *frame, size_t len)
{
    (void)dev;
    puts("Rx frame");
    for (uint8_t i = 0; i < len; i++) {
        printf("0x%02X ", ((uint8_t *)frame)[i]);
    }
    puts("");
}
