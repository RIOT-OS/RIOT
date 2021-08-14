/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx MCU I/O register definitions
 *
 * @author          Leon George <leon@georgemail.eu>
 */

#ifndef CC26XX_CC13XX_I2C_H
#define CC26XX_CC13XX_I2C_H

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * I2C registers
 */
typedef struct {
    reg32_t SOAR; /**< slave own address */
    union {
        reg32_t SSTAT; /**< slave status */
        reg32_t SCTL; /**< slave control */
    };
    reg32_t SDR; /**< slave data */
    reg32_t SIMR; /**< slave interrupt mask */
    reg32_t SRIS; /**< slave raw interrupt status */
    reg32_t SMIS; /**< slave masked interrupt status */
    reg32_t SICR; /**< slave interrupt clear */
    reg32_t __reserved[0x1F9]; /**< meh */
    reg32_t MSA; /**< master slave address */
    union {
        reg32_t MSTAT; /**< master status */
        reg32_t MCTRL; /**< master control */
    };
    reg32_t MDR; /**< master data */
    reg32_t MTPR; /**< master timer period */
    reg32_t MIMR; /**< master interrupt mask */
    reg32_t MRIS; /**< master raw interrupt status */
    reg32_t MMIS; /**< master masked interrupt statues */
    reg32_t MICR; /**< master interrupt clear */
    reg32_t MCR; /**< master configuration */
} i2c_regs_t;

/**
 * @brief       I2C master function enable
 * @details     0h = Master mode is disabled.
 *              1h = Master mode is enabled
 */
#define MCR_MFE  0x00000010

/**
 * @brief       SCL clock period set to 100 kHZ
 * @details     {PERDMACLK / [2 × (SCL_LP + SCL_HP) × SCL_CLK]} – 1
 *              with SCL_LP==6 && SCL_HP==4 use 0x17 for 100kHZ with 48MHZ CPU clock
 *              This field specifies the period of the SCL clock.
 *              SCL_PRD = 2*(1+TPR)*(SCL_LP + SCL_HP)*CLK_PRD
 *              where:
 *              SCL_PRD is the SCL line period (I2C clock).
 *              TPR is the timer period register value (range of 1 to 127)
 *              SCL_LP is the SCL low period (fixed at 6).
 *              SCL_HP is the SCL high period (fixed at 4).
 *              CLK_PRD is the system clock period in ns.
 */
#define MTPR_TPR_100KHZ  0x00000017

/**
 * @brief       Receive or Send
 * @details     This bit-field specifies if the next operation is a receive (high) or a
 *              transmit/send (low) from the addressed slave SA.
 *              0h = Transmit/send data to slave
 *              1h = Receive data from slave
 */
#define MSA_RS 0x00000001

/**
 * @brief       Bus busy
 * @details     0: The I2C bus is idle.
 *              1: The I2C bus is busy.
 *              The bit changes based on the MCTRL.START and MCTRL.STOP
 *              conditions.
 */
#define MSTAT_BUSBSY 0x00000040

/**
 * @brief       I2C idle
 * @details     0: The I2C controller is not idle.
 *              1: The I2C controller is idle.
 */
#define MSTAT_IDLE  0x00000020

/**
 * @brief       Arbitration lost
 * @details     0: The I2C controller won arbitration.
 *              1: The I2C controller lost arbitration.
 */
#define MSTAT_ARBLST  0x00000010

/**
 * @brief       Data Was Not Acknowledge
 * @details     0: The transmitted data was acknowledged.
 *              1: The transmitted data was not acknowledged.
 */
#define MSTAT_DATACK_N  0x00000008

/**
 * @brief       Address Was Not Acknowledge
 * @details     0: The transmitted address was acknowledged.
 *              1: The transmitted address was not acknowledged.
 */
#define MSTAT_ADRACK_N  0x00000004

/**
 * @brief       Error
 * @details     0: No error was detected on the last operation.
 *              1: An error occurred on the last operation.
 */
#define MSTAT_ERR  0x00000002

/**
 * @brief       I2C busy
 * @details     0: The controller is idle.
 *              1: The controller is busy.
 *              When this bit-field is set, the other status bits are not valid.
@note The I2C controller requires four SYSBUS clock cycles to
 *              assert the BUSY status after I2C master operation has been initiated
 *              through MCTRL register.
 *              Hence after programming MCTRL register, application is requested
 *              to wait for four SYSBUS clock cycles before issuing a controller
 *              status inquiry through MSTAT register.
 *              Any prior inquiry would result in wrong status being reported.
 */
#define MSTAT_BUSY  0x00000001

/**
 * @brief       Data acknowledge enable
 * @details     0: The received data byte is not acknowledged automatically by the
 *              master.
 *              1: The received data byte is acknowledged automatically by the
 *              master.
 *              This bit-field must be cleared when the I2C bus controller requires
 *              no further data to be transmitted from the slave transmitter.
 *              0h = Disable acknowledge
 *              1h = Enable acknowledge
 */
#define MCTRL_ACK  0x00000008

/**
 * @brief       This bit-field determines if the cycle stops at the end of the data
cycle or continues on to a repeated START condition
 * @details     0: The controller does not generate the Stop condition.
 *              1: The controller generates the Stop condition.
 *              0h = Disable STOP
 *              1h = Enable STOP
 */
#define MCTRL_STOP  0x00000004

/**
 * @brief       This bit-field generates the Start or Repeated Start condition
 * @details     0: The controller does not generate the Start condition.
 *              1: The controller generates the Start condition.
 *              0h = Disable START
 *              1h = Enable START
 */
#define MCTRL_START  0x00000002

/**
 * @brief       I2C master enable
 * @details     0: The master is disabled.
 *              1: The master is enabled to transmit or receive data.
 *              0h = Disable Master
 *              1h = Enable Master
 */
#define MCTRL_RUN  0x00000001

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define I2C_BASE           (PERIPH_BASE + 0x2000) /**< I2C base address */
/** @} */

#define I2C ((i2c_regs_t *) (I2C_BASE)) /**< I2C register bank */

#ifdef __cplusplus
}
#endif

#endif /* CC26XX_CC13XX_I2C_H */

/*@}*/
