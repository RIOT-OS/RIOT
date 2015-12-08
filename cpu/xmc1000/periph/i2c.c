/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <stddef.h>

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "sched.h"
#include "mutex.h"
#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"


#define TDF_RECV_ACK       (2 << 8)
#define TDF_RECV_NACK      (3 << 8)
#define TDF_START          (4 << 8)
#define TDF_REPEATED_START (5 << 8)
#define TDF_STOP           (6 << 8)


/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[] =  {
    [I2C_0] = MUTEX_INIT,
};

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    const usic_fdr_t fdr = {
        /* STEP value: (320/1024) * 32Mhz = 10Mhz base clock */
        .field.step = 320,
        /* DM: fractional divider mode */
        .field.dm	= 2
    };

    usic_brg_t brg = {
        /* (DCTQ+1) x (PCTQ+1) / f[PDIV] */
        .field.ctqsel	= USIC_CTQIN_PDIV,
        .field.dctq		= 9,
        .field.pctq		= 0,
        .field.pdiv		= 9
    };

    /* setup & start the USIC channel */
    usic_init((usic_channel_t *)&i2c_instance[dev], brg, fdr);

    return 0;
}

static inline void clear_for_sending(USIC_CH_TypeDef *usic)
{
    while (!(usic->PSR & USIC_CH_PSR_IICMode_TBIF_Msk)) {
        /* wait */
    }

    USIC0_CH1->PSCR = USIC_CH_PSR_TBIF_Msk;
}

uint32_t i2c_transfer_real(i2c_t dev, uint8_t address, uint8_t *reg,
                           char *tx_buf, size_t tx_len,
                           char *rx_buf, size_t rx_len)
{

    USIC_CH_TypeDef *usic = i2c_instance[dev].usic;

    uint16_t sent = 0, received = 0;

    /* clear all interrupt status flags in PSR register */
    usic->PSCR = 0xffffffff;

    /* send start condition with address and r/w bit */
    usic->TBUF[0] = TDF_START | (address << 1) | (!reg && !tx_buf);

    /* wait for detection of start condition */
    while (!(usic->PSR & USIC_CH_PSR_IICMode_SCR_Msk)) ;
    USIC0_CH1->PSCR = USIC_CH_PSR_IICMode_SCR_Msk;

    if (reg) {
        clear_for_sending(usic);

        usic->TBUF[0] = *(reg);

        while (!(usic->PSR & (USIC_CH_PSR_IICMode_ACK_Msk |
                              USIC_CH_PSR_IICMode_NACK_Msk))) ;

        if (usic->PSR & USIC_CH_PSR_IICMode_NACK_Msk) {
            USIC0_CH1->PSCR = USIC_CH_PSR_IICMode_NACK_Msk;
            goto stop;
        }
        else {       /* ACK */
            sent++;
            USIC0_CH1->PSCR = (USIC_CH_PSR_IICMode_ACK_Msk |
                               USIC_CH_PSR_IICMode_NACK_Msk);
        }
    }

    if (tx_buf) {
        for (int i = 0; i < tx_len; i++) {
            clear_for_sending(usic);

            usic->TBUF[0] = tx_buf[i];

            while (!(usic->PSR & (USIC_CH_PSR_IICMode_ACK_Msk |
                                  USIC_CH_PSR_IICMode_NACK_Msk))) ;

            if (usic->PSR & USIC_CH_PSR_IICMode_NACK_Msk) {
                USIC0_CH1->PSCR = USIC_CH_PSR_IICMode_NACK_Msk;
                goto stop;
            }
            else {   /* ACK */
                sent++;
                USIC0_CH1->PSCR = (USIC_CH_PSR_IICMode_ACK_Msk |
                                   USIC_CH_PSR_IICMode_NACK_Msk);
            }
        }
    }

    if (sent && rx_buf) {
        /* clear all interrupt status flags in PSR register */
        usic->PSCR = 0xffffffff;

        /* send start condition with address and r/w bit */
        usic->TBUF[0] = TDF_REPEATED_START | (address << 1) | 1;

        /* busy loop */
        while (!(usic->PSR & USIC_CH_PSR_IICMode_RSCR_Msk)) ;
        USIC0_CH1->PSCR = USIC_CH_PSR_IICMode_RSCR_Msk;
    }

    if (rx_buf) {
        for (int i = 0; i < rx_len - 1; i++) {
            /* busy loop */
            clear_for_sending(usic);

            /* ask for receiving data with acknowledgement */
            usic->TBUF[0] = TDF_RECV_ACK;

            /* busy loop */
            while (!((usic->PSR & USIC_CH_PSR_IICMode_AIF_Msk) |
                     (usic->PSR & USIC_CH_PSR_IICMode_RIF_Msk))) ;

            USIC0_CH1->PSCR = (USIC_CH_PSR_IICMode_AIF_Msk | USIC_CH_PSR_IICMode_RIF_Msk);

            if (usic->PSR & USIC_CH_PSR_IICMode_NACK_Msk) {
                USIC0_CH1->PSCR = USIC_CH_PSR_IICMode_NACK_Msk;
                break;
            }

            rx_buf[received] = usic->RBUF;

            received++;
        }

        /* busy loop */
        clear_for_sending(usic);

        /* ask for receiving data with non-acknowledgement */
        usic->TBUF[0] = TDF_RECV_NACK;

        /* busy loop */
        while (!((usic->PSR & USIC_CH_PSR_IICMode_AIF_Msk) |
                 (usic->PSR & USIC_CH_PSR_IICMode_RIF_Msk))) ;

        USIC0_CH1->PSCR = (USIC_CH_PSR_IICMode_AIF_Msk | USIC_CH_PSR_IICMode_RIF_Msk);

        *(rx_buf + received) = usic->RBUF;

        received++;
    }


stop:
    clear_for_sending(usic);

    /* send stop condition */
    usic->TBUF[0] = TDF_STOP;

    return (sent << 16) | received;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, char *data)
{
    return i2c_transfer_real(dev, address, &reg, NULL, 0, data, 1) & 0xffff;
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
    return i2c_transfer_real(dev, address, &reg, NULL, 0, data, length) & 0xffff;
}

int i2c_read_byte(i2c_t dev, uint8_t address, char *data)
{
    return i2c_transfer_real(dev, address, NULL, NULL, 0, data, 1) & 0xffff;
}

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    return i2c_transfer_real(dev, address, NULL, NULL, 0, data, length) & 0xffff;
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
    return i2c_transfer_real(dev, address, NULL, &data, 1, NULL, 0) >> 16;
}

int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    return i2c_transfer_real(dev, address, NULL, data, length, NULL, 0) >> 16;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, char data)
{
    return i2c_transfer_real(dev, address, &reg, &data, 1, NULL, 0) >> 16;
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
    return i2c_transfer_real(dev, address, &reg, data, length, NULL, 0) >> 16;
}

int i2c_acquire(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }

    mutex_lock(&locks[dev]);
    return 0;
}

int i2c_release(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }

    mutex_unlock(&locks[dev]);
    return 0;
}

void i2c_poweron(i2c_t dev)
{
    /* not implemented yet */
}

void i2c_poweroff(i2c_t dev)
{
    /* not implemented yet */
}
