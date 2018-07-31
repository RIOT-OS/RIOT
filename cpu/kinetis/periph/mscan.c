/*
 * Copyright (C) 2018 HERE Deutschland GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
* @ingroup     cpu_kinetis_mscan
*
* @{
*
* @file
* @brief       MSCAN driver implementation
*
* @author      Anton Gerasimov <anton.gerasimov@here.com>
*
* @}
*/

#include "cpu.h"
#include "periph/uart.h"
#include "can/candev.h"
#include "can/device.h"
#include "mscan.h"

#ifdef MODULE_PERIPH_MSCAN
static candev_t *_dev;

static volatile uint8_t last_rflg;
static volatile uint8_t last_tflg;

static volatile struct can_frame recv_frame;

static struct can_frame *tx_frames[3]; // three hardware TX buffers

static struct can_filter filters[2];
static bool filter_active[2];

static const struct can_bittiming_const const_bittiming = {
    .tseg1_min = 4,
    .tseg1_max = 16,
    .tseg2_min = 2,
    .tseg2_max = 8,
    .sjw_max = 4,
    .brp_min = 1,
    .brp_max = 64,
    .brp_inc = 1,
};

static void disable_can_irq(void)
{
    NVIC_DisableIRQ(MSCAN_RX_IRQn);
    NVIC_DisableIRQ(MSCAN_TX_IRQn);
}

static void enable_can_irq(void)
{
    NVIC_EnableIRQ(MSCAN_RX_IRQn);
    NVIC_EnableIRQ(MSCAN_TX_IRQn);
}

void isr_mscan_rx(void)
{
    if (MSCAN->CANRFLG & 1) { //packet received
        last_rflg |= 1;
        if (MSCAN->REIDR1 & 0x08) {
            // extended ID
            recv_frame.can_id = (1UL << 31) |
                                (MSCAN->REIDR3 >> 1) |
                                ((MSCAN->REIDR2) << 7) |
                                ((MSCAN->REIDR1 & 0x07) << 15) |
                                ((MSCAN->REIDR1 >> 5) << 18) |
                                (MSCAN->REIDR0 << 21);
        }
        else {
            // standard ID
            recv_frame.can_id = MSCAN->RSIDR1 >> 5 |
                                MSCAN->RSIDR0 << 3;
        }
        recv_frame.can_dlc = MSCAN->RDLR & 0x0F;
        for (int i = 0; i < recv_frame.can_dlc; i++) {
            recv_frame.data[i] = MSCAN->REDSR[i];
        }

        MSCAN->CANRFLG |= 1;
        _dev->event_callback(_dev, CANDEV_EVENT_ISR, NULL);
    }
}


void isr_mscan_tx(void)
{
    uint8_t bufmask = MSCAN->CANTFLG & MSCAN->CANTIER & 0x07;           // three hardware buffers
    uint8_t statmask = (MSCAN->CANRFLG & 0x3C) << 2;                    // current CAN RX/TX status

    if ((MSCAN->CANRIER & (1 << 7)) && (MSCAN->CANRFLG & (1 << 7))) {   // wake-up
        last_rflg |= (1 << 7);
        MSCAN->CANRFLG |= (1 << 7);
    }

    if ((MSCAN->CANRIER & (1 << 6)) && (MSCAN->CANRFLG & (1 << 6))) { //status change
        last_rflg |= (1 << 6) | statmask;
        MSCAN->CANRFLG |= (1 << 6);
    }

    if ((MSCAN->CANRIER & (1 << 1)) && (MSCAN->CANRFLG & (1 << 1))) { //overrun
        last_rflg |= (1 << 1);
        MSCAN->CANRFLG |= (1 << 1);
    }

    if (bufmask) {
        last_tflg |= bufmask;
        MSCAN->CANTIER &= ~bufmask;
    }
    _dev->event_callback(_dev, CANDEV_EVENT_ISR, NULL);
}

static void can_to_init_mode(void);
static void can_from_init_mode(void);
static void set_bittiming(candev_t *dev);

static int _init(candev_t *dev)
{
    _dev = dev;

    candev_kea_t *kea_dev = (candev_kea_t *) dev;

    disable_can_irq();
    filter_active[0] = filter_active[1] = false;

    SIM->SCGC |= SIM_SCGC_MSCAN_MASK;

    can_to_init_mode();

    set_bittiming(dev);

    MSCAN->CANCTL1 &= ~(1 << 4); // exit listen-only mode

    can_from_init_mode();

    kea_dev->mode = CANOPT_STATE_ON;

    MSCAN->CANRIER =     1  |       // RX interrupt
                     (0x3 << 2) |   // All receive status changes
                     (0x3 << 4) |   // All tranmit status changes
                     (0x1 << 6);    // Status interrupt
    enable_can_irq();
    return 0;
}

static int _send(candev_t *dev, const struct can_frame *frame)
{
    (void) dev;
    int res = -1;
    disable_can_irq();
    for (int i = 0; i < 3; i++) {
        if (MSCAN->CANTFLG  & (1 << i)) {
            uint8_t bufmask = 1 << i;
            MSCAN->CANTIER &= ~bufmask;
            uint32_t af = frame->can_id;
            uint8_t dlc = frame->can_dlc;

            if (dlc > 8) {
                break;
            }

            MSCAN->CANTBSEL = (1 << i);

            if (af & 0x80000000) {
                MSCAN->TEIDR3 = (af << 1) & 0xFF;
                MSCAN->TEIDR2 = (af >> 7) & 0xFF;
                MSCAN->TEIDR1 = ((af >> 15) & 0x07) |
                                0x18 | // SRR and extended flags
                                ((af >> 18) & 0x07) << 5;
                MSCAN->TEIDR0 = (af >> 21) & 0xFF;
            }
            else {
                MSCAN->TSIDR1 = (af & 0x07) << 5;
                MSCAN->TSIDR0 = (af >> 3) & 0xFF;
            }

            MSCAN->TDLR = dlc;
            for (int j = 0; j < dlc; j++) {
                MSCAN->TEDSR[j] = frame->data[j];
            }

            MSCAN->CANTFLG = bufmask; // schedule for transmission
            MSCAN->CANTIER |= bufmask;

            tx_frames[i] = (struct can_frame *)frame;
            /* Confirmation should be given on TX interrupt, but that doesn't work due to unidentified bug in CAN subsystem */
            dev->event_callback(dev, CANDEV_EVENT_TX_CONFIRMATION, (void *)(frame));
            res = 0;
            break;
        }
    }

    enable_can_irq();
    return res;
}

static int _abort(candev_t *dev, const struct can_frame *frame)
{
    (void) dev;
    if (frame == NULL) {
        return -1;
    }

    for (int i = 0; i < 3; ++i) {
        if (tx_frames[i] == frame) {
            MSCAN->CANTARQ = (1 << i);
            return 0;
        }
    }
    return -1;
}

uint8_t canrier_tmp = 0x00;
uint8_t cantier_tmp = 0x00;
uint8_t canctl0_tmp = 0x00;

static void can_to_init_mode(void)
{
    canrier_tmp = MSCAN->CANRIER;
    cantier_tmp = MSCAN->CANTIER;
    canctl0_tmp = MSCAN->CANCTL0;

    if (MSCAN->CANCTL1 & (1 << 7)) {            // CANE == 1, reinit
        MSCAN->CANCTL0 |= (1 << 1);             // request transition into sleep mode
        while (!(MSCAN->CANCTL1 & (1 << 1))) {  // wait for transiton into sleep mode

        }
        MSCAN->CANCTL0 |= 1;            // request transition into init mode
        while (!(MSCAN->CANCTL1 & 1)) { // initialization is acknowledged
        }
    }
    else {
        MSCAN->CANCTL1 |= (1 << 7) | (1 << 6);  // enable CAN, clock from bus clock
        while (!(MSCAN->CANCTL1 & 1)) {         // initialization is acknowledged
        }
    }
}

static void can_from_init_mode(void)
{
    MSCAN->CANCTL0 = canctl0_tmp & ~(1);    // exit initialization mode
    while (MSCAN->CANCTL1 & 1) {            // initialization is acknowledged

    }
    MSCAN->CANRIER = canrier_tmp;
    MSCAN->CANTIER = cantier_tmp;
}

static void can_set_mode(candev_t *dev, canopt_state_t mode)
{
    candev_kea_t *kea_dev = (candev_kea_t *) dev;

    if (kea_dev->mode == mode) {
        return;
    }

    can_to_init_mode();
    switch (mode) {
        case CANOPT_STATE_OFF:
            SIM->SCGC &= ~SIM_SCGC_MSCAN_MASK;
            break;

        case CANOPT_STATE_SLEEP:
            if (kea_dev->mode == CANOPT_STATE_OFF) {
                _init(_dev);
            }
            MSCAN->CANCTL0 |= (1 << 1);             // request transition into sleep mode
            while (!(MSCAN->CANCTL1 & (1 << 1))) {  // wait for transiton into sleep mode
            }
            break;

        case CANOPT_STATE_LISTEN_ONLY:
            if (kea_dev->mode == CANOPT_STATE_OFF) {
                _init(_dev);
            }

            MSCAN->CANCTL1 |= (1 << 4);

            if (kea_dev->mode == CANOPT_STATE_SLEEP) {
                MSCAN->CANCTL0 &= ~(1 << 1);            // request return from sleep mode
                while ((MSCAN->CANCTL1 & (1 << 1))) {   // wait for return from sleep mode
                }
            }
            break;

        case CANOPT_STATE_ON:
            if (kea_dev->mode == CANOPT_STATE_OFF) {
                _init(_dev);
            }
            else if (kea_dev->mode == CANOPT_STATE_SLEEP) {
                MSCAN->CANCTL0 &= ~(1 << 1);            // request return from sleep mode
                while ((MSCAN->CANCTL1 & (1 << 1))) {   // wait for return from sleep mode
                }
            }
            else if (CANOPT_STATE_LISTEN_ONLY) {
                MSCAN->CANCTL1 &= ~(1 << 4);
            }
            break;
    }

    can_from_init_mode();
    kea_dev->mode = mode;
}

static void set_bittiming(candev_t *dev)
{
    MSCAN->CANBTR0 = ((dev->bittiming.brp - 1) & 0x3F) | ((dev->bittiming.sjw - 1) << 6);
    MSCAN->CANBTR1 = ((dev->bittiming.prop_seg + dev->bittiming.phase_seg1) - 1) | ((dev->bittiming.phase_seg2 - 1) << 4);
}


static void _isr(candev_t *dev)
{
    struct can_frame frame;
    uint8_t rflg;
    uint8_t tflg;

    disable_can_irq();
    rflg = last_rflg;
    tflg = last_tflg;

    if (rflg & 1) {
        frame = recv_frame;
    }
    last_rflg = last_tflg = 0x00;
    enable_can_irq();

    if (dev->event_callback) {
        // receive interrupt
        if (rflg & 1) {
            dev->event_callback(dev, CANDEV_EVENT_RX_INDICATION, &frame);
        }

        // overrun interupt
        if (rflg & (1 << 1)) {
            dev->event_callback(dev, CANDEV_EVENT_RX_ERROR, NULL);
        }

        // status change interupt
        if (rflg & (1 << 6)) {
            uint8_t tstat = (rflg >> 2) & 0x3;
            uint8_t rstat = (rflg >> 4) & 0x3;
            if (tstat == 3) {
                dev->event_callback(dev, CANDEV_EVENT_BUS_OFF, NULL);
            }
            else if (tstat == 2) {
                dev->event_callback(dev, CANDEV_EVENT_ERROR_PASSIVE, NULL);
            }
            else if (tstat == 1) {
                dev->event_callback(dev, CANDEV_EVENT_ERROR_WARNING, NULL);
            }
            else if (rstat != 0) {
                dev->event_callback(dev, CANDEV_EVENT_RX_ERROR, NULL);
            }
        }

        // wake-up interupt
        if (rflg & (1 << 7)) {
            dev->event_callback(dev, CANDEV_EVENT_WAKE_UP, NULL);
        }

        // TX empty interupt
        if (tflg) {
            for (int i = 0; i < 3; ++i) {
                if ((tflg & (1 << i)) && (tx_frames[i] != NULL)) {
                    /* Confirmation should be given here, but doesn't work due to unidentified bug in CAN subsystem */
                    tx_frames[i] = NULL;
                }
            }
        }
    }
}

static int _get(candev_t *dev, canopt_t opt, void *value, size_t max_len)
{
    switch (opt) {
        case CANOPT_BITTIMING: {
            if (value == NULL) {
                return -EINVAL;
            }

            if (max_len < sizeof(struct can_bittiming)) {
                return -EOVERFLOW;
            }

            *((struct can_bittiming *)value) = dev->bittiming; // does it really make sense?
            return sizeof(struct can_bittiming);
        }

        case CANOPT_BITTIMING_CONST: {
            if (value == NULL) {
                return -EINVAL;
            }

            if (max_len < sizeof(struct can_bittiming)) {
                return -EOVERFLOW;
            }


            *((struct can_bittiming_const *)value) = const_bittiming;
            return sizeof(struct can_bittiming_const);
        }

        case CANOPT_CLOCK: {
            if (value == NULL) {
                return -EINVAL;
            }

            if (max_len < sizeof(uint32_t)) {
                return -EOVERFLOW;
            }

            *((uint32_t *)value) = ((candev_kea_t *)dev)->conf->clock_freq;
            return sizeof(uint32_t);
        }

        case CANOPT_TEC:
        case CANOPT_REC:
            // if someone wants to implement this, remember to enter sleep mode for reading
            return -ENOTSUP;

        case CANOPT_RX_FILTERS: {
            if (value == NULL) {
                return -EINVAL;
            }

            if ((max_len % sizeof(struct can_filter)) != 0) {
                return -EOVERFLOW;
            }

            struct can_filter *out = (struct can_filter *) value;
            int num_filters = max_len / sizeof(struct can_filter);
            int num = 0;
            for (int idx = 0; idx < 2 && num < num_filters; idx++) {
                if (filter_active[idx]) {
                    out[num++] = filters[idx];
                }
            }

            return num * sizeof(struct can_filter);
        }
        default:
            return -ENOTSUP;
    }
}

static int _set(candev_t *dev, canopt_t opt, void *value, size_t value_len)
{
    switch (opt) {
        case CANOPT_BITTIMING:
            if (value_len != sizeof(struct can_bittiming)) {
                return -EOVERFLOW;
            }
            dev->bittiming = *((struct can_bittiming *) value);
            can_to_init_mode();
            set_bittiming(dev);
            can_from_init_mode();
            return sizeof(struct can_bittiming);

        case CANOPT_STATE:
            if (value_len != sizeof(canopt_state_t)) {
                return -EOVERFLOW;
            }
            can_set_mode(dev, *((canopt_state_t *) value));
            return sizeof(canopt_state_t);
        default:
            return -ENOTSUP;
    }
}

static int _set_filter(candev_t *dev, const struct can_filter *filter)
{
    (void) dev;
    volatile uint8_t *canidar_bank;
    volatile uint8_t *canidmr_bank;

    if (!filter_active[0]) {
        canidar_bank = MSCAN->CANIDAR_BANK_1;
        canidmr_bank = MSCAN->CANIDMR_BANK_1;
        filter_active[0] = true;
        filters[0] = *filter;
    }
    else if (!filter_active[1]) {
        canidar_bank = MSCAN->CANIDAR_BANK_2;
        canidmr_bank = MSCAN->CANIDMR_BANK_2;
        filter_active[1] = true;
        filters[1] = *filter;
    }
    else {
        return -1;
    }

    can_to_init_mode();

    uint32_t can_id = filter->can_id;
    uint32_t mask = ~filter->can_mask;  // MSCAN's masks are inverted, i.e. 1 is 'don't care'
    if (can_id & 0x80000000) {          // CAN 2.0B ID
        canidar_bank[0] = (can_id >> 21) & 0xFF;
        canidar_bank[1] = ((can_id >> 15) & 0x07) |
                          (1 << 3) |
                          (1 << 4) |
                          (((can_id >> 18) & 0x07) << 5);
        canidar_bank[2] = (can_id >> 7) & 0xFF;
        canidar_bank[3] = (can_id << 1) & 0xFE; //ignoring rtr

        canidmr_bank[0] = (mask >> 21) & 0xFF;
        canidmr_bank[1] = (mask >> 15) & 0xEF;
        canidmr_bank[2] = (mask >> 7) & 0xFF;
        canidmr_bank[3] = ((mask << 1) & 0xFF) | 1; //ignoring rtr
    }
    else {
        canidar_bank[0] = (can_id >> 3) & 0xFF;
        canidar_bank[1] = (can_id & 0x07) << 5;

        canidmr_bank[0] = (mask >> 3) & 0xFF;
        canidmr_bank[1] = (mask & 0x07) << 5 |
                          (1 << 4) |    // ignoring rtr
                          0x07;         //ignoring reserved
    }
    can_from_init_mode();
    return 0;
}

static int _remove_filter(candev_t *dev, const struct can_filter *filter)
{
    (void) dev;
    if (filter_active[0] && (filters[0].can_id == filter->can_id) && (filters[0].can_mask == filter->can_mask)) {
        filter_active[0] = false;
        can_to_init_mode();
        MSCAN->CANIDMR_BANK_1[0] = MSCAN->CANIDMR_BANK_1[1] = MSCAN->CANIDMR_BANK_1[2] = MSCAN->CANIDMR_BANK_1[3] = 0x00;
        can_from_init_mode();
        return 0;
    }

    if (filter_active[1] && (filters[0].can_id == filter->can_id) && (filters[0].can_mask == filter->can_mask)) {
        filter_active[1] = false;
        can_to_init_mode();
        MSCAN->CANIDMR_BANK_2[0] = MSCAN->CANIDMR_BANK_2[1] = MSCAN->CANIDMR_BANK_2[2] = MSCAN->CANIDMR_BANK_2[3] = 0x00;
        can_from_init_mode();
        return 0;
    }

    return -1;
}

candev_driver_t candev_kea_driver = {
    .send = _send,
    .abort = _abort,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
    .set_filter = _set_filter,
    .remove_filter = _remove_filter,
};

int candev_kea_init(candev_kea_t *dev, const candev_kea_conf_t *conf)
{
    memset(dev, 0, sizeof(candev_kea_t));
    dev->candev.driver = &candev_kea_driver;
    dev->conf = conf;
    dev->candev.bittiming.bitrate = CANDEV_KEA_DEFAULT_BITRATE;
    dev->candev.bittiming.sample_point = CANDEV_KEA_DEFAULT_SPT;

    return can_device_calc_bittiming(conf->clock_freq, &const_bittiming, &dev->candev.bittiming);
}
#else
typedef int dont_be_pedantic;
#endif
