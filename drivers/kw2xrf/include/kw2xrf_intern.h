/*
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup
 * @{
 *
 * @file
 * @brief       Internal function interfaces for kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#include <stdint.h>
#include "kw2xrf.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Power Modes */
typedef enum {
    KW2XRF_HIBERNATE = 0,
    KW2XRF_DOZE,
    KW2XRF_IDLE,
    KW2XRF_AUTODOZE,
} kw2xrf_powermode_t;

inline void kw2xrf_set_dreg_bit(kw2xrf_t *dev, uint8_t reg, uint8_t bit)
{
    uint8_t tmp = kw2xrf_read_dreg(dev, reg);
    tmp |= bit;
    kw2xrf_write_dreg(dev, reg, tmp);
}

inline void kw2xrf_clear_dreg_bit(kw2xrf_t *dev, uint8_t reg, uint8_t bit)
{
    uint8_t tmp = kw2xrf_read_dreg(dev, reg);
    tmp &= ~bit;
    kw2xrf_write_dreg(dev, reg, tmp);
}

/** Enable any transceiver interrupt to assert IRQ_B */
inline void kw2xrf_enable_irq_b(kw2xrf_t *dev)
{
    kw2xrf_clear_dreg_bit(dev, MKW2XDM_PHY_CTRL4, MKW2XDM_PHY_CTRL4_TRCV_MSK);
}

/** Mask all transceiver interrupts to assert IRQ_B */
inline void kw2xrf_mask_irq_b(kw2xrf_t *dev)
{
    kw2xrf_set_dreg_bit(dev, MKW2XDM_PHY_CTRL4, MKW2XDM_PHY_CTRL4_TRCV_MSK);
}

void kw2xrf_disable_interrupts(kw2xrf_t *dev);

void kw2xrf_update_overwrites(kw2xrf_t *dev);

void kw2xrf_set_out_clk(kw2xrf_t *dev);

void kw2xrf_set_power_mode(kw2xrf_t *dev, kw2xrf_powermode_t pm);

int kw2xrf_can_switch_to_idle(kw2xrf_t *dev);

typedef enum kw2xrf_timer_timebase {
    KW2XRF_TIMEBASE_500000HZ = 2,
    KW2XRF_TIMEBASE_250000HZ,
    KW2XRF_TIMEBASE_125000HZ,
    KW2XRF_TIMEBASE_62500HZ,
    KW2XRF_TIMEBASE_31250HZ,
    KW2XRF_TIMEBASE_15625HZ,
} kw2xrf_timer_timebase_t;

/**
 * Initialize the Event Timer Block (up counter)
 * The Event Timer Block provides:
 *   - Abort an RX and CCA sequence at pre-determined time
 *   - Latches "timestamp" value during packet reception
 *   - Initiates timer-triggered sequences
 */
void kw2xrf_timer_init(kw2xrf_t *dev, kw2xrf_timer_timebase_t tb);

void kw2xrf_timer2_seq_start_on(kw2xrf_t *dev);

void kw2xrf_timer2_seq_start_off(kw2xrf_t *dev);

void kw2xrf_timer3_seq_abort_on(kw2xrf_t *dev);

void kw2xrf_timer3_seq_abort_off(kw2xrf_t *dev);

/**
 * Use T2CMP or T2PRIMECMP to Trigger Transceiver Operations
 */
void kw2xrf_trigger_tx_ops_enable(kw2xrf_t *dev, uint32_t timeout);

/**
 * Disable Trigger for Transceiver Operations
 *
 */
void kw2xrf_trigger_tx_ops_disable(kw2xrf_t *dev);

/**
 * Use T3CMP to Abort an RX operation
 */
void kw2xrf_abort_rx_ops_enable(kw2xrf_t *dev, uint32_t timeout);

/**
 * Disable Trigger to Abort an RX operation
 */
void kw2xrf_abort_rx_ops_disable(kw2xrf_t *dev);

void kw2xrf_seq_timeout_on(kw2xrf_t *dev, uint32_t timeout);

void kw2xrf_seq_timeout_off(kw2xrf_t *dev);

/**
 * Returns Timestamp of the actual received packet
 */
uint32_t kw2xrf_get_timestamp(kw2xrf_t *dev);

#ifdef __cplusplus
}
#endif
/** @} */
