/*
 * Copyright (C) 2016-2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of the CAN controller driver
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <cpu_conf.h>

#include "periph/can.h"
#include "periph/gpio.h"
#include "can/device.h"
#include "can/common.h"
#include "periph_conf.h"
#include "pm_layered.h"
#include "thread.h"
#include "sched.h"
#include "mutex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define CAN_MAX_WAIT_CHANGE (10000U)
#define CAN_TIxR_SFF_SHIFT  (21U)
#define CAN_TIxR_EFF_SHIFT  (3U)

#define CAN_MCR_BIT_DONT_TOUCH (0xFFFFFF03)

typedef enum {
    MODE_NORMAL,
    MODE_SLEEP,
    MODE_INIT,
} can_mode_t;

static int _init(candev_t *candev);
static void _isr(candev_t *candev);
static int _send(candev_t *candev, const struct can_frame *frame);
static int _abort(candev_t *candev, const struct can_frame *frame);
static int _set(candev_t *candev, canopt_t opt, void *value, size_t value_len);
static int _get(candev_t *candev, canopt_t opt, void *value, size_t max_len);
static int _set_filter(candev_t *candev, const struct can_filter *filter);
static int _remove_filter(candev_t *candev, const struct can_filter *filter);

static void tx_irq_handler(can_t *dev);
static void tx_isr(can_t *dev);
static void tx_conf(can_t *dev, int mailbox);
static void rx_irq_handler(can_t *dev, int mailbox);
static void rx_isr(can_t *dev);
static void sce_irq_handler(can_t *dev);

static inline void set_bit_timing(can_t *dev);

static inline can_mode_t get_mode(CAN_TypeDef *can);
static int set_mode(CAN_TypeDef *can, can_mode_t mode);

static const candev_driver_t candev_stm32_driver = {
    .send = _send,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
    .abort = _abort,
    .set_filter = _set_filter,
    .remove_filter = _remove_filter,
};

static const struct can_bittiming_const bittiming_const = {
    .tseg1_min = 1,
    .tseg1_max = 16,
    .tseg2_min = 1,
    .tseg2_max = 8,
    .sjw_max = 1,
    .brp_min = 1,
    .brp_max = 1024,
    .brp_inc = 1,
};

enum {
    STATUS_NOT_USED,
    STATUS_ON,
    STATUS_READY_FOR_SLEEP,
    STATUS_SLEEP,
};

static uint8_t _status[CANDEV_STM32_CHAN_NUMOF];

static can_t *_can[CANDEV_STM32_CHAN_NUMOF];

static inline int get_channel(CAN_TypeDef *can)
{
#if CANDEV_STM32_CHAN_NUMOF == 1
    (void)can;
    return 0;
#else
    return (int) (((uint32_t)can - (uint32_t)CAN1) >> 10);
#endif
}

static inline can_mode_t get_mode(CAN_TypeDef *can)
{
    if ((can->MCR & CAN_MCR_SLEEP) == CAN_MCR_SLEEP) {
        return MODE_SLEEP;
    }
    else if ((can->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) {
        return MODE_INIT;
    }
    else {
        return MODE_NORMAL;
    }
}

static int set_mode(CAN_TypeDef *can, can_mode_t mode)
{
    int max_loop = CAN_MAX_WAIT_CHANGE;
    int res = 0;

    switch (mode) {
        case MODE_NORMAL:
            can->MCR &= ~(CAN_MCR_SLEEP | CAN_MCR_INRQ);
            /* wait for hardware confirmation */
            while (((can->MSR & CAN_MSR_INAK) || (can->MSR & CAN_MSR_SLAK)) && max_loop != 0) {
                max_loop--;
            }
            break;
        case MODE_SLEEP:
            /* set sleep mode */
            can->MCR &= ~CAN_MCR_INRQ;
            can->MCR |= CAN_MCR_SLEEP;
            while (((can->MSR & CAN_MSR_INAK) || !(can->MSR & CAN_MSR_SLAK)) && max_loop != 0) {
                max_loop--;
            }
            break;
        case MODE_INIT:
            can->MCR &= ~CAN_MCR_SLEEP;
            can->MCR |= CAN_MCR_INRQ;
            /* wait for hardware confirmation */
            while ((!(can->MSR & CAN_MSR_INAK) || (can->MSR & CAN_MSR_SLAK)) && max_loop != 0) {
                max_loop--;
            }
            break;
        default:
            DEBUG("unsupported mode\n");
            res = -1;
            break;
    }

    if (max_loop == 0) {
        DEBUG("candev_stm32: didn't switch mode %d\n", mode);
        res = -1;
    }
    return res;
}

static inline int filter_is_set(CAN_TypeDef *master, uint8_t filter)
{
    return (master->FA1R & (1 << filter)) >> filter;
}

void can_init(can_t *dev, const can_conf_t *conf)
{
    dev->candev.driver = &candev_stm32_driver;

    struct can_bittiming timing = { .bitrate = CANDEV_STM32_DEFAULT_BITRATE,
                                    .sample_point = CANDEV_STM32_DEFAULT_SPT };
    can_device_calc_bittiming(CLOCK_APB1, &bittiming_const, &timing);

    memcpy(&dev->candev.bittiming, &timing, sizeof(timing));
    dev->conf = conf;

    dev->rx_pin = GPIO_UNDEF;
    dev->tx_pin = GPIO_UNDEF;
}

static void set_filter(CAN_TypeDef *can, uint32_t fr1, uint32_t fr2, uint8_t filter, uint8_t fifo)
{
    /* Set filter/mask */
    if ((fr1 & CAN_EFF_FLAG) == CAN_EFF_FLAG) {
        can->sFilterRegister[filter].FR1 = ((fr1 & CAN_EFF_MASK) << CAN_TIxR_EFF_SHIFT)
                                           | CAN_TI0R_IDE;
        can->sFilterRegister[filter].FR2 = ((fr2 & CAN_EFF_MASK) << CAN_TIxR_EFF_SHIFT)
                                           | CAN_TI0R_IDE;
    }
    else {
        can->sFilterRegister[filter].FR1 = (fr1 & CAN_SFF_MASK) << CAN_TIxR_SFF_SHIFT;
        can->sFilterRegister[filter].FR2 = (fr2 & CAN_SFF_MASK) << CAN_TIxR_SFF_SHIFT;
    }
    if (fifo == 0) {
        can->FFA1R &= ~(1 << filter); /* To FIFO 0 */
    }
    else {
        can->FFA1R |= (1 << filter);    /* To FIFO 1 */
    }
    can->FM1R &= ~(1 << filter);        /* Identifier Mask mode */
    can->FS1R |= (1 << filter);         /* Single 32-bit scale config */
    can->FA1R |= (1 << filter);         /* Activate filer */
}

static void get_can_filter(CAN_TypeDef *can, uint8_t filter_id, uint32_t *filter, uint32_t *mask)
{
    uint32_t fr1 = can->sFilterRegister[filter_id].FR1;
    uint32_t fr2 = can->sFilterRegister[filter_id].FR2;

    if ((fr1 & CAN_TI0R_IDE) == CAN_TI0R_IDE) {
        *filter = ((fr1 >> CAN_TIxR_EFF_SHIFT) & CAN_EFF_MASK) | CAN_EFF_FLAG;
        *mask = ((fr2 >> CAN_TIxR_EFF_SHIFT) & CAN_EFF_MASK) | CAN_EFF_FLAG;
    }
    else {
        *filter = (fr1 >> CAN_TIxR_SFF_SHIFT) & CAN_SFF_MASK;
        *mask = (fr2 >> CAN_TIxR_SFF_SHIFT) & CAN_SFF_MASK;
    }
    if (fr1 & CAN_TI0R_RTR) {
        *filter |= CAN_RTR_FLAG;
    }
    if (fr2 & CAN_TI0R_RTR) {
        *mask |= CAN_RTR_FLAG;
    }
}

static inline void unset_filter(CAN_TypeDef *can, uint8_t filter)
{
    can->FA1R &= ~(1 << filter);
}

#ifndef CPU_FAM_STM32F1
void candev_stm32_set_pins(can_t *dev, gpio_t tx_pin, gpio_t rx_pin,
                           gpio_af_t af)
#else
void candev_stm32_set_pins(can_t *dev, gpio_t tx_pin, gpio_t rx_pin)
#endif
{
    if (dev->tx_pin != GPIO_UNDEF) {
        gpio_init(dev->tx_pin, GPIO_IN);
        gpio_init_analog(dev->tx_pin);
    }
    if (dev->rx_pin != GPIO_UNDEF) {
        gpio_init(dev->rx_pin, GPIO_IN);
        gpio_init_analog(dev->rx_pin);
    }
    dev->tx_pin = tx_pin;
    dev->rx_pin = rx_pin;
    /* configure pins */
    gpio_init(rx_pin, GPIO_IN);
    gpio_init(tx_pin, GPIO_OUT);
#ifndef CPU_FAM_STM32F1
    dev->af = af;
    gpio_init_af(rx_pin, af);
    gpio_init_af(tx_pin, af);
#else
    gpio_init_af(tx_pin, GPIO_AF_OUT_PP);
#endif
}

static int _init(candev_t *candev)
{
    can_t *dev = container_of(candev, can_t, candev);
    int res = 0;

    _can[get_channel(dev->conf->can)] = dev;

    memset(dev->tx_mailbox, 0, sizeof(dev->tx_mailbox));
    memset(&dev->rx_fifo, 0, sizeof(dev->rx_fifo));

    dev->isr_flags.isr_tx = 0;
    dev->isr_flags.isr_rx = 0;

#if CANDEV_STM32_CHAN_NUMOF > 1
    /* Enable master clock */
    periph_clk_en(APB1, dev->conf->master_rcc_mask);
#endif
    /* Enable device clock */
    periph_clk_en(APB1, dev->conf->rcc_mask);

    _status[get_channel(dev->conf->can)] = STATUS_ON;

    /* configure pins */
#ifndef CPU_FAM_STM32F1
    candev_stm32_set_pins(dev, dev->conf->tx_pin, dev->conf->rx_pin, dev->conf->af);
#else
    candev_stm32_set_pins(dev, dev->conf->tx_pin, dev->conf->rx_pin);
#endif

    set_mode(dev->conf->can, MODE_INIT);

    /* Set configuration */
    uint32_t mask_config = (CAN_MCR_TTCM * dev->conf->ttcm) | (CAN_MCR_ABOM * dev->conf->abom) |
                           (CAN_MCR_AWUM * dev->conf->awum) | (CAN_MCR_NART * dev->conf->nart) |
                           (CAN_MCR_RFLM * dev->conf->rflm) | (CAN_MCR_TXFP * dev->conf->txfp);
    dev->conf->can->MCR |= mask_config;
    dev->conf->can->MCR &= (mask_config | CAN_MCR_BIT_DONT_TOUCH);

    set_bit_timing(dev);

    /* Loopback and/or silent mode */
    dev->conf->can->BTR |= (CAN_BTR_LBKM * dev->conf->lbkm) | (CAN_BTR_SILM * dev->conf->silm);

    /* Default filter config: No rx frame */
#if CANDEV_STM32_CHAN_NUMOF > 1
    if (dev->conf->can == dev->conf->can_master) {

        assert(dev->conf->first_filter == 0);
        assert(dev->conf->nb_filters <= CAN_STM32_NB_FILTER);
#endif

    dev->conf->can->FMR = CAN_FMR_FINIT;
#if CANDEV_STM32_CHAN_NUMOF > 1
    /* Clear start bank */
    dev->conf->can->FMR &= ~CAN_FMR_CAN2SB;
    /* Set start filter */
    dev->conf->can->FMR |= dev->conf->nb_filters << 8;

    for (int i = 0; i < dev->conf->nb_filters; i++) {
#else
    for (int i = 0; i < CAN_STM32_NB_FILTER; i++) {
#endif
        dev->conf->can->FA1R &= ~(1 << i);
    }
    dev->conf->can->FMR &= ~CAN_FMR_FINIT;
#if CANDEV_STM32_CHAN_NUMOF > 1
}
else {
    can_mode_t mode = get_mode(dev->conf->can_master);
    set_mode(dev->conf->can_master, MODE_INIT);
    dev->conf->can_master->FMR = CAN_FMR_FINIT;
    dev->conf->can_master->FMR &= ~CAN_FMR_CAN2SB;
    dev->conf->can_master->FMR |= dev->conf->first_filter << 8;
    for (int i = 0; i < dev->conf->nb_filters; i++) {
        dev->conf->can_master->FA1R &= ~(1 << (i + dev->conf->first_filter));
    }
    dev->conf->can_master->FMR &= ~CAN_FMR_FINIT;
    set_mode(dev->conf->can_master, mode);
}
#endif

    /* Clear interrupt flags */
    dev->conf->can->TSR |= CAN_TSR_RQCP0 | CAN_TSR_RQCP1 | CAN_TSR_RQCP2;
    dev->conf->can->RF0R |= CAN_RF0R_FMP0 | CAN_RF0R_FULL0 | CAN_RF0R_FOVR0;
    dev->conf->can->RF1R |= CAN_RF1R_FMP1 | CAN_RF1R_FULL1 | CAN_RF1R_FOVR1;
    dev->conf->can->ESR |= CAN_ESR_EWGF | CAN_ESR_EPVF | CAN_ESR_BOFF;
    dev->conf->can->MSR |= CAN_MSR_WKUI;

    dev->conf->can->IER = CAN_IER_WKUIE |
                          CAN_IER_EPVIE | CAN_IER_EWGIE | CAN_IER_ERRIE | CAN_IER_BOFIE |
                          CAN_IER_FOVIE1 | CAN_IER_FMPIE1 | CAN_IER_FOVIE0 | CAN_IER_FMPIE0 |
                          CAN_IER_TMEIE;
    /* Enable interrupts */
#if defined(CPU_FAM_STM32F0)
    NVIC_EnableIRQ(dev->conf->irqn);
#else
    NVIC_EnableIRQ(dev->conf->tx_irqn);
    NVIC_EnableIRQ(dev->conf->rx0_irqn);
    NVIC_EnableIRQ(dev->conf->rx1_irqn);
    NVIC_EnableIRQ(dev->conf->sce_irqn);
#endif

    res = set_mode(dev->conf->can, MODE_NORMAL);
#ifdef STM32_PM_STOP
    pm_block(STM32_PM_STOP);
#endif

    return res;
}

static inline void set_bit_timing(can_t *dev)
{
    /* Set bit timing */
    dev->conf->can->BTR = (((uint32_t)(dev->candev.bittiming.sjw - 1) << 24) & CAN_BTR_SJW) |
                          (((uint32_t)(dev->candev.bittiming.phase_seg2 - 1) << 20) & CAN_BTR_TS2) |
                          (((uint32_t)((dev->candev.bittiming.phase_seg1 +
                                        dev->candev.bittiming.prop_seg)
                                       - 1) << 16) & CAN_BTR_TS1) |
                          ((uint32_t)(dev->candev.bittiming.brp - 1) & CAN_BTR_BRP);
}

static int _send(candev_t *candev, const struct can_frame *frame)
{
    can_t *dev = container_of(candev, can_t, candev);
    CAN_TypeDef *can = dev->conf->can;
    int mailbox = 0;

    DEBUG("_send: candev=%p, frame=%p\n", (void *) candev, (void *) frame);

    for (mailbox = 0; mailbox < CAN_STM32_TX_MAILBOXES; mailbox++) {
        if (dev->tx_mailbox[mailbox] == NULL) {
            break;
        }
    }
    if (mailbox == CAN_STM32_TX_MAILBOXES) {
        return -EBUSY;
    }
    dev->tx_mailbox[mailbox] = frame;

    if ((frame->can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG) {
        can->sTxMailBox[mailbox].TIR = (frame->can_id & CAN_EFF_MASK) << CAN_TIxR_EFF_SHIFT
                                       | CAN_TI0R_IDE;
    }
    else {
        can->sTxMailBox[mailbox].TIR = (frame->can_id & CAN_SFF_MASK) << CAN_TIxR_SFF_SHIFT;
    }
    can->sTxMailBox[mailbox].TDTR = frame->can_dlc & CAN_TDT0R_DLC;

    can->sTxMailBox[mailbox].TDLR = 0;
    can->sTxMailBox[mailbox].TDHR = 0;
    for (int j = 0; j < 4 && frame->can_dlc > j; j++) {
        can->sTxMailBox[mailbox].TDLR |= (uint32_t)(frame->data[j] << (8 * j));
    }
    for (int j = 4; j < 8 && frame->can_dlc > j; j++) {
        can->sTxMailBox[mailbox].TDHR |= (uint32_t)(frame->data[j] << (8 * (j - 4)));
    }

    can->sTxMailBox[mailbox].TIR |= CAN_TI0R_TXRQ;

    return mailbox;
}

static int _abort(candev_t *candev, const struct can_frame *frame)
{
    can_t *dev = container_of(candev, can_t, candev);
    CAN_TypeDef *can = dev->conf->can;
    int mailbox = 0;

    for (mailbox = 0; mailbox < CAN_STM32_TX_MAILBOXES; mailbox++) {
        if (dev->tx_mailbox[mailbox] == frame) {
            break;
        }
    }
    if (mailbox == CAN_STM32_TX_MAILBOXES) {
        return -EOVERFLOW;
    }

    can->TSR |= CAN_TSR_ABRQ0 << (8 * mailbox);
    dev->tx_mailbox[mailbox] = NULL;

    return 0;
}

#define CAN_RIxR_SFF_SHIFT 21
#define CAN_RIxR_EFF_SHIFT 3
#define CAN_RDTxR_FMI_SHIFT 8

static int read_frame(can_t *dev, struct can_frame *frame, int mailbox)
{
    CAN_TypeDef *can = dev->conf->can;

    /* Get frame ID */
    if ((can->sFIFOMailBox[mailbox].RIR & CAN_RI0R_IDE) == CAN_RI0R_IDE) {
        frame->can_id = can->sFIFOMailBox[mailbox].RIR >> CAN_RIxR_EFF_SHIFT;
        frame->can_id |= CAN_EFF_FLAG;
    }
    else {
        frame->can_id = can->sFIFOMailBox[mailbox].RIR >> CAN_RIxR_SFF_SHIFT;
    }
    if ((can->sFIFOMailBox[mailbox].RIR & CAN_RI0R_RTR) == CAN_RI0R_RTR) {
        frame->can_id |= CAN_RTR_FLAG;
    }

    /* Get DLC */
    frame->can_dlc = can->sFIFOMailBox[mailbox].RDTR & CAN_RDT0R_DLC;

    /* Get Data */
    for (int j = 0; j < 4; j++) {
        frame->data[j] = (can->sFIFOMailBox[mailbox].RDLR >> (j * 8)) & 0xFF;
    }
    for (int j = 4; j < 8; j++) {
        frame->data[j] = (can->sFIFOMailBox[mailbox].RDHR >> ((j - 4) * 8)) & 0xFF;
    }

    /* filter number matching the received frame */
    /* filter = (can->sFIFOMailBox[mailbox].RDTR & CAN_RDT0R_FMI) >> CAN_RDTxR_FMI_SHIFT; */

    /* Release input mailbox */
    if (mailbox == 0) {
        can->RF0R |= CAN_RF0R_RFOM0;
    }
    else {
        can->RF1R |= CAN_RF1R_RFOM1;
    }

    return 0;
}

static void _isr(candev_t *candev)
{
    can_t *dev = container_of(candev, can_t, candev);

    if (dev->isr_flags.isr_tx) {
        tx_isr(dev);
    }

    if (dev->isr_flags.isr_wkup) {
        if (dev->candev.event_callback) {
            dev->candev.event_callback(&dev->candev, CANDEV_EVENT_WAKE_UP, NULL);
        }
    }

    unsigned int irq;

    irq = irq_disable();
    if (dev->isr_flags.isr_rx & 1) {
        dev->isr_flags.isr_rx &= ~1;
        irq_restore(irq);
        rx_isr(dev);
    }
    else {
        irq_restore(irq);
    }

    irq = irq_disable();
    if (dev->isr_flags.isr_rx & 2) {
        dev->isr_flags.isr_rx &= ~2;
        irq_restore(irq);
        rx_isr(dev);
    }
    else {
        irq_restore(irq);
    }
}

static inline int get_first_filter(can_t *dev)
{
#if CANDEV_STM32_CHAN_NUMOF == 1
    (void)dev;
    return 0;
#else
    return dev->conf->first_filter;
#endif
}

static inline int get_nb_filter(can_t *dev)
{
#if CANDEV_STM32_CHAN_NUMOF == 1
    (void)dev;
    return CAN_STM32_NB_FILTER;
#else
    return dev->conf->nb_filters;
#endif
}

static inline CAN_TypeDef *get_master(can_t *dev)
{
#if CANDEV_STM32_CHAN_NUMOF == 1
    return dev->conf->can;
#else
    return dev->conf->can_master;
#endif
}

static inline int is_master(can_t *dev)
{
#if CANDEV_STM32_CHAN_NUMOF == 1
    (void)dev;
    return 1;
#else
    return dev->conf->can_master == dev->conf->can;
#endif
}

static void _wkup_cb(void *arg)
{
    can_t *dev = arg;
    gpio_irq_disable(dev->rx_pin);

    DEBUG("int wkup: %p\n", arg);

    dev->isr_flags.isr_wkup = 1;

    if (dev->candev.event_callback) {
        dev->candev.event_callback(&dev->candev, CANDEV_EVENT_ISR, NULL);
    }
}

#if CANDEV_STM32_CHAN_NUMOF > 1
static void enable_int(can_t *dev, int master_from_slave)
{
    DEBUG("EN int (%d) (%p)\n", master_from_slave, (void *)dev);

    if (master_from_slave) {
        can_t *master = _can[get_channel(get_master(dev))];
        gpio_init_int(master->rx_pin, GPIO_IN, GPIO_FALLING, _wkup_cb, master);
    }
    else {
        gpio_init_int(dev->rx_pin, GPIO_IN, GPIO_FALLING, _wkup_cb, dev);
    }
}
#endif

static void disable_int(can_t *dev, int master_from_slave)
{
    DEBUG("DIS int (%d) (%p)\n", master_from_slave, (void *)dev);

    if (master_from_slave) {
#if CANDEV_STM32_CHAN_NUMOF > 1
        can_t *master = _can[get_channel(get_master(dev))];
        gpio_irq_disable(master->rx_pin);
#ifndef CPU_FAM_STM32F1
        candev_stm32_set_pins(master, master->tx_pin, master->rx_pin, master->af);
#else
        candev_stm32_set_pins(master, master->tx_pin, master->rx_pin);
#endif
#endif
    }
    else {
        gpio_irq_disable(dev->rx_pin);
#ifndef CPU_FAM_STM32F1
        candev_stm32_set_pins(dev, dev->tx_pin, dev->rx_pin, dev->af);
#else
        candev_stm32_set_pins(dev, dev->tx_pin, dev->rx_pin);
#endif
    }
}

static void turn_off(can_t *dev)
{
    DEBUG("turn off (%p)\n", (void *)dev);

    unsigned irq = irq_disable();
#if CANDEV_STM32_CHAN_NUMOF > 1
    if (is_master(dev)) {
        int chan = get_channel(dev->conf->can);
        if (chan < CANDEV_STM32_CHAN_NUMOF - 1 && _status[chan + 1] != STATUS_SLEEP) {
            /* a slave exists and is not sleeping */
            _status[chan] = STATUS_READY_FOR_SLEEP;
        }
        else {
            /* no slave or slave already sleeping */
            if (_status[get_channel(dev->conf->can)] != STATUS_SLEEP) {
#ifdef STM32_PM_STOP
                pm_unblock(STM32_PM_STOP);
#endif
            }
            _status[chan] = STATUS_SLEEP;
            if (dev->conf->en_deep_sleep_wake_up) {
                periph_clk_dis(APB1, dev->conf->rcc_mask);
                enable_int(dev, 0);
            }
        }
    }
    else {
        int master_chan = get_channel(get_master(dev));
        switch (_status[master_chan]) {
            case STATUS_READY_FOR_SLEEP:
                _status[master_chan] = STATUS_SLEEP;
#ifdef STM32_PM_STOP
                pm_unblock(STM32_PM_STOP);
#endif
            /* Fall through */
            case STATUS_NOT_USED:
                if (dev->conf->en_deep_sleep_wake_up) {
                    periph_clk_dis(APB1, dev->conf->master_rcc_mask);
                }
                break;
        }
        if (dev->conf->en_deep_sleep_wake_up) {
            periph_clk_dis(APB1, dev->conf->rcc_mask);
        }
        if (_status[get_channel(dev->conf->can)] != STATUS_SLEEP) {
#ifdef STM32_PM_STOP
            pm_unblock(STM32_PM_STOP);
#endif
        }
        _status[get_channel(dev->conf->can)] = STATUS_SLEEP;
        if (dev->conf->en_deep_sleep_wake_up) {
            if (_status[master_chan] == STATUS_SLEEP) {
                enable_int(dev, 1);
            }
            enable_int(dev, 0);
        }
    }
#else
    if (_status[get_channel(dev->conf->can)] != STATUS_SLEEP) {
#ifdef STM32_PM_STOP
        pm_unblock(STM32_PM_STOP);
#endif
    }
    _status[get_channel(dev->conf->can)] = STATUS_SLEEP;
    if (dev->conf->en_deep_sleep_wake_up) {
        periph_clk_dis(APB1, dev->conf->rcc_mask);
        gpio_init_int(dev->rx_pin, GPIO_IN, GPIO_FALLING, _wkup_cb, dev);
    }
#endif
    irq_restore(irq);
}

static void turn_on(can_t *dev)
{
    DEBUG("turn on (%p)\n", (void *)dev);

    unsigned irq = irq_disable();
#if CANDEV_STM32_CHAN_NUMOF > 1
    if (!is_master(dev)) {
        int master_chan = get_channel(get_master(dev));
        switch (_status[master_chan]) {
            case STATUS_SLEEP:
                _status[master_chan] = STATUS_READY_FOR_SLEEP;
                if (dev->conf->en_deep_sleep_wake_up) {
                    disable_int(dev, 1);
                }
#ifdef STM32_PM_STOP
                pm_block(STM32_PM_STOP);
#endif
            /* Fall through */
            case STATUS_NOT_USED:
                periph_clk_en(APB1, dev->conf->master_rcc_mask);
                break;
        }
    }
#endif
    if (_status[get_channel(dev->conf->can)] == STATUS_SLEEP) {
#ifdef STM32_PM_STOP
        pm_block(STM32_PM_STOP);
#endif
        if (dev->conf->en_deep_sleep_wake_up) {
            disable_int(dev, 0);
        }
        periph_clk_en(APB1, dev->conf->rcc_mask);
    }
    _status[get_channel(dev->conf->can)] = STATUS_ON;

    irq_restore(irq);
}

static int _wake_up(can_t *dev)
{
    turn_on(dev);
    return set_mode(dev->conf->can, MODE_NORMAL);
}

static int _sleep(can_t *dev)
{
    int res = set_mode(dev->conf->can, MODE_SLEEP);
    turn_off(dev);
    return res;
}

static int _set(candev_t *candev, canopt_t opt, void *value, size_t value_len)
{
    can_t *dev = container_of(candev, can_t, candev);
    CAN_TypeDef *can = dev->conf->can;
    int res = 0;
    can_mode_t mode;

    switch (opt) {
        case CANOPT_BITTIMING:
            if (value_len < sizeof(dev->candev.bittiming)) {
                res = -EOVERFLOW;
            }
            else {
                memcpy(&dev->candev.bittiming, value, sizeof(dev->candev.bittiming));
                mode = get_mode(can);
                if (mode == MODE_SLEEP) {
                    res = _wake_up(dev);
                    if (res != 0) {
                        res = -EBUSY;
                        break;
                    }
                }
                res = set_mode(can, MODE_INIT);
                if (res == 0) {
                    set_bit_timing(dev);
                    res = sizeof(dev->candev.bittiming);
                }
                if (mode == MODE_SLEEP) {
                    if (_sleep(dev) < 0) {
                        res = -EBUSY;
                    }
                }
                else if (set_mode(can, mode) < 0) {
                    res = -EBUSY;
                }
            }
            break;
        case CANOPT_STATE:
            if (value_len < sizeof(canopt_state_t)) {
                res = -EOVERFLOW;
            }
            else {
                switch (*((canopt_state_t *)value)) {
                    case CANOPT_STATE_OFF:
                    case CANOPT_STATE_SLEEP:
                        DEBUG("candev_stm32 %p: power down\n", (void *)dev);
                        res = _sleep(dev);
                        break;
                    case CANOPT_STATE_ON:
                        DEBUG("candev_stm32 %p: power up\n", (void *)dev);
                        res = _wake_up(dev);
                        break;
                    case CANOPT_STATE_LISTEN_ONLY:
                        mode = get_mode(can);
                        res = set_mode(can, MODE_INIT);
                        can->BTR |= CAN_BTR_SILM;
                        res += set_mode(can, mode);
                        break;
                    case CANOPT_STATE_LOOPBACK:
                        DEBUG("candev_stm32 %p: Loopback\n", (void *)dev);
                        res = set_mode(can, MODE_INIT);
                        can->BTR |= CAN_BTR_LBKM;
                        res += set_mode(can, MODE_NORMAL);
                        break;
                }
            }
            break;
        default:
            res = -ENOTSUP;
            break;
    }

    return res;
}

#define CAN_ESR_REC_SHIFT 24
#define CAN_ESR_TEC_SHIFT 16

static int _get(candev_t *candev, canopt_t opt, void *value, size_t max_len)
{
    can_t *dev = container_of(candev, can_t, candev);
    CAN_TypeDef *can = dev->conf->can;
    int res = 0;

    switch (opt) {
        case CANOPT_BITTIMING:
            if (max_len < sizeof(dev->candev.bittiming)) {
                res = -EOVERFLOW;
            }
            else {
                memcpy(value, &dev->candev.bittiming, sizeof(dev->candev.bittiming));
                res = sizeof(dev->candev.bittiming);
            }
            break;
        case CANOPT_BITTIMING_CONST:
            if (max_len < sizeof(bittiming_const)) {
                res = -EOVERFLOW;
            }
            else {
                memcpy(value, &bittiming_const, sizeof(bittiming_const));
                res = sizeof(bittiming_const);
            }
            break;
        case CANOPT_CLOCK:
            if (max_len < sizeof(uint32_t)) {
                res = -EOVERFLOW;
            }
            else {
                *((uint32_t *)value) = CLOCK_APB1;
                res = sizeof(uint32_t);
            }
            break;
        case CANOPT_RX_FILTERS:
            if (max_len % sizeof(struct can_filter) != 0) {
                res = -EOVERFLOW;
            }
            else {
                unsigned en_filt = 0;
                unsigned base_filter = get_first_filter(dev);
                unsigned last_filter = base_filter + get_nb_filter(dev);
                for (unsigned i = base_filter; i < last_filter; i++) {
                    if ((get_master(dev)->FA1R & (1u << i)) == (1u << i)) {
                        en_filt++;
                    }
                }
                if (max_len / sizeof(struct can_filter) < en_filt) {
                    res = -EOVERFLOW;
                }
                else {
                    for (unsigned i = 0; i < en_filt; i++) {
                        struct can_filter *filter = (struct can_filter *)value + i;
                        get_can_filter(get_master(
                                           dev), i + base_filter, &filter->can_id,
                                       &filter->can_mask);
                    }
                    res = en_filt * sizeof(struct can_filter);
                }
            }
            break;
        case CANOPT_TEC:
            if (max_len != sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                uint16_t *tec = (uint16_t *)value;
                *tec = (can->ESR & CAN_ESR_TEC) >> CAN_ESR_TEC_SHIFT;
                res = sizeof(uint16_t);
            }
            break;
        case CANOPT_REC:
            if (max_len != sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                uint16_t *rec = (uint16_t *)value;
                *rec = (can->ESR & CAN_ESR_REC) >> CAN_ESR_REC_SHIFT;
                res = sizeof(uint16_t);
            }
            break;
        default:
            res = -ENOTSUP;
            break;
    }

    return res;
}

static int _set_filter(candev_t *candev, const struct can_filter *filter)
{
    can_t *dev = container_of(candev, can_t, candev);

    DEBUG("_set_filter: dev=%p, filter=0x%" PRIx32 "\n", (void *)candev, filter->can_id);

    int first_filter = get_first_filter(dev);
    int last_filter = first_filter + get_nb_filter(dev);

    uint8_t i;
    for (i = first_filter; i < last_filter; i++) {
        if (!filter_is_set(get_master(dev), i)) {
            get_master(dev)->FMR = CAN_FMR_FINIT;
            set_filter(get_master(
                           dev), filter->can_id, filter->can_mask, i, i % CAN_STM32_RX_MAILBOXES);
            get_master(dev)->FMR &= ~CAN_FMR_FINIT;
            break;
        }
    }
    if (i == last_filter) {
        return -EOVERFLOW;
    }

    return i - first_filter;
}

static int _remove_filter(candev_t *candev, const struct can_filter *filter)
{
    can_t *dev = container_of(candev, can_t, candev);

    int first_filter = get_first_filter(dev);
    int last_filter = first_filter + get_nb_filter(dev);

    uint8_t i;

    for (i = first_filter; i < last_filter; i++) {
        if (filter_is_set(get_master(dev), i)) {
            uint32_t filt, mask;
            /* Clear RTR and ERR flags as they are ignored by the set/get filter */
            get_can_filter(get_master(dev), i, &filt, &mask);
            DEBUG("_remove_filter: filter=0x%" PRIx32 ",0x%" PRIx32 ", nb=%d, "
                  "dev_filter=0x%" PRIx32 ",0x%" PRIx32 "\n",
                  filter->can_id, filter->can_mask, (int)i, filt, mask);
            if ((filt == filter->can_id) && /* ID match */
                                            /* Filter match (extended case */
                (((filt & CAN_EFF_FLAG) &&
                  ((mask & CAN_EFF_MASK) == (filter->can_mask & CAN_EFF_MASK)))
                 /* Filter match (standard case */
                 || (!(filt & CAN_EFF_FLAG) &&
                     ((mask & CAN_SFF_MASK) == (filter->can_mask & CAN_SFF_MASK))))) {
                get_master(dev)->FMR = CAN_FMR_FINIT;
                unset_filter(get_master(dev), i);
                get_master(dev)->FMR &= ~CAN_FMR_FINIT;
                break;
            }
        }
    }
    if (i == last_filter) {
        return -EOVERFLOW;
    }

    return 0;
}

static void tx_conf(can_t *dev, int mailbox)
{
    candev_t *candev = (candev_t *) dev;
    const struct can_frame *frame = dev->tx_mailbox[mailbox];

    dev->tx_mailbox[mailbox] = NULL;

    DEBUG("_tx_conf: device=%p, mb=%d\n", (void *)dev, mailbox);

    if (frame && dev->candev.event_callback) {
        dev->candev.event_callback(candev, CANDEV_EVENT_TX_CONFIRMATION,
                                   (void *) frame);
    }
}

static void tx_irq_handler(can_t *dev)
{
    CAN_TypeDef *can = dev->conf->can;
    int flags = dev->isr_flags.isr_tx;

    DEBUG("tx irq\n");

    if (can->TSR & CAN_TSR_RQCP0) {
        if (can->TSR & CAN_TSR_TXOK0) {
            dev->isr_flags.isr_tx |= 1;
        }
        can->TSR |= CAN_TSR_RQCP0;
    }
    if (can->TSR & CAN_TSR_RQCP1) {
        if (can->TSR & CAN_TSR_TXOK1) {
            dev->isr_flags.isr_tx |= 2;
        }
        can->TSR |= CAN_TSR_RQCP1;
    }
    if (can->TSR & CAN_TSR_RQCP2) {
        if (can->TSR & CAN_TSR_TXOK2) {
            dev->isr_flags.isr_tx |= 4;
        }
        can->TSR |= CAN_TSR_RQCP2;
    }

    if (dev->candev.event_callback && flags != dev->isr_flags.isr_tx) {
        dev->candev.event_callback(&(dev->candev), CANDEV_EVENT_ISR, NULL);
    }
}

static void tx_isr(can_t *dev)
{
    unsigned int irq;

    irq = irq_disable();
    if (dev->isr_flags.isr_tx & 1) {
        dev->isr_flags.isr_tx &= ~1;
        irq_restore(irq);
        tx_conf(dev, 0);
    }
    else {
        irq_restore(irq);
    }

    irq = irq_disable();
    if (dev->isr_flags.isr_tx & 2) {
        dev->isr_flags.isr_tx &= ~2;
        irq_restore(irq);
        tx_conf(dev, 1);
    }
    else {
        irq_restore(irq);
    }

    irq = irq_disable();
    if (dev->isr_flags.isr_tx & 4) {
        dev->isr_flags.isr_tx &= ~4;
        irq_restore(irq);
        tx_conf(dev, 2);
    }
    else {
        irq_restore(irq);
    }
}

static void rx_irq_handler(can_t *dev, int mailbox)
{
    CAN_TypeDef *can = dev->conf->can;
    candev_t *candev = (candev_t *) dev;

    DEBUG("rx irq\n");
    if ((can->RF0R & CAN_RF0R_FOVR0) == CAN_RF0R_FOVR0) {
        can->RF0R |= CAN_RF0R_FOVR0;
        if (dev->candev.event_callback) {
            dev->candev.event_callback(candev, CANDEV_EVENT_RX_ERROR, NULL);
        }
        return;
    }
    if ((can->RF1R & CAN_RF1R_FOVR1) == CAN_RF1R_FOVR1) {
        can->RF1R |= CAN_RF1R_FOVR1;
        if (dev->candev.event_callback) {
            dev->candev.event_callback(candev, CANDEV_EVENT_RX_ERROR, NULL);
        }
        return;
    }

    if (!dev->rx_fifo.is_full) {
        int i = dev->rx_fifo.write_idx;
        read_frame(dev, &(dev->rx_fifo.frame[i]), mailbox);

        if (!dev->isr_flags.isr_rx) {
            dev->isr_flags.isr_rx |= mailbox + 1;
            if (dev->candev.event_callback) {
                dev->candev.event_callback(candev, CANDEV_EVENT_ISR, NULL);
            }
        }

        dev->rx_fifo.write_idx++;
        if (dev->rx_fifo.write_idx == CAN_STM32_RX_MAIL_FIFO) {
            dev->rx_fifo.write_idx = 0;
        }
        if (dev->rx_fifo.write_idx == dev->rx_fifo.read_idx) {
            dev->rx_fifo.is_full = 1;
        }
    }
    else {
        if (dev->candev.event_callback) {
            dev->candev.event_callback(candev, CANDEV_EVENT_RX_ERROR, NULL);
        }
    }
}

static void rx_isr(can_t *dev)
{
    DEBUG("_rx_isr: device=%p\n", (void *)dev);

    while (dev->rx_fifo.is_full || dev->rx_fifo.read_idx != dev->rx_fifo.write_idx) {
        int i = dev->rx_fifo.read_idx;

        if (dev->candev.event_callback) {
            dev->candev.event_callback(&dev->candev,
                                       CANDEV_EVENT_RX_INDICATION,
                                       &dev->rx_fifo.frame[i]);
        }

        dev->rx_fifo.read_idx++;
        if (dev->rx_fifo.read_idx == CAN_STM32_RX_MAIL_FIFO) {
            dev->rx_fifo.read_idx = 0;
        }

        dev->rx_fifo.is_full = 0;
    }
}

static void sce_irq_handler(can_t *dev)
{
    CAN_TypeDef *can = dev->conf->can;
    candev_t *candev = (candev_t *) dev;

    DEBUG("sce irq: ");

    if ((can->MSR & CAN_MSR_ERRI) == CAN_MSR_ERRI) {
        can->MSR = CAN_MSR_ERRI;
        if ((can->ESR & CAN_ESR_BOFF) == CAN_ESR_BOFF) {
            DEBUG("bus-off\n");
            if (!dev->conf->abom) {
                set_mode(can, MODE_INIT);
            }
            if (dev->candev.event_callback) {
                dev->candev.event_callback(candev, CANDEV_EVENT_BUS_OFF, NULL);
            }
        }
        else if ((can->ESR & CAN_ESR_EPVF) == CAN_ESR_EPVF) {
            DEBUG("error passive\n");
            if (dev->candev.event_callback) {
                dev->candev.event_callback(candev, CANDEV_EVENT_ERROR_PASSIVE, NULL);
            }
        }
        else if ((can->ESR & CAN_ESR_EWGF) == CAN_ESR_EWGF) {
            DEBUG("error warning\n");
            if (dev->candev.event_callback) {
                dev->candev.event_callback(candev, CANDEV_EVENT_ERROR_WARNING, NULL);
            }
        }
    }
    else if ((can->MSR & CAN_MSR_WKUI) == CAN_MSR_WKUI) {
        can->MSR = CAN_MSR_WKUI;
        DEBUG("wakeup\n");
        if (dev->candev.event_callback) {
            dev->candev.event_callback(candev, CANDEV_EVENT_WAKE_UP, NULL);
        }
    }
}

#if defined(CPU_FAM_STM32F0)
#define CAN_MSR_INT_MASK  0x0000001C
#define CAN_TSR_INT_MASK  0x000F0F0F
#define CAN_RFxR_INT_MASK 0x0000001B
#define CAN_ESR_INT_MASK  0x00000077

void isr_cec_can(void)
{
    DEBUG("bxCAN irq\n");

    if ((CAN->ESR & CAN_ESR_INT_MASK) || (CAN->MSR & CAN_MSR_INT_MASK)) {
        sce_irq_handler(_can[0]);
    }
    if (CAN->TSR & CAN_TSR_INT_MASK) {
        tx_irq_handler(_can[0]);
    }
    if (CAN->RF0R & CAN_RFxR_INT_MASK) {
        rx_irq_handler(_can[0], 0);
    }
    if (CAN->RF1R & CAN_RFxR_INT_MASK) {
        rx_irq_handler(_can[0], 1);
    }

    cortexm_isr_end();
}
#else
void ISR_CAN1_TX(void)
{
    tx_irq_handler(_can[0]);

    cortexm_isr_end();
}

void ISR_CAN1_RX0(void)
{
    rx_irq_handler(_can[0], 0);

    cortexm_isr_end();
}

void ISR_CAN1_RX1(void)
{
    rx_irq_handler(_can[0], 1);

    cortexm_isr_end();
}

void ISR_CAN1_SCE(void)
{
    sce_irq_handler(_can[0]);

    cortexm_isr_end();
}

#if CANDEV_STM32_CHAN_NUMOF > 1
void ISR_CAN2_TX(void)
{
    tx_irq_handler(_can[1]);

    cortexm_isr_end();
}

void ISR_CAN2_RX0(void)
{
    rx_irq_handler(_can[1], 0);

    cortexm_isr_end();
}

void ISR_CAN2_RX1(void)
{
    rx_irq_handler(_can[1], 1);

    cortexm_isr_end();
}

void ISR_CAN2_SCE(void)
{
    sce_irq_handler(_can[1]);

    cortexm_isr_end();
}
#endif

#if CANDEV_STM32_CHAN_NUMOF > 2
void ISR_CAN3_TX(void)
{
    tx_irq_handler(_can[2]);

    cortexm_isr_end();
}

void ISR_CAN3_RX0(void)
{
    rx_irq_handler(_can[2], 0);

    cortexm_isr_end();
}

void ISR_CAN3_RX1(void)
{
    rx_irq_handler(_can[2], 1);

    cortexm_isr_end();
}

void ISR_CAN3_SCE(void)
{
    sce_irq_handler(_can[2]);

    cortexm_isr_end();
}
#endif
#endif
