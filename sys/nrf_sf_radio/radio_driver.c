/*
 * SPDX-FileCopyrightText: 2026 Xin He
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_nrf_sf_radio
 * @{
 *
 * @file
 * @brief       Physical layer radio, timer and PPI configuration
 *
 * @author      Xin He <xin.he@mailbox.tu-dresden.de>
 *
 * @}
 */

#include <stddef.h>
#include "nrf_clock.h"
#include "nrf_sf_radio/radio_driver.h"

static void _timer2_init(void);
static int _timer_ppi_init(void);
static void _timer2_reset_tx(void);
static void _timer2_reset_rx(void);
static void _init_radio(void);

static const uint8_t ble_hw_frequency_channels[40] = {
    4,  6,  8, 10, 12, 14, 16, 18, 20, 22,  /* 0-9 */
    24, 28, 30, 32, 34, 36, 38, 40, 42, 44, /* 10-19 */
    46, 48, 50, 52, 54, 56, 58, 60, 62, 64, /* 20-29 */
    66, 68, 70, 72, 74, 76, 78,  2, 26, 80, /* 30-39 */
};

#ifndef NRF_SF_RADIO_ACCESS_ADDRESS
#  define NRF_SF_RADIO_ACCESS_ADDRESS (0x8E89BED6u)
#endif
#ifndef NRF_SF_RADIO_FAST_RAMPUP
#  define NRF_SF_RADIO_FAST_RAMPUP (1U)
#endif
#define AA_PREFIX ((uint32_t)((NRF_SF_RADIO_ACCESS_ADDRESS >> 24) & 0xFFu))
#define AA_BASE   ((uint32_t)((NRF_SF_RADIO_ACCESS_ADDRESS << 8) & 0xFFFFFF00u))

#define READY_TIME_PPI_CH   (9U)
#define END_TIME_PPI_CH     (10U)
#define TX_START_PPI_CH     (11U)
#define TIMER2_START_PPI_CH (12U)
#define RX_EN_PPI_CH        (13U)
#define TX_EN_PPI_CH        (15U)
#define ADDRESS_TIME_PPI_CH (14U)

static int _ppi_anchor_inited;

static void _timer2_init(void)
{
    NRF_TIMER2->TASKS_SHUTDOWN = 1;

    NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
    NRF_TIMER2->PRESCALER = 0;
    NRF_TIMER2->TASKS_CLEAR = 1;

    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    NRF_TIMER2->INTENCLR = 0xffffffffUL;

    NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_STOP_Msk |
                         TIMER_SHORTS_COMPARE0_CLEAR_Msk;

    NRF_PPI->CH[TIMER2_START_PPI_CH].EEP =
        (uint32_t)&NRF_TIMER3->EVENTS_COMPARE[0];
    NRF_PPI->CH[TIMER2_START_PPI_CH].TEP =
        (uint32_t)&NRF_TIMER2->TASKS_START;

    NRF_PPI->CH[TX_START_PPI_CH].EEP =
        (uint32_t)&NRF_TIMER2->EVENTS_COMPARE[0];
    NRF_PPI->CH[TX_START_PPI_CH].TEP = (uint32_t)&NRF_RADIO->TASKS_START;

    NRF_PPI->CHENSET = (1UL << TIMER2_START_PPI_CH) |
                       (1UL << TX_START_PPI_CH);
}

static int _timer_ppi_init(void)
{
    if (_ppi_anchor_inited) {
        return 0;
    }

    NRF_TIMER3->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER3->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
    NRF_TIMER3->PRESCALER = 0;
    NRF_TIMER3->TASKS_CLEAR = 1;
    NRF_TIMER3->TASKS_START = 1;

    NRF_PPI->CH[TX_EN_PPI_CH].EEP = (uint32_t)&NRF_TIMER3->EVENTS_COMPARE[0];
    NRF_PPI->CH[TX_EN_PPI_CH].TEP = (uint32_t)&NRF_RADIO->TASKS_TXEN;

    NRF_PPI->CH[RX_EN_PPI_CH].EEP = (uint32_t)&NRF_TIMER3->EVENTS_COMPARE[0];
    NRF_PPI->CH[RX_EN_PPI_CH].TEP = (uint32_t)&NRF_RADIO->TASKS_RXEN;

    NRF_PPI->CH[ADDRESS_TIME_PPI_CH].EEP = (uint32_t)&NRF_RADIO->EVENTS_ADDRESS;
    NRF_PPI->CH[ADDRESS_TIME_PPI_CH].TEP = (uint32_t)&NRF_TIMER3->TASKS_CAPTURE[1];
    NRF_PPI->CHENSET = (1UL << ADDRESS_TIME_PPI_CH);

    NRF_PPI->CH[END_TIME_PPI_CH].EEP = (uint32_t)&NRF_RADIO->EVENTS_END;
    NRF_PPI->CH[END_TIME_PPI_CH].TEP = (uint32_t)&NRF_TIMER3->TASKS_CAPTURE[3];
    NRF_PPI->CHENSET = (1UL << END_TIME_PPI_CH);

    NRF_PPI->CH[READY_TIME_PPI_CH].EEP = (uint32_t)&NRF_RADIO->EVENTS_READY;
    NRF_PPI->CH[READY_TIME_PPI_CH].TEP = (uint32_t)&NRF_TIMER3->TASKS_CAPTURE[4];
    NRF_PPI->CHENSET = (1UL << READY_TIME_PPI_CH);

    _timer2_init();
    _ppi_anchor_inited = 1;

    return 0;
}

void nrf_sf_radio_wait_until(volatile uint32_t *event, uint32_t timeout_ticks)
{
    uint32_t start_ticks = nrf_sf_radio_now_ticks();

    nrf_sf_radio_wait_until_abs(event, start_ticks + timeout_ticks);
}

void nrf_sf_radio_wait_until_abs(volatile uint32_t *event, uint32_t deadline)
{
    while (((event == NULL) || (*event == 0U)) &&
           ((int32_t)(nrf_sf_radio_now_ticks() - deadline) < 0)) {}
}

uint32_t nrf_sf_radio_get_last_ready_time_ticks(void)
{
    return NRF_TIMER3->CC[4];
}

uint32_t nrf_sf_radio_get_last_end_time_ticks(void)
{
    return NRF_TIMER3->CC[3];
}

uint32_t nrf_sf_radio_get_last_address_time_ticks(void)
{
    return NRF_TIMER3->CC[1];
}

uint32_t nrf_sf_radio_now_ticks(void)
{
    NRF_TIMER3->TASKS_CAPTURE[2] = 1;
    return NRF_TIMER3->CC[2];
}

static void _timer2_reset_tx(void)
{
    NRF_PPI->CHENSET = (1UL << TX_EN_PPI_CH);
    NRF_PPI->CHENCLR = (1UL << RX_EN_PPI_CH);

    NRF_TIMER2->TASKS_SHUTDOWN = 1;
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    NRF_TIMER2->CC[0] = NRF_SF_RADIO_RAMPUP_TIME_TICKS;

    NRF_TIMER3->EVENTS_COMPARE[0] = 0;
    NRF_RADIO->EVENTS_DISABLED = 0U;
    NRF_RADIO->TASKS_DISABLE = 1U;

    NRF_RADIO->EVENTS_END = 0U;
    NRF_RADIO->EVENTS_ADDRESS = 0U;
    NRF_RADIO->EVENTS_READY = 0U;

    nrf_sf_radio_wait_until(&NRF_RADIO->EVENTS_DISABLED,
                            NRF_SF_RADIO_US_TO_TIMER_TICKS(2U));
    NRF_RADIO->EVENTS_DISABLED = 0U;
}

void nrf_sf_radio_tx_arm(uint8_t *buf, uint32_t deadline_ticks)
{
    _timer2_reset_tx();
    NRF_TIMER3->EVENTS_COMPARE[0] = 0;
    NRF_TIMER3->CC[0] = deadline_ticks;
    NRF_RADIO->PACKETPTR = (uint32_t)buf;
}

static void _timer2_reset_rx(void)
{
    NRF_PPI->CHENSET = (1UL << RX_EN_PPI_CH);
    NRF_PPI->CHENCLR = (1UL << TX_EN_PPI_CH);

    NRF_TIMER2->TASKS_SHUTDOWN = 1;
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    NRF_TIMER2->CC[0] = NRF_SF_RADIO_RAMPUP_TIME_TICKS;

    NRF_TIMER3->EVENTS_COMPARE[0] = 0;
    NRF_RADIO->EVENTS_DISABLED = 0U;
    NRF_RADIO->TASKS_DISABLE = 1U;

    NRF_RADIO->EVENTS_END = 0U;
    NRF_RADIO->EVENTS_ADDRESS = 0U;
    NRF_RADIO->EVENTS_READY = 0U;

    nrf_sf_radio_wait_until(&NRF_RADIO->EVENTS_DISABLED,
                            NRF_SF_RADIO_US_TO_TIMER_TICKS(2U));
    NRF_RADIO->EVENTS_DISABLED = 0U;
}

void nrf_sf_radio_rx_arm(uint8_t *buf, uint32_t deadline_ticks)
{
    _timer2_reset_rx();
    NRF_TIMER3->EVENTS_COMPARE[0] = 0;
    NRF_TIMER3->CC[0] = deadline_ticks;
    NRF_RADIO->PACKETPTR = (uint32_t)buf;
}

void nrf_sf_radio_try_rx_enable(uint8_t *buf)
{
    NRF_RADIO->EVENTS_DISABLED = 0U;
    NRF_RADIO->TASKS_DISABLE = 1;
    nrf_sf_radio_wait_until(&NRF_RADIO->EVENTS_DISABLED,
                            NRF_SF_RADIO_US_TO_TIMER_TICKS(2U));

    NRF_TIMER2->TASKS_SHUTDOWN = 1;
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    NRF_TIMER2->CC[0] = NRF_SF_RADIO_RAMPUP_TIME_TICKS;

    NRF_RADIO->EVENTS_READY = 0U;
    NRF_RADIO->EVENTS_DISABLED = 0U;
    NRF_RADIO->EVENTS_END = 0U;
    NRF_RADIO->EVENTS_ADDRESS = 0U;
    NRF_RADIO->EVENTS_PAYLOAD = 0U;

    NRF_RADIO->PACKETPTR = (uint32_t)buf;

    NRF_RADIO->TASKS_RXEN = 1;
    NRF_TIMER2->TASKS_START = 1;
}

void nrf_sf_radio_set_mode(uint32_t mode)
{
    NRF_RADIO->MODE = mode;
}

void nrf_sf_radio_set_ble_channel(uint8_t channel)
{
    NRF_RADIO->FREQUENCY = ble_hw_frequency_channels[channel];
    NRF_RADIO->DATAWHITEIV = channel;
}

void nrf_sf_radio_set_power(uint16_t power)
{
    if (power > 2) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Pos4dBm;
    }
    else if (power > -2) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_0dBm;
    }
    else if (power > -6) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg4dBm;
    }
    else if (power > -10) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg8dBm;
    }
    else if (power > -14) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg12dBm;
    }
    else if (power > -18) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg16dBm;
    }
    else if (power > -25) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg20dBm;
    }
    else {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg30dBm;
    }
}

static void _init_radio(void)
{
    clock_hfxo_request();

    NRF_RADIO->POWER = 1;
    NRF_RADIO->TASKS_DISABLE = 1;
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->EVENTS_ADDRESS = 0;

    NRF_RADIO->MODE = RADIO_MODE_MODE_Ble_1Mbit;
    uint32_t modecnf0 = (RADIO_MODECNF0_DTX_Center << RADIO_MODECNF0_DTX_Pos);
#if NRF_SF_RADIO_FAST_RAMPUP
    modecnf0 |= (RADIO_MODECNF0_RU_Fast << RADIO_MODECNF0_RU_Pos);
#else
    modecnf0 |= (RADIO_MODECNF0_RU_Default << RADIO_MODECNF0_RU_Pos);
#endif
    NRF_RADIO->MODECNF0 = modecnf0;

    NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_0dBm;
    NRF_RADIO->FREQUENCY = ble_hw_frequency_channels[37];
    NRF_RADIO->DATAWHITEIV = 37;

    NRF_RADIO->PREFIX0 = AA_PREFIX;
    NRF_RADIO->BASE0 = AA_BASE;
    NRF_RADIO->TXADDRESS = 0;
    NRF_RADIO->RXADDRESSES = 1;


    NRF_RADIO->PCNF0 = (1 << RADIO_PCNF0_S0LEN_Pos) |
                       (8 << RADIO_PCNF0_LFLEN_Pos) |
                       (RADIO_PCNF0_PLEN_8bit << RADIO_PCNF0_PLEN_Pos) |
                       (RADIO_PCNF0_S1INCL_Automatic <<
                        RADIO_PCNF0_S1INCL_Pos) |
                       (0 << RADIO_PCNF0_S1LEN_Pos);
    NRF_RADIO->PCNF1 = (251 << RADIO_PCNF1_MAXLEN_Pos) |
                       (3 << RADIO_PCNF1_BALEN_Pos) |
                       (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos) |
                       (RADIO_PCNF1_WHITEEN_Enabled << RADIO_PCNF1_WHITEEN_Pos);

    NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) |
                        (RADIO_CRCCNF_SKIPADDR_Skip << RADIO_CRCCNF_SKIPADDR_Pos);
    NRF_RADIO->CRCINIT = 0x555555;
    NRF_RADIO->CRCPOLY = 0x00065B;

    NRF_RADIO->SHORTS = (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);
}

void nrf_sf_radio_start(void)
{
    _init_radio();
    _timer_ppi_init();
}
