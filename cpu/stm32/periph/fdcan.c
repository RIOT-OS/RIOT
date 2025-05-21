/*
 * Copyright (C) 2024 COGIP Robotics association
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of the CAN FD controller driver
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 * @}
 */

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <cpu_conf.h>

#include "can/device.h"
#include "periph/can.h"
#include "periph/gpio.h"
#include "pm_layered.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define CAN_MAX_WAIT_CHANGE (10000U)

typedef enum {
    MODE_NORMAL,
    MODE_SLEEP,
    MODE_INIT,
} can_mode_t;

/* Driver functions */
static int _init(candev_t *candev);
static void _isr(candev_t *candev);
static int _send(candev_t *candev, const can_frame_t *frame);
static int _abort(candev_t *candev, const can_frame_t *frame);
static int _set(candev_t *candev, canopt_t opt, void *value, size_t value_len);
static int _get(candev_t *candev, canopt_t opt, void *value, size_t max_len);
static int _set_filter(candev_t *candev, const struct can_filter *filter);
static int _remove_filter(candev_t *candev, const struct can_filter *filter);

/* Interrupts handler */
static void tx_irq_handler(can_t *dev);
static void tx_isr(can_t *dev);
static void tx_conf(can_t *dev, int mailbox);
static void rx_irq_handler(can_t *dev);
static void rx_new_message_irq_handler(can_t *dev, uint8_t message_ram_rx_fifo);
static void rx_isr(can_t *dev);

/* Bittiming configuration */
static inline void set_bit_timing(can_t *dev);

static inline can_mode_t get_mode(FDCAN_GlobalTypeDef *can);
static int set_mode(FDCAN_GlobalTypeDef *can, can_mode_t mode);

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

/* Classic CAN bittiming */
static const struct can_bittiming_const bittiming_const = {
    .tseg1_min = 1,
    .tseg1_max = 256,
    .tseg2_min = 1,
    .tseg2_max = 128,
    .sjw_max = 1,
    .brp_min = 1,
    .brp_max = 1024,
    .brp_inc = 1,
};

/* FDCAN data bittiming */
static const struct can_bittiming_const fd_data_bittiming_const = {
    .tseg1_min = 1,
    .tseg1_max = 16,
    .tseg2_min = 1,
    .tseg2_max = 8,
    .sjw_max = 1,
    .brp_min = 1,
    .brp_max = 32,
    .brp_inc = 1,
};

enum {
    STATUS_ON,
    STATUS_SLEEP,
};

/* FDCAN channels */
static can_t *_can[FDCANDEV_STM32_CHAN_NUMOF];
/* FDCAN channels status */
static uint8_t _status[FDCANDEV_STM32_CHAN_NUMOF];

/* Return channel for a given FDCAN device */
static inline uint8_t get_channel(FDCAN_GlobalTypeDef *can)
{
    return (int) (((uint32_t)can - (uint32_t)FDCAN1) >> 10);
}

/* Return FDCAN STM32 datasheet channel name */
static inline uint8_t get_channel_id(FDCAN_GlobalTypeDef *can)
{
    return get_channel(can) + 1;
}

/* Get current FDCAN channel mode */
static inline can_mode_t get_mode(FDCAN_GlobalTypeDef *can)
{
    if ((can->CCCR & FDCAN_CCCR_CSR) == FDCAN_CCCR_CSR) {
        DEBUG("%s: Current FDCAN%u mode: SLEEP\n", __func__, get_channel_id(can));
        return MODE_SLEEP;
    }
    else if ((can->CCCR & FDCAN_CCCR_INIT) == FDCAN_CCCR_INIT) {
        DEBUG("%s: Current FDCAN%u mode: INIT\n", __func__, get_channel_id(can));
        return MODE_INIT;
    }
    else {
        DEBUG("%s: Current FDCAN%u mode: NORMAL\n", __func__, get_channel_id(can));
        return MODE_NORMAL;
    }
}

/* Change FDCAN channel mode */
static int set_mode(FDCAN_GlobalTypeDef *can, can_mode_t mode)
{
    int max_loop = CAN_MAX_WAIT_CHANGE;
    int res = 0;

    switch (mode) {
        case MODE_NORMAL:
            DEBUG("%s: Set FDCAN%u NORMAL mode\n", __func__, get_channel_id(can));
            /* Disable sleep mode */
            can->CCCR &= ~FDCAN_CCCR_CSR;
            while ((can->CCCR & FDCAN_CCCR_CSA) && max_loop != 0) {
                max_loop--;
            }
            if (max_loop) {
                max_loop = CAN_MAX_WAIT_CHANGE;
                DEBUG("%s: FDCAN%u FDCAN_CCCR_CSA reset\n", __func__, get_channel_id(can));
            }
            can->CCCR &= ~FDCAN_CCCR_INIT;
            /* CCE (Configuration Change Enable) bit is automatically
               cleared when INIT bit is cleared. So to ensure the FDCAN
               channel is no more in INIT mode, wait for CCE and INIT bits
               to be cleared. */
            while ((can->CCCR & FDCAN_CCCR_CCE) && max_loop != 0) {
                max_loop--;
            }
            if (max_loop) {
                max_loop = CAN_MAX_WAIT_CHANGE;
                DEBUG("%s: FDCAN%u FDCAN_CCCR_CCE reset\n", __func__, get_channel_id(can));
            }
            while ((can->CCCR & FDCAN_CCCR_INIT) && max_loop != 0) {
                max_loop--;
            }
            if (max_loop) {
                max_loop = CAN_MAX_WAIT_CHANGE;
                DEBUG("%s: FDCAN%u_CCCR_INIT reset\n", __func__, get_channel_id(can));
            }
            break;
        case MODE_SLEEP:
            DEBUG("%s: Set FDCAN%u SLEEP mode\n", __func__, get_channel_id(can));
            /* Enable sleep mode */
            can->CCCR |= FDCAN_CCCR_CSR;
            while (!(can->CCCR & FDCAN_CCCR_CSA) && max_loop != 0) {
                max_loop--;
            }
            break;
        case MODE_INIT:
            DEBUG("%s: Set FDCAN%u INIT mode\n", __func__, get_channel_id(can));
            /* Disable sleep mode */
            can->CCCR &= ~FDCAN_CCCR_CSR;
            while ((can->CCCR & FDCAN_CCCR_CSA) && max_loop != 0) {
                max_loop--;
            }
            if (max_loop) {
                max_loop = CAN_MAX_WAIT_CHANGE;
            }
            /* Enable INIT mode */
            can->CCCR |= FDCAN_CCCR_INIT;
            while (!(can->CCCR & FDCAN_CCCR_INIT) && max_loop != 0) {
                max_loop--;
            }
            /* Enable Configuration changes */
            can->CCCR |= FDCAN_CCCR_CCE;
            break;
        default:
            DEBUG("%s: FDCAN%u Unsupported mode\n", __func__, get_channel_id(can));
            res = -EINVAL;
            break;
    }

    DEBUG("%s: FDCAN%u CCCR = %lx\n", __func__, get_channel_id(can), can->CCCR);

    if (max_loop == 0) {
        DEBUG("%s: FDCAN%u didn't switch mode %d\n", __func__, get_channel_id(can), mode);
        res = -ETIMEDOUT;
    }

    return res;
}

void can_init(can_t *dev, const can_conf_t *conf)
{
    /* Set CAN device callbacks */
    dev->candev.driver = &candev_stm32_driver;

    /* Use the PCLK (APB1 peripheral) clock by default as this clock is always available */
    RCC->CCIPR &= ~RCC_CCIPR_FDCANSEL;
    RCC->CCIPR |= RCC_CCIPR_FDCANSEL_1;

    /* Compute bittiming values for classic CAN */
    struct can_bittiming timing = { .bitrate = FDCANDEV_STM32_DEFAULT_BITRATE,
                                    .sample_point = FDCANDEV_STM32_DEFAULT_SPT };
    can_device_calc_bittiming(CLOCK_APB1, &bittiming_const, &timing);
    timing.tq = (timing.brp * NS_PER_SEC) / CLOCK_APB1;

    /* Compute bittiming values for FDCAN data stream */
    struct can_bittiming fd_data_timing = { .bitrate = FDCANDEV_STM32_DEFAULT_FD_DATA_BITRATE,
                                    .sample_point = FDCANDEV_STM32_DEFAULT_SPT };
    can_device_calc_bittiming(CLOCK_APB1, &fd_data_bittiming_const, &fd_data_timing);
    fd_data_timing.tq = (fd_data_timing.brp * NS_PER_SEC) / CLOCK_APB1;

    /* Save the calculated bittimings */
    memcpy(&dev->candev.bittiming, &timing, sizeof(timing));
    memcpy(&dev->candev.fd_data_bittiming, &fd_data_timing, sizeof(fd_data_timing));
    DEBUG("%s: &dev->rx_mailbox = %p, sizeof(dev->rx_mailbox) = %u\n",
          __func__, &dev->rx_mailbox, sizeof(dev->rx_mailbox));

    /* Save configuration */
    dev->conf = conf;

    /* Reset rx_pin/tx_pin */
    dev->rx_pin = GPIO_UNDEF;
    dev->tx_pin = GPIO_UNDEF;
}

/* Get FDCAN message RAM address */
static uint32_t* get_message_ram(FDCAN_GlobalTypeDef *can) {
    /* In case of multiple instances the RAM start address for the FDCANn is computed by end
       of FDCANn-1 address + 4, and the FDCANn end address is computed by FDCANn start
       address + 0x0350 - 4. */
    DEBUG("%s: FDCAN%u message RAM address is %p\n",
          __func__, get_channel_id(can),
          (uint32_t *)(SRAMCAN_BASE
          + (((uint32_t)can - FDCAN1_BASE) >> 10) * FDCAN_SRAM_MESSAGE_RAM_SIZE)
          );

    return (uint32_t*)(
        SRAMCAN_BASE
        + (((uint32_t)can - FDCAN1_BASE) >> 10) * FDCAN_SRAM_MESSAGE_RAM_SIZE
        );
}

/* Get FDCAN channel extended filters list message RAM address */
static uint32_t* get_flesa_message_ram(FDCAN_GlobalTypeDef *can) {
    DEBUG("%s: FDCAN%u FLESA message RAM address is %p\n",
          __func__, get_channel_id(can),
          (void*)get_message_ram(can) + FDCAN_SRAM_FLESA
          );

    return get_message_ram(can) + FDCAN_SRAM_FLESA;
}

/* Get FDCAN channel Rx FIFO message RAM address */
static uint32_t* get_message_ram_rx_fifo_address(FDCAN_GlobalTypeDef *can,
                                                 uint8_t message_ram_rx_fifo) {
    DEBUG("%s: FDCAN%u Rx FIFO message RAM address is %p\n",
          __func__, get_channel_id(can),
          get_message_ram(can) + FDCAN_SRAM_F0SA + message_ram_rx_fifo * FDCAN_SRAM_RXFIFO_SIZE
          );

    return get_message_ram(can) + FDCAN_SRAM_F0SA + message_ram_rx_fifo * FDCAN_SRAM_RXFIFO_SIZE;
}

/* Get FDCAN channel Tx buffers message RAM address */
static uint32_t* get_message_ram_tx_buffer_address(FDCAN_GlobalTypeDef *can) {
    DEBUG("%s: FDCAN%u Tx FIFO message RAM address is %p\n",
          __func__, get_channel_id(can),
          get_message_ram(can) + FDCAN_SRAM_TBSA
          );

    return get_message_ram(can) + FDCAN_SRAM_TBSA;
}

/** Check if filter is set according to filter ID
 *  filter ID < FDCAN_STM32_NB_STD_FILTER: it is a standard filter (message ID is 11 bits long)
 *  filter ID >= FDCAN_STM32_NB_STD_FILTER: it is an extended filter (message ID is 29 bits long)
 */
static int filter_is_set(FDCAN_GlobalTypeDef *can, uint8_t filter_id)
{
    int ret = false;

    if (filter_id < FDCAN_STM32_NB_STD_FILTER) {
        DEBUG("%s: FDCAN%u filter %u is a standard filter\n",
              __func__, get_channel_id(can), filter_id);
        if (filter_id < (can->RXGFC & FDCAN_RXGFC_LSS) >> FDCAN_RXGFC_LSS_Pos) {
            DEBUG("%s: Filter %u is lesser than standard filter list size (%lx)\n",
                  __func__,
                  filter_id,
                  (can->RXGFC & FDCAN_RXGFC_LSS) >> FDCAN_RXGFC_LSS_Pos);

            /* Filter List Standard start at
               RAM address + FLSSA (0x00) + standard filter offset (4 Bytes * filter_id) */
            uint32_t *fls_ram_address = get_message_ram(can)
                                        + FDCAN_SRAM_FLS_FILTER_SIZE * filter_id;

            DEBUG("%s: FDCAN%u ftandard filter message RAM address is %p\n",
                  __func__, get_channel_id(can),
                  fls_ram_address
                  );

            if (((*fls_ram_address & FDCAN_SRAM_FLS_SFT) != FDCAN_SRAM_FLS_SFT_DISABLED)
                || ((*fls_ram_address & FDCAN_SRAM_FLS_SFEC) != FDCAN_SRAM_FLS_SFEC_DISABLED)) {
                DEBUG("%s: FDCAN%u filter %u is enabled by SFT(%lx) and SFEC(%lx)\n",
                  __func__, get_channel_id(can),
                  filter_id,
                  (*fls_ram_address & FDCAN_SRAM_FLS_SFT),
                  (*fls_ram_address & FDCAN_SRAM_FLS_SFEC));

                ret = true;
            }
            else {
                DEBUG("%s: FDCAN%u filter %u is disabled by SFT(%lx) and/or SFEC(%lx)\n",
                  __func__, get_channel_id(can),
                  filter_id,
                  (*fls_ram_address & FDCAN_SRAM_FLS_SFT),
                  (*fls_ram_address & FDCAN_SRAM_FLS_SFEC));
            }
        }
    }
    else {
        DEBUG("%s: Filter %u is an extended filter\n", __func__, filter_id);
        /* Real extended filter index */
        filter_id -= FDCAN_STM32_NB_STD_FILTER;
        if (filter_id < (can->RXGFC & FDCAN_RXGFC_LSE) >> FDCAN_RXGFC_LSE_Pos) {
            DEBUG("%s: FDCAN%u filter %u is lesser than extended filter list size (%lx)\n",
                  __func__, get_channel_id(can),
                  filter_id,
                  (can->RXGFC & FDCAN_RXGFC_LSE) >> FDCAN_RXGFC_LSE_Pos);

            /* Filter List Extended start at
               RAM address + FLESA (0x70) + extended filter offset (8 Bytes * filter_id) */
            uint32_t *fle_ram_address_f0 = get_flesa_message_ram(can)
                                           + FDCAN_SRAM_FLE_FILTER_SIZE * filter_id;
            uint32_t *fle_ram_address_f1 = fle_ram_address_f0
                                           + FDCAN_SRAM_FLE_FILTER_SIZE / 2;

            DEBUG("%s: FDCAN%u extended filter message RAM F0 address is %p\n",
                  __func__, get_channel_id(can),
                  fle_ram_address_f0
                  );
            DEBUG("%s: FDCAN%u extended filter message RAM F1 address is %p\n",
                  __func__, get_channel_id(can),
                  fle_ram_address_f1
                  );

            if ((*fle_ram_address_f0 & FDCAN_SRAM_FLE_F0_EFEC) != FDCAN_SRAM_FLE_F0_EFEC_DISABLED) {

                DEBUG("%s: FDCAN%u filter %u is enabled by EFEC(%lx)\n",
                  __func__, get_channel_id(can),
                  filter_id,
                  (*fle_ram_address_f0 & FDCAN_SRAM_FLE_F0_EFEC));
                ret = true;
            }
            else {
                DEBUG("%s: FDCAN%u filter %u is disabled by EFEC(%lx)\n",
                  __func__, get_channel_id(can),
                  filter_id,
                  (*fle_ram_address_f0 & FDCAN_SRAM_FLE_F0_EFEC));
            }
        }
    }

    return ret;
}

/* Set standard filter for 11 bits message ID */
static int set_standard_filter(FDCAN_GlobalTypeDef *can, uint32_t fr1, uint32_t fr2,
                               uint8_t filter_id, uint8_t fifo)
{
    /* Check filter ID */
    if (filter_id >= FDCAN_STM32_NB_STD_FILTER) {
        DEBUG("ERROR: invalid standard filter ID %u\n", filter_id);
        return -ENXIO;
    }

    /* Filter List Standard start at
       RAM address + FLSSA (0x00) + standard filter offset (4 Bytes * filter_id) */
    uint32_t *fls_ram_address = get_message_ram(can) + FDCAN_SRAM_FLS_FILTER_SIZE * filter_id;
    DEBUG("%s: FDCAN%u standard filter message RAM address is %p\n",
          __func__, get_channel_id(can),
          fls_ram_address
          );

    DEBUG("%s: FDCAN%u standard filter %u value is %lx\n",
          __func__, get_channel_id(can),
          filter_id,
          *fls_ram_address
          );

    /* Set filter/mask */
    if ((fr1 & ~CAN_SFF_MASK) || (fr2 & ~CAN_SFF_MASK)) {
        DEBUG("%s: FDCAN%u standard filter %u ID(%lx) or mask(%lx) are not valid\n",
            __func__, get_channel_id(can),
            filter_id,
            fr1,
            fr2);

            return -EINVAL;
    }

    /* Set filter mask and ID */
    *fls_ram_address &= ~FDCAN_SRAM_FLS_SFID1;
    *fls_ram_address |= fr1 << FDCAN_SRAM_FLS_SFID1_Pos;

    *fls_ram_address &= ~FDCAN_SRAM_FLS_SFID2;
    *fls_ram_address |= fr2;

    /* Set filter type */
    *fls_ram_address &= ~FDCAN_SRAM_FLS_SFT;
    *fls_ram_address |= FDCAN_SRAM_FLS_SFT_CLASSIC;

    /* Filter FIFO configuration */
    *fls_ram_address &= ~FDCAN_SRAM_FLS_SFEC;
    *fls_ram_address |= (fifo ? FDCAN_SRAM_FLS_SFEC_FIFO1 : FDCAN_SRAM_FLS_SFEC_FIFO0);

    DEBUG("%s: FDCAN%u standard filter %u value is %lx\n",
          __func__, get_channel_id(can),
          filter_id,
          *fls_ram_address
          );

    /* Set LSS (List Size of Standard filters) according to new filter ID */
    if (filter_id >= (can->RXGFC & FDCAN_RXGFC_LSS) >> FDCAN_RXGFC_LSS_Pos) {
        can->RXGFC &= ~FDCAN_RXGFC_LSS;
        can->RXGFC |= ((filter_id) + 1) << FDCAN_RXGFC_LSS_Pos;

        DEBUG("%s: FDCAN%u new LSS value is %u\n",
              __func__, get_channel_id(can), filter_id + 1);
    }

    DEBUG("%s: FDCAN%u new RXGFC value is %lx\n",
          __func__, get_channel_id(can), can->RXGFC);

    return 0;
}

/* Set extended filter for 29 bits message ID */
static int set_extended_filter(FDCAN_GlobalTypeDef *can, uint32_t fr1, uint32_t fr2,
                               uint8_t filter_id, uint8_t fifo)
{
    /* Check filter ID */
    if (filter_id >= FDCAN_STM32_NB_EXT_FILTER) {
        DEBUG("ERROR: invalid extended filter ID %u\n", filter_id);
        return -ENXIO;
    }

    /* Filter List Extended start at
       RAM address + FLESA (0x70) + extended filter offset (8 Bytes * filter_id) */
    uint32_t *fle_ram_address_f0 = get_flesa_message_ram(can)
                                   + FDCAN_SRAM_FLE_FILTER_SIZE * filter_id;
    uint32_t *fle_ram_address_f1 = fle_ram_address_f0 + FDCAN_SRAM_FLE_FILTER_SIZE / 2;

    /* Reset filter */
    *fle_ram_address_f0 = 0;
    *fle_ram_address_f1 = 0;

    *fle_ram_address_f0 |= (fr1 & CAN_EFF_MASK)                     /* Filter ID */
                           | (fifo ? FDCAN_SRAM_FLE_F0_EFEC_FIFO1
                              : FDCAN_SRAM_FLE_F0_EFEC_FIFO0);      /* Filter FIFO configuration */
    *fle_ram_address_f1 |= (fr2 & CAN_EFF_MASK)                     /* Filter mask */
                           | FDCAN_SRAM_FLE_F1_EFT_CLASSIC;         /* Set classic filter type */

    DEBUG("%s: FDCAN%u extended filter %u F0 value is %lx\n",
          __func__, get_channel_id(can),
          filter_id,
          *fle_ram_address_f0
          );
    DEBUG("%s: FDCAN%u extended filter %u F1 value is %lx\n",
          __func__, get_channel_id(can),
          filter_id,
          *fle_ram_address_f1
          );

    /* Set LSS (List Size of Extended filters) according to new filter ID */
    if (filter_id >= (can->RXGFC & FDCAN_RXGFC_LSE) >> FDCAN_RXGFC_LSE_Pos) {
        can->RXGFC &= ~FDCAN_RXGFC_LSE;
        can->RXGFC |= (filter_id + 1) << FDCAN_RXGFC_LSE_Pos;

        DEBUG("%s: FDCAN%u new LSE value is %u\n",
              __func__, get_channel_id(can), filter_id + 1);
    }

    DEBUG("%s: FDCAN%u new RXGFC value is %lx\n",
          __func__, get_channel_id(can), can->RXGFC);

    return 0;
}

/** Select set function for standard or extended filter.
 *  Extended filters can filter standard messages
 */
static int set_filter(FDCAN_GlobalTypeDef *can, uint32_t fr1, uint32_t fr2,
                      uint8_t filter_id, uint8_t fifo)
{
    /* Check ID and mask, to use an extended filter if needed */
    if (filter_id < FDCAN_STM32_NB_STD_FILTER) {
        return set_standard_filter(can, fr1, fr2, filter_id, fifo);
    }
    else {
        return set_extended_filter(can, fr1, fr2, filter_id - FDCAN_STM32_NB_STD_FILTER, fifo);
    }
}

static int get_can_filter(FDCAN_GlobalTypeDef *can, uint8_t filter_id,
                          uint32_t *filter, uint32_t *mask)
{
    if (filter_id < FDCAN_STM32_NB_STD_FILTER) {
        /* Filter List Standard start at
           RAM address + FLSSA (0x00) + standard filter offset (4 Bytes * filter_id) */
        uint32_t *fls_ram_address = get_message_ram(can) + FDCAN_SRAM_FLS_FILTER_SIZE * filter_id;
        *filter = (*fls_ram_address & FDCAN_SRAM_FLS_SFID1) >> FDCAN_SRAM_FLS_SFID1_Pos;
        *mask = (*fls_ram_address & FDCAN_SRAM_FLS_SFID2);
    }
    else {
        filter_id -= FDCAN_STM32_NB_STD_FILTER;
        /* Filter List Extended start at
           RAM address + FLESA (0x70) + extended filter offset (8 Bytes * filter_id) */
        uint32_t *fle_ram_address_f0 = get_flesa_message_ram(can)
                                       + FDCAN_SRAM_FLE_FILTER_SIZE * filter_id;
        uint32_t *fle_ram_address_f1 = fle_ram_address_f0 + FDCAN_SRAM_FLE_FILTER_SIZE / 2;
        *filter = (*fle_ram_address_f0 & FDCAN_SRAM_FLE_F0_EFID1);
        *mask = (*fle_ram_address_f1 & FDCAN_SRAM_FLE_F1_EFID2);
    }

    return 0;
}

static void unset_standard_filter(FDCAN_GlobalTypeDef *can, uint8_t filter_id)
{
    /* Filter List Standard start at
       RAM address + FLSSA (0x00) + standard filter offset (4 Bytes * filter_id) */
    uint32_t *fls_ram_address = get_message_ram(can) + FDCAN_SRAM_FLS_FILTER_SIZE * filter_id;
    /* Disable filter */
    *fls_ram_address &= ~FDCAN_SRAM_FLS_SFEC;

    DEBUG("%s: FDCAN%u standard filter %u value is %lx\n",
          __func__, get_channel_id(can),
          filter_id,
          *fls_ram_address
          );

    /* Update LSS value if necessary */
    uint8_t lss_value = (can->RXGFC & FDCAN_RXGFC_LSS) >> FDCAN_RXGFC_LSS_Pos;
    can->RXGFC &= ~FDCAN_RXGFC_LSS;
    can->RXGFC |= lss_value > 0 ? lss_value-- : 0;

    DEBUG("%s: FDCAN%u new LSS value is %u\n",
          __func__, get_channel_id(can), filter_id);
}

static void unset_extended_filter(FDCAN_GlobalTypeDef *can, uint8_t filter_id)
{
    /* Filter List Extended start at
       RAM address + FLESA (0x70) + extended filter offset (8 Bytes * filter_id) */
    uint32_t *fle_ram_address_f0 = get_flesa_message_ram(can)
                                   + FDCAN_SRAM_FLE_FILTER_SIZE * filter_id;
    /* Disable filter */
    *fle_ram_address_f0 &= ~FDCAN_SRAM_FLE_F0_EFEC;

    DEBUG("%s: FDCAN%u extended filter %u F0 value is %lx\n",
          __func__, get_channel_id(can),
          filter_id,
          *fle_ram_address_f0
          );

    /* Update LSE value */
    uint8_t lse_value = (can->RXGFC & FDCAN_RXGFC_LSE) >> FDCAN_RXGFC_LSE_Pos;
    can->RXGFC &= ~FDCAN_RXGFC_LSE;
    can->RXGFC |= lse_value > 0 ? lse_value-- : 0;

    DEBUG("%s: FDCAN%u new LSE value is %u\n",
          __func__, get_channel_id(can), filter_id);
}

/**
 *  Select unset function for standard or extended filter.
 *  Extended filters can filter standard messages
 */
static void unset_filter(FDCAN_GlobalTypeDef *can, uint8_t filter_id)
{
    /* Check ID and mask, to use an extended filter if needed */
    if (filter_id < FDCAN_STM32_NB_STD_FILTER) {
        unset_standard_filter(can, filter_id);
    }
    else {
        unset_extended_filter(can, filter_id - FDCAN_STM32_NB_STD_FILTER);
    }
}

void candev_stm32_set_pins(can_t *dev, gpio_t tx_pin, gpio_t rx_pin,
                           gpio_af_t af)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;

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

    DEBUG("%s: FDCAN%u GPIO RX = %lu\n",
          __func__, get_channel_id(can), rx_pin);
    DEBUG("%s: FDCAN%u GPIO TX = %lu\n",
          __func__, get_channel_id(can), tx_pin);

    dev->af = af;
    gpio_init_af(rx_pin, af);
    gpio_init_af(tx_pin, af);
}

static int _init(candev_t *candev)
{
    can_t *dev = container_of(candev, can_t, candev);
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    _can[get_channel(can)] = dev;

    /* Erase TX mailbox and RX FIFO */
    memset(dev->tx_mailbox, 0, sizeof(dev->tx_mailbox));
    memset(&dev->rx_mailbox, 0, sizeof(dev->rx_mailbox));

    dev->isr_flags.isr_tx = 0;
    dev->isr_flags.isr_rx = 0;

    /* Enable device clock */
    periph_clk_en(APB1, dev->conf->rcc_mask);

    DEBUG("%s: FDCAN%u RCC->CCIPR = %lx\n",
          __func__, get_channel_id(can), RCC->CCIPR);

    _status[get_channel(can)] = STATUS_ON;

    /* configure pins */
    candev_stm32_set_pins(dev, dev->conf->tx_pin, dev->conf->rx_pin, dev->conf->af);

    set_mode(can, MODE_INIT);

    set_bit_timing(dev);

    /* Enable interrupts */
    NVIC_EnableIRQ(dev->conf->it0_irqn);
    NVIC_EnableIRQ(dev->conf->it1_irqn);

    can->CCCR &= ~(FDCAN_CCCR_DAR       /* Enable auto retransmission on failure */
                   | FDCAN_CCCR_PXHD);  /* Enable protocol exception handling */
    can->CCCR |= (FDCAN_CCCR_FDOE       /* Enable FD mode */
                  | FDCAN_CCCR_BRSE     /* Enable bitrate switching */
                  | FDCAN_CCCR_TXP);    /* Enable transmit pause */

#ifdef STM32_PM_STOP
    pm_block(STM32_PM_STOP);
#endif

    /* Clear interrupt flags */
    can->IR |= 0xFFFFFFU;
    /* Enable all interrupts and both interrupt lines */
    can->IE |= 0xFFFFFFU;

    can->TXBTIE = FDCAN_TXBTIE_TIE;

    can->ILS = FDCAN_ILS_PERR | FDCAN_ILS_SMSG;
    can->ILE |= FDCAN_ILE_EINT0 | FDCAN_ILE_EINT1;

    DEBUG("%s: FDCAN%u->CCCR =  %lx\n",
          __func__, get_channel_id(can), can->CCCR);

    int res = set_mode(can, MODE_NORMAL);

    DEBUG("%s: FDCAN%u init done, return %d\n",
          __func__, get_channel_id(can), res);

    /* Reject non matching standard IDs */
    can->RXGFC |= FDCAN_RXGFC_ANFS;
    /* Reject non matching extended IDs */
    can->RXGFC |= FDCAN_RXGFC_ANFE;

    return res;
}

static inline void set_bit_timing(can_t *dev)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    DEBUG("%s: FDCAN%u setup bittiming\n",
          __func__, get_channel_id(can));
    /* Normal CAN bittiming */
    can->NBTP = 0;
    can->NBTP = (dev->candev.bittiming.sjw - 1)
                 << FDCAN_NBTP_NSJW_Pos     /* SJW */
                | (dev->candev.bittiming.phase_seg2 - 1)
                   << FDCAN_NBTP_NTSEG2_Pos /* Phase Seg 2 */
                | (dev->candev.bittiming.phase_seg1
                   + dev->candev.bittiming.prop_seg - 1)
                   << FDCAN_NBTP_NTSEG1_Pos /* Phase Seg 1 */
                | (dev->candev.bittiming.brp - 1)
                   << FDCAN_NBTP_NBRP_Pos;  /* BRP */

    DEBUG("%s: FDCAN%u->NBTP = %lx\n",
          __func__, get_channel_id(can), can->NBTP);

    /* FD CAN bittiming */
    can->DBTP = 0;
    can->DBTP = (dev->candev.fd_data_bittiming.sjw - 1)
                 << FDCAN_DBTP_DSJW_Pos     /* SJW */
                | (dev->candev.fd_data_bittiming.phase_seg2 - 1)
                   << FDCAN_DBTP_DTSEG2_Pos /* Phase Seg 2 */
                | (dev->candev.fd_data_bittiming.phase_seg1
                   + dev->candev.fd_data_bittiming.prop_seg - 1)
                   << FDCAN_DBTP_DTSEG1_Pos /* Phase Seg 1 */
                | (dev->candev.fd_data_bittiming.brp - 1)
                   << FDCAN_DBTP_DBRP_Pos   /* BRP */
                | FDCAN_DBTP_TDC;           /* Transceiver delay compensation */

    DEBUG("%s: FDCAN%u->DBTP = %lx\n",
          __func__, get_channel_id(can), can->DBTP);

    can->TDCR &= ~FDCAN_TDCR_TDCO;
    can->TDCR |= (dev->candev.loop_delay / dev->candev.bittiming.tq) << FDCAN_TDCR_TDCO_Pos;

    DEBUG("%s: FDCAN%u->TDCR = %lx\n",
          __func__, get_channel_id(can), can->TDCR);
}

static uint32_t dlc_to_len(__uint32_t dlc) {
    if (dlc <= 8) {
        return dlc;
    }
    switch (dlc)
    {
    case 9:
        return 12;
    case 10:
        return 16;
    case 11:
        return 20;
    case 12:
        return 24;
    case 13:
        return 32;
    case 14:
        return 48;
    case 15:
        return 64;
    default:
        DEBUG("%s: FDCAN DLC greater than 15, should never happens, consider it as 15",
              __func__);
        return 64;
    }
}

static uint32_t len_to_dlc(uint32_t len) {
    if (len <= 8) {
        return len;
    }
    else if (len <= 12) {
        return 9;
    }
    else if (len <= 16) {
        return 10;
    }
    else if (len <= 20) {
        return 11;
    }
    else if (len <= 24) {
        return 12;
    }
    else if (len <= 32) {
        return 13;
    }
    else if (len <= 48) {
        return 14;
    }
    else if (len <= 64) {
        return 15;
    }
    else {
        DEBUG("%s: FDCAN payload size greater than 64, should never happens, consider it as 64",
              __func__);
        return 15;
    }
}

static int _send(candev_t *candev, const can_frame_t *frame)
{
    can_t *dev = container_of(candev, can_t, candev);
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    DEBUG("%s: FDCAN%u candev=%p, frame=%p\n",
          __func__, get_channel_id(can),
          (void *) candev, (void *) frame);

    /* Check Tx FIFO is not full */
    if (can->TXFQS & FDCAN_TXFQS_TFQF) {
        DEBUG("%s: FDCAN%u Tx FIFO is full\n",
              __func__, get_channel_id(can));
        return -ENOMEM;
    }

    /* Get free Tx buffer element */
    uint32_t put_index = (can->TXFQS & FDCAN_TXFQS_TFQPI) >> FDCAN_TXFQS_TFQPI_Pos;

    dev->tx_mailbox[put_index] = frame;

    uint32_t* tx_buffer_element_t0 = get_message_ram_tx_buffer_address(can)
                                     + (put_index * FDCAN_SRAM_TXBUFFER_SIZE);
    memset(tx_buffer_element_t0, 0, FDCAN_SRAM_TXBUFFER_SIZE * 4);
    uint32_t* tx_buffer_element_t1 = tx_buffer_element_t0 + 1;
    uint32_t* tx_buffer_element_data = tx_buffer_element_t1 + 1;
    DEBUG("%s: FDCAN%u tx_buffer_element_t0 = %p\n",
          __func__, get_channel_id(can), tx_buffer_element_t0);
    DEBUG("%s: FDCAN%u tx_buffer_element_t1 = %p\n",
          __func__, get_channel_id(can), tx_buffer_element_t1);
    DEBUG("%s: FDCAN%u tx_buffer_element_data = %p\n",
          __func__, get_channel_id(can), tx_buffer_element_data);

    /* Check identifier */
    if ((frame->can_id & ~CAN_EFF_FLAG) > CAN_EFF_MASK) {
        return -EINVAL;
    }

    /* Determine if it a standard or extended identifier */
    uint32_t xtd_bit = ((((frame->can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG)
                       || (frame->flags & CANFD_FDF)) ? 1 : 0) << FDCAN_SRAM_TXBUFFER_T0_XTD_Pos;
    DEBUG("%s: FDCAN%u xtd_bit = %lx\n",
          __func__, get_channel_id(can), xtd_bit);

    /* Shift standard ID of 18 bits */
    uint32_t id = (xtd_bit ? (frame->can_id & CAN_EFF_MASK)
                  : ((frame->can_id & CAN_SFF_MASK) << FDCAN_SRAM_TXBUFFER_T0_ID_Pos));
    DEBUG("%s: FDCAN%u id = %lx\n",
          __func__, get_channel_id(can), id);

    uint32_t rtr_bit = ((frame->can_id & CAN_RTR_FLAG) ? 1 : 0)
                       << FDCAN_SRAM_TXBUFFER_T0_RTR_Pos;
    DEBUG("%s: FDCAN%u rtr_bit = %lx\n",
          __func__, get_channel_id(can), rtr_bit);

    *tx_buffer_element_t0 =
        ((((frame->flags & CANFD_ESI) ? 1 : 0) << FDCAN_SRAM_TXBUFFER_T0_ESI_Pos)
                    /* The Error State Indicator (ESI) depends of error passive flag */
        | xtd_bit   /* Message ID identifier type (11/29 bits) */
        | rtr_bit   /* Always a data frame */
        | id);      /* Message ID */

    /* Bit rate switching is specific to FDCAN. */
    if (!(frame->flags & CANFD_FDF) && (frame->flags & CANFD_BRS)) {
        DEBUG("%s: FDCAN%u frame %lu BRS is enabled on a non-fd frame\n",
              __func__, get_channel_id(can), frame->can_id);
        return -EINVAL;
    }

    /* Check length */
    if ((frame->flags & CANFD_FDF) && (frame->len > CANFD_MAX_DLEN)) {
        DEBUG("%s: FDCAN%u frame %lu len is upper than 64 bytes (%d)\n",
              __func__, get_channel_id(can), frame->can_id, frame->len);
        return -EINVAL;
    }
    if (!(frame->flags & CANFD_FDF) && (frame->len > CAN_MAX_DLEN)) {
        DEBUG("%s: FDCAN%u frame %lu len is upper than 8 bytes (%d)\n",
              __func__, get_channel_id(can), frame->can_id, frame->len);
        return -EINVAL;
    }

    *tx_buffer_element_t1 = 0;
    *tx_buffer_element_t1 = FDCAN_SRAM_TXBUFFER_T1_EFC_DISABLE          /* ESI bit */
                            | ((frame->flags & CANFD_BRS)
                              ? FDCAN_SRAM_TXBUFFER_T1_BRS_ON
                              : FDCAN_SRAM_TXBUFFER_T1_BRS_OFF)         /* Bit Rate Switching */
                            | ((frame->flags & CANFD_FDF)
                              ? FDCAN_SRAM_TXBUFFER_T1_FDF_FD
                              : FDCAN_SRAM_TXBUFFER_T1_FDF_CLASSIC)     /* CAN FD frame */
                            | (len_to_dlc(frame->len)
                              << FDCAN_SRAM_TXBUFFER_T1_DLC_Pos);       /* Data Length Code*/

    DEBUG("%s: FDCAN%u *tx_buffer_element_t0 = %lx\n",
          __func__, get_channel_id(can), *tx_buffer_element_t0);
    DEBUG("%s: FDCAN%u *tx_buffer_element_t1 = %lx\n",
          __func__, get_channel_id(can), *tx_buffer_element_t1);

    /* Set data */
    for (uint8_t byte = 0; byte < frame->len && !rtr_bit; byte++) {
        *tx_buffer_element_data |= (uint32_t)frame->data[byte] << ((byte % 4) * 8);
        DEBUG("%s: FDCAN%u *tx_buffer_element_data[%u] = %lx\n",
              __func__, get_channel_id(can),
              byte / 4,
              *tx_buffer_element_data);

        if (byte % 4 == 3) {
            /* Increment buffer pointer */
            ++tx_buffer_element_data;

            DEBUG("%s: FDCAN%u tx_buffer_element_data = %p\n",
                  __func__, get_channel_id(can), tx_buffer_element_data);
        }
    }

    /* Request transmission */
    can->TXBAR = 1 << put_index;

    return put_index;
}

static int _abort(candev_t *candev, const can_frame_t *frame)
{
    can_t *dev = container_of(candev, can_t, candev);
    FDCAN_GlobalTypeDef *can = dev->conf->can;
    int mailbox = 0;

    for (mailbox = 0; mailbox < FDCAN_STM32_TX_MAILBOXES; mailbox++) {
        if (dev->tx_mailbox[mailbox] == frame) {
            break;
        }
    }
    if (mailbox == FDCAN_STM32_TX_MAILBOXES) {
        return -EOVERFLOW;
    }

    can->TXBCR = 1 << (uint32_t)mailbox;
    dev->tx_mailbox[mailbox] = NULL;

    return 0;
}

static int read_frame(can_t *dev, can_frame_t *frame, int message_ram_rx_fifo)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    DEBUG("%s: FDCAN%u Rx FIFO%u is used\n",
          __func__, get_channel_id(can),
          message_ram_rx_fifo);

    uint32_t get_index = 0;

    if (message_ram_rx_fifo) {
        get_index = (can->RXF1S & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos;
    }
    else {
        get_index = (can->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos;
    }

    uint32_t* rx_fifo_element_r0 = get_message_ram_rx_fifo_address(can, message_ram_rx_fifo)
                                   + get_index * FDCAN_SRAM_RXFIFO_ELEMENT_SIZE;
    uint32_t* rx_fifo_element_r1 = rx_fifo_element_r0 + 1;
    uint8_t* rx_fifo_element_data = (uint8_t*)rx_fifo_element_r1 + 4;

    DEBUG("%s: FDCAN%u rx_fifo_element_r0 = %p\n",
          __func__, get_channel_id(can), rx_fifo_element_r0);
    DEBUG("%s: FDCAN%u rx_fifo_element_r1 = %p\n",
          __func__, get_channel_id(can), rx_fifo_element_r1);
    DEBUG("%s: FDCAN%u *rx_fifo_element_r0 = %lx\n",
          __func__, get_channel_id(can), *rx_fifo_element_r0);
    DEBUG("%s: FDCAN%u *rx_fifo_element_r1 = %lx\n",
          __func__, get_channel_id(can), *rx_fifo_element_r1);

    /* Check identifier */
    if ((frame->can_id & ~CAN_EFF_FLAG) > CAN_EFF_MASK) {
        return -EINVAL;
    }

    /* Determine if it is a standard or extended identifier */
    if (*rx_fifo_element_r0 & FDCAN_SRAM_RXFIFO_R0_XTD) {
        frame->can_id = *rx_fifo_element_r0 & FDCAN_SRAM_RXFIFO_R0_ID;
        frame->can_id |= CAN_EFF_FLAG;
    }
    else {
        frame->can_id = (*rx_fifo_element_r0 & FDCAN_SRAM_RXFIFO_R0_ID)
                        >> FDCAN_SRAM_RXFIFO_R0_ID_Pos;
    }

    /* Check if it is a RTR frame */
    if (*rx_fifo_element_r0 & FDCAN_SRAM_RXFIFO_R0_RTR) {
        frame->can_id |= CAN_RTR_FLAG;
    }

    /* Check if it is a CAN FD frame */
    if (*rx_fifo_element_r1 & FDCAN_SRAM_RXFIFO_R1_FDF) {
        frame->flags |= CANFD_FDF;
    }

    /* Check if Bit Rate Switching is on */
    if (*rx_fifo_element_r1 & FDCAN_SRAM_RXFIFO_R1_BRS) {
        frame->flags |= CANFD_BRS;
    }

    frame->len = dlc_to_len((*rx_fifo_element_r1 & FDCAN_SRAM_RXFIFO_R1_DLC)
                 >> FDCAN_SRAM_RXFIFO_R1_DLC_Pos);

    /* Get Data */
    for (uint8_t byte = 0; byte < frame->len && !(frame->can_id & CAN_RTR_FLAG); byte++) {
        DEBUG("%s: FDCAN%u rx_fifo_element_data = %p\n",
              __func__, get_channel_id(can), rx_fifo_element_data);
        DEBUG("%s: FDCAN%u *rx_fifo_element_data = %x\n",
              __func__, get_channel_id(can), *rx_fifo_element_data);
        frame->data[byte] = *rx_fifo_element_data;
        rx_fifo_element_data++;
    }

    if (message_ram_rx_fifo) {
        can->RXF1A = get_index;
    }
    else {
        can->RXF0A = get_index;
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

static void _wkup_cb(void *arg)
{
    can_t *dev = arg;
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    gpio_irq_disable(dev->rx_pin);

    DEBUG("%s: FDCAN%u int wkup: %p\n", __func__, get_channel_id(can), arg);

    dev->isr_flags.isr_wkup = 1;

    if (dev->candev.event_callback) {
        dev->candev.event_callback(&dev->candev, CANDEV_EVENT_ISR, NULL);
    }
}

/* Disable GPIO Rx interrupt on wake up */
static void disable_gpio_int(can_t *dev)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    DEBUG("%s: FDCAN%u disable Rx GPIO interrupt\n",
          __func__, get_channel_id(can));

    gpio_irq_disable(dev->rx_pin);
    candev_stm32_set_pins(dev, dev->tx_pin, dev->rx_pin, dev->af);
}

/* Enable GPIO Rx interrupt on sleep */
static void enable_gpio_int(can_t *dev)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    DEBUG("%s: FDCAN%u enable Rx GPIO interrupt\n",
          __func__, get_channel_id(can));

    gpio_init_int(dev->rx_pin, GPIO_IN, GPIO_FALLING, _wkup_cb, dev);
}

static void turn_off(can_t *dev)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    DEBUG("%s: FDCAN%u turn off (%p)\n", __func__, get_channel_id(can), (void *)dev);

    unsigned irq = irq_disable();

    if (_status[get_channel(dev->conf->can)] != STATUS_SLEEP) {
#ifdef STM32_PM_STOP
        pm_unblock(STM32_PM_STOP);
#endif
    }

    _status[get_channel(dev->conf->can)] = STATUS_SLEEP;

    if (dev->conf->en_deep_sleep_wake_up) {
        periph_clk_dis(APB1, dev->conf->rcc_mask);
        enable_gpio_int(dev);
    }

    irq_restore(irq);
}

static void turn_on(can_t *dev)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    DEBUG("%s: FDCAN%u turn on (%p)\n",
          __func__, get_channel_id(can), dev);

    unsigned irq = irq_disable();

    if (_status[get_channel(dev->conf->can)] == STATUS_SLEEP) {
#ifdef STM32_PM_STOP
        pm_block(STM32_PM_STOP);
#endif
        if (dev->conf->en_deep_sleep_wake_up) {
            disable_gpio_int(dev);
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
    FDCAN_GlobalTypeDef *can = dev->conf->can;
    int res = 0;
    can_mode_t mode;

    switch (opt) {
        case CANOPT_BITTIMING:
            DEBUG("%s: FDCAN%u set bittiming\n", __func__, get_channel_id(can));
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
        case CANOPT_FD_BITTIMING:
            DEBUG("%s: FDCAN%u set FDCAN data bittiming\n", __func__, get_channel_id(can));
            if (value_len < sizeof(dev->candev.fd_data_bittiming)) {
                res = -EOVERFLOW;
            }
            else {
                memcpy(&dev->candev.fd_data_bittiming, value,
                       sizeof(dev->candev.fd_data_bittiming));
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
                    res = sizeof(dev->candev.fd_data_bittiming);
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
                        DEBUG("%s: FDCAN%u power down\n", __func__, get_channel_id(can));
                        res = _sleep(dev);
                        break;
                    case CANOPT_STATE_ON:
                        DEBUG("%s: FDCAN%u power up\n", __func__, get_channel_id(can));
                        res = _wake_up(dev);
                        break;
                    case CANOPT_STATE_LISTEN_ONLY:
                        DEBUG("%s: FDCAN%u listen only\n", __func__, get_channel_id(can));
                        mode = get_mode(can);
                        res = set_mode(can, MODE_INIT);
                        can->CCCR |= FDCAN_CCCR_MON;
                        res += set_mode(can, mode);
                        break;
                    case CANOPT_STATE_LOOPBACK:
                        DEBUG("%s: FDCAN%u loopback\n", __func__, get_channel_id(can));
                        res = set_mode(can, MODE_INIT);
                        can->TEST |= FDCAN_TEST_LBCK;
                        can->CCCR |= FDCAN_CCCR_MON;
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

static int _get(candev_t *candev, canopt_t opt, void *value, size_t max_len)
{
    can_t *dev = container_of(candev, can_t, candev);
    FDCAN_GlobalTypeDef *can = dev->conf->can;
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
        case CANOPT_FD_BITTIMING:
            if (max_len < sizeof(dev->candev.fd_data_bittiming)) {
                res = -EOVERFLOW;
            }
            else {
                memcpy(value, &dev->candev.fd_data_bittiming,
                       sizeof(dev->candev.fd_data_bittiming));
                res = sizeof(dev->candev.fd_data_bittiming);
            }
            break;
        case CANOPT_FD_BITTIMING_CONST:
            if (max_len < sizeof(fd_data_bittiming_const)) {
                res = -EOVERFLOW;
            }
            else {
                memcpy(value, &fd_data_bittiming_const, sizeof(fd_data_bittiming_const));
                res = sizeof(fd_data_bittiming_const);
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
                uint8_t nb_enabled_filter = 0;
                struct can_filter *filter_list = (struct can_filter *)value;
                size_t filter_list_size = max_len / sizeof(struct can_filter);
                for (unsigned i = 0; i < FDCAN_STM32_NB_FILTER; i++) {
                    struct can_filter *filter = filter_list + nb_enabled_filter;
                    if (filter_is_set(can, i)) {
                        nb_enabled_filter++;
                        get_can_filter(can, i, &filter->can_id, &filter->can_mask);
                    }
                    if (filter_list_size <= nb_enabled_filter) {
                        res = -EOVERFLOW;
                        break;
                    }
                }
                if (!res) {
                    res = nb_enabled_filter * sizeof(struct can_filter);
                }
            }
            break;
        case CANOPT_TEC:
            if (max_len != sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                uint16_t *tec = (uint16_t *)value;
                *tec = (can->ECR & FDCAN_ECR_TEC) >> FDCAN_ECR_TEC_Pos;
                res = sizeof(uint16_t);
            }
            break;
        case CANOPT_REC:
            if (max_len != sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                uint16_t *rec = (uint16_t *)value;
                *rec = (can->ECR & FDCAN_ECR_REC) >> FDCAN_ECR_REC_Pos;
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
    FDCAN_GlobalTypeDef *can = dev->conf->can;
    int res = 0;

    DEBUG("%s: FDCAN%u dev=%p, filter=0x%" PRIx32 ", mask=0x%" PRIx32 "\n", __func__,
          get_channel_id(can), (void *)candev, filter->can_id, filter->can_mask);

    uint8_t i;
    for (i = 0; i < FDCAN_STM32_NB_FILTER; i++) {
        if (!filter_is_set(can, i)
                && ((i >= FDCAN_STM32_NB_STD_FILTER)    /* First free filter slot is an extended
                                                           filter */
                || (!(filter->can_id & ~CAN_SFF_MASK)
                    &&  !(filter->can_mask & ~CAN_SFF_MASK)))) { /* or the filter is standard */
            can_mode_t mode = get_mode(can);
            set_mode(can, MODE_INIT);
            res = set_filter(can, filter->can_id, filter->can_mask, i, i % FDCAN_STM32_RX_MAILBOXES);
            set_mode(can, mode);
            if (res) {
                return -EINVAL;
            }

            break;
        }
    }

    if (i == FDCAN_STM32_NB_FILTER) {
        return -EOVERFLOW;
    }

    return i;
}

static int _remove_filter(candev_t *candev, const struct can_filter *filter)
{
    can_t *dev = container_of(candev, can_t, candev);
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    uint8_t i;

    for (i = 0; i < FDCAN_STM32_NB_FILTER; i++) {
        if (filter_is_set(can, i)) {
            uint32_t filt, mask;
            get_can_filter(can, i, &filt, &mask);
            DEBUG("%s: FDCAN%u filter=0x%" PRIx32 ",0x%" PRIx32 ", nb=%d, "
                  "dev_filter=0x%" PRIx32 ",0x%" PRIx32 "\n", __func__, get_channel_id(can),
                  filter->can_id, filter->can_mask, (int)i, filt, mask);
            if ((filt == filter->can_id) /* ID match */
                    /* Filter match (extended case) */
                    && (((filt & CAN_EFF_FLAG)
                        && ((mask & CAN_EFF_MASK) == (filter->can_mask & CAN_EFF_MASK)))
                    /* Filter match (standard case) */
                    || (!(filt & CAN_EFF_FLAG)
                        && ((mask & CAN_SFF_MASK) == (filter->can_mask & CAN_SFF_MASK))))) {
                can_mode_t mode = get_mode(can);
                set_mode(can, MODE_INIT);
                unset_filter(can, i);
                set_mode(can, mode);
                break;
            }
        }
    }

    return 0;
}

static void tx_conf(can_t *dev, int mailbox)
{
    candev_t *candev = (candev_t *) dev;
    FDCAN_GlobalTypeDef *can = dev->conf->can;
    const can_frame_t *frame = dev->tx_mailbox[mailbox];

    dev->tx_mailbox[mailbox] = NULL;

    DEBUG("%s: FDCAN%u, mailbox=%d\n",
          __func__, get_channel_id(can),
          mailbox);

    if (frame && dev->candev.event_callback) {
        dev->candev.event_callback(candev, CANDEV_EVENT_TX_CONFIRMATION,
                                   (void *) frame);
    }
}

static void tx_irq_handler(can_t *dev)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;
    int flags = dev->isr_flags.isr_tx;

    //DEBUG("%s: FDCAN%u tx irq\n", __func__, get_channel_id(can));

    if (can->IR & FDCAN_IR_TC) {
        dev->isr_flags.isr_tx |= can->TXBTO;
        can->IR |= FDCAN_IR_TC;
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

static void rx_new_message_irq_handler(can_t *dev, uint8_t message_ram_rx_fifo)
{
    candev_t *candev = (candev_t *) dev;
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    DEBUG("%s: FDCAN%u rx new message irq\n", __func__, get_channel_id(can));

    if (!dev->rx_mailbox.is_full) {
        int i = dev->rx_mailbox.write_idx;
        read_frame(dev, &(dev->rx_mailbox.frame[i]), message_ram_rx_fifo);

        if (!dev->isr_flags.isr_rx) {
            dev->isr_flags.isr_rx |= message_ram_rx_fifo + 1;
            if (dev->candev.event_callback) {
                dev->candev.event_callback(candev, CANDEV_EVENT_ISR, NULL);
            }
        }

        dev->rx_mailbox.write_idx++;
        if (dev->rx_mailbox.write_idx == FDCAN_STM32_RX_MAILBOXES) {
            dev->rx_mailbox.write_idx = 0;
        }
        if (dev->rx_mailbox.write_idx == dev->rx_mailbox.read_idx) {
            dev->rx_mailbox.is_full = 1;
        }
    }
    else {
        DEBUG("%s: FDCAN%u RX fifo is full!\n", __func__, get_channel_id(can));
        if (dev->candev.event_callback) {
            dev->candev.event_callback(candev, CANDEV_EVENT_RX_ERROR, NULL);
        }
    }
}

static void rx_isr(can_t *dev)
{
    DEBUG("_rx_isr: device=%p\n", (void *)dev);

    while (dev->rx_mailbox.is_full || dev->rx_mailbox.read_idx != dev->rx_mailbox.write_idx) {
        int i = dev->rx_mailbox.read_idx;

        if (dev->candev.event_callback) {
            dev->candev.event_callback(&dev->candev,
                                       CANDEV_EVENT_RX_INDICATION,
                                       &dev->rx_mailbox.frame[i]);
        }

        dev->rx_mailbox.read_idx++;
        if (dev->rx_mailbox.read_idx == FDCAN_STM32_RX_MAILBOXES) {
            dev->rx_mailbox.read_idx = 0;
        }

        dev->rx_mailbox.is_full = 0;
    }
}

static void rx_irq_handler(can_t *dev)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;
    candev_t *candev = (candev_t *) dev;

    DEBUG("%s: FDCAN%u rx irq\n", __func__, get_channel_id(can));

    /* FIFO 0 */
    if ((can->IR & FDCAN_IR_RF0L) == FDCAN_IR_RF0L) {
        DEBUG("%s: FDCAN%u RF0L: Rx FIFO 0 message lost\n", __func__, get_channel_id(can));
        can->IR |= FDCAN_IR_RF0L;
        if (dev->candev.event_callback) {
            dev->candev.event_callback(candev, CANDEV_EVENT_RX_ERROR, NULL);
        }
    }
    if ((can->IR & FDCAN_IR_RF0F) == FDCAN_IR_RF0F) {
        DEBUG("%s: FDCAN%u RF0F: Rx FIFO 0 full\n", __func__, get_channel_id(can));
        can->IR |= FDCAN_IR_RF0F;
        if (dev->candev.event_callback) {
            dev->candev.event_callback(candev, CANDEV_EVENT_RX_ERROR, NULL);
        }
    }
    if ((can->IR & FDCAN_IR_RF0N) == FDCAN_IR_RF0N) {
        DEBUG("%s: FDCAN%u RF0N: Rx FIFO 0 new message\n", __func__, get_channel_id(can));
        can->IR |= FDCAN_IR_RF0N;
        rx_new_message_irq_handler(dev, 0);
    }

    /* FIFO 1 */
    if ((can->IR & FDCAN_IR_RF1L) == FDCAN_IR_RF1L) {
        DEBUG("%s: FDCAN%u RF1L: Rx FIFO 1 message lost\n", __func__, get_channel_id(can));
        can->IR |= FDCAN_IR_RF0L;
        if (dev->candev.event_callback) {
            dev->candev.event_callback(candev, CANDEV_EVENT_RX_ERROR, NULL);
        }
    }
    if ((can->IR & FDCAN_IR_RF1F) == FDCAN_IR_RF1F) {
        DEBUG("%s: FDCAN%u RF1F: Rx FIFO 1 full\n", __func__, get_channel_id(can));
        can->IR |= FDCAN_IR_RF1F;
        if (dev->candev.event_callback) {
            dev->candev.event_callback(candev, CANDEV_EVENT_RX_ERROR, NULL);
        }
    }
    if ((can->IR & FDCAN_IR_RF1N) == FDCAN_IR_RF1N) {
        DEBUG("%s: FDCAN%u RF1N: Rx FIFO 1 new message\n", __func__, get_channel_id(can));
        can->IR |= FDCAN_IR_RF1N;
        rx_new_message_irq_handler(dev, 1);
    }
}

static void irq_handler(can_t *dev)
{
    FDCAN_GlobalTypeDef *can = dev->conf->can;

    DEBUG("%s: FDCAN%u Got interrupts, can->IR = %lx\n", __func__,
          get_channel_id(can), dev->conf->can->IR);

    if (dev->conf->can->IR & 0x3F) {
        rx_irq_handler(dev);
    }
    if (dev->conf->can->IR & 0xFC0) {
        tx_irq_handler(dev);
    }
    if (can->IR & FDCAN_IR_PEA) {
        DEBUG("%s: FDCAN%u PSR.LEC = %lx\n", __func__, get_channel_id(can),
              dev->conf->can->PSR & FDCAN_PSR_LEC);
        DEBUG("%s: FDCAN%u ECR = %lx\n", __func__, get_channel_id(can),
              dev->conf->can->ECR);
        can->IR |= FDCAN_IR_PEA;
    }
    if (can->IR & FDCAN_IR_PED) {
        DEBUG("%s: FDCAN%u PSR.DLEC = %lx\n", __func__, get_channel_id(can),
              dev->conf->can->PSR & FDCAN_PSR_DLEC);
        DEBUG("%s: FDCAN%u ECR = %lx\n", __func__, get_channel_id(can), dev->conf->can->ECR);
        can->IR |= FDCAN_IR_PED;
    }

    if (dev->conf->can->IR) {
        DEBUG("%s: FDCAN%u Unhandled interrupts, can->IR = %lx\n", __func__,
              get_channel_id(can), dev->conf->can->IR);
        can->IR |= can->IR;
    }
}

void ISR_FDCAN1_IT0(void)
{
    irq_handler(_can[0]);

    cortexm_isr_end();
}

void ISR_FDCAN1_IT1(void)
{
    irq_handler(_can[0]);

    cortexm_isr_end();
}

#if defined(FDCAN2)
void ISR_FDCAN2_IT0(void)
{
    irq_handler(_can[1]);

    cortexm_isr_end();
}

void ISR_FDCAN2_IT1(void)
{
    irq_handler(_can[1]);

    cortexm_isr_end();
}
#endif /* FDCAN2 */

#if defined(FDCAN3)
void ISR_FDCAN3_IT0(void)
{
    irq_handler(_can[2]);

    cortexm_isr_end();
}

void ISR_FDCAN3_IT1(void)
{
    irq_handler(_can[2]);

    cortexm_isr_end();
}
#endif /* FDCAN3 */
