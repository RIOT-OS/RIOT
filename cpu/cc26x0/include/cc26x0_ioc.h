/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0_ioc_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 MCU I/O register definitions
 *
 * @author          Leon George <leon@georgemail.eu>
 */

#ifndef CC26x0_IOC_H
#define CC26x0_IOC_H

#include "cc26x0.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define MCU_IOC_BASE           (0x40081000) /**< IOC (MCU) base address */
/** @} */


/**
 * @brief obtain IOCFG-register for a DIO
 *
 * @param[in] dio_num DIO number (0-31)
 */
typedef struct {
    reg32_t CFG[32]; /**< config */
} cc26x0_ioc_regs_t;

#define IOC ((cc26x0_ioc_regs_t *)(MCU_IOC_BASE)) /**< IOC register banks */

/**
 * @name values for IOCFG
 * @{
 */
#define IOCFG_PORTID_GPIO               0x00000000  /**< GPIO */
#define IOCFG_PORTID_AON_SCS            0x00000001  /**< AON SPI-S SCS */
#define IOCFG_PORTID_AON_SCK            0x00000002  /**< AON SPI-S SCK */
#define IOCFG_PORTID_AON_SDI            0x00000003  /**< AON SPI-S SDI */
#define IOCFG_PORTID_AON_SDO            0x00000004  /**< AON SPI-S SDO */
#define IOCFG_PORTID_AON_CLK32K         0x00000007  /**< AON external 32kHz clock */
#define IOCFG_PORTID_AUX_IO             0x00000008  /**< AUX IO */
#define IOCFG_PORTID_SSI0_RX            0x00000009  /**< SSI0 receive */
#define IOCFG_PORTID_SSI0_TX            0x0000000A  /**< SSI0 transmit */
#define IOCFG_PORTID_SSI0_FSS           0x0000000B  /**< SSI0 FSS */
#define IOCFG_PORTID_SSI0_CLK           0x0000000C  /**< SSI0 clock */
#define IOCFG_PORTID_I2C_MSSDA          0x0000000D  /**< I2C data */
#define IOCFG_PORTID_I2C_MSSCL          0x0000000E  /**< I2C clock */
#define IOCFG_PORTID_UART0_RX           0x0000000F  /**< UART0 receive */
#define IOCFG_PORTID_UART0_TX           0x00000010  /**< UART0 transmit */
#define IOCFG_PORTID_UART0_CTS          0x00000011  /**< UART0 clear to send */
#define IOCFG_PORTID_UART0_RTS          0x00000012  /**< UART0 request to send */
#define IOCFG_PORTID_PORT_EVT0          0x00000017  /**< PORT EVENT 0 */
#define IOCFG_PORTID_PORT_EVT1          0x00000018  /**< PORT EVENT 1 */
#define IOCFG_PORTID_PORT_EVT2          0x00000019  /**< PORT EVENT 2 */
#define IOCFG_PORTID_PORT_EVT3          0x0000001A  /**< PORT EVENT 3 */
#define IOCFG_PORTID_PORT_EVT4          0x0000001B  /**< PORT EVENT 4 */
#define IOCFG_PORTID_PORT_EVT5          0x0000001C  /**< PORT EVENT 5 */
#define IOCFG_PORTID_PORT_EVT6          0x0000001D  /**< PORT EVENT 6 */
#define IOCFG_PORTID_PORT_EVT7          0x0000001E  /**< PORT EVENT 7 */
#define IOCFG_PORTID_SWV                0x00000020  /**< serial wire viewer */
#define IOCFG_PORTID_SSI1_RX            0x00000021  /**< SSI1 receive */
#define IOCFG_PORTID_SSI1_TX            0x00000022  /**< SSI1 transmit */
#define IOCFG_PORTID_SSI1_FSS           0x00000023  /**< SSI1 FSS */
#define IOCFG_PORTID_SSI1_CLK           0x00000024  /**< SSI1 clock */
#define IOCFG_PORTID_I2S_AD0            0x00000025  /**< I2S data 0 */
#define IOCFG_PORTID_I2S_AD1            0x00000026  /**< I2S data 1 */
#define IOCFG_PORTID_I2S_WCLK           0x00000027  /**< I2S frame/word clock */
#define IOCFG_PORTID_I2S_BCLK           0x00000028  /**< I2S bit clock */
#define IOCFG_PORTID_I2S_MCLK           0x00000029  /**< I2S master clock 2 */
#define IOCFG_PORTID_RFC_TRC            0x0000002E  /**< RF core trace */
#define IOCFG_PORTID_RFC_GPO0           0x0000002F  /**< RC core data out 0 */
#define IOCFG_PORTID_RFC_GPO1           0x00000030  /**< RC core data out 1 */
#define IOCFG_PORTID_RFC_GPO2           0x00000031  /**< RC core data out 2 */
#define IOCFG_PORTID_RFC_GPO3           0x00000032  /**< RC core data out 3 */
#define IOCFG_PORTID_RFC_GPI0           0x00000033  /**< RC core data in 0 */
#define IOCFG_PORTID_RFC_GPI1           0x00000034  /**< RC core data in 1 */
#define IOCFG_PORTID_RFC_SMI_DL_OUT     0x00000035  /**< RF core SMI data link out */
#define IOCFG_PORTID_RFC_SMI_DL_IN      0x00000036  /**< RF core SMI data link in */
#define IOCFG_PORTID_RFC_SMI_CMD_OUT    0x00000037  /**< RF core SMI command link out */
#define IOCFG_PORTID_RFC_SMI_CMD_IN     0x00000038  /**< RF core SMI command link in */

#define IOCFG_IOSTR_AUTO                0x00000000  /**< automatic drive strength (2/4/8 mA @ VVDS) */
#define IOCFG_IOSTR_MAX                 0x00000300  /**< maximum drive strength (2/4/8 mA @ 1.8V) */
#define IOCFG_IOSTR_MED                 0x00000200  /**< medium drive strength (2/4/8 mA @ 2.5V) */
#define IOCFG_IOSTR_MIN                 0x00000100  /**< minimum drive strength (2/4/8 mA @ 3.3V) */

#define IOCFG_IOCURR_2MA                0x00000000  /**< 2mA drive strength */
#define IOCFG_IOCURR_4MA                0x00000400  /**< 4mA drive strength */
#define IOCFG_IOCURR_8MA                0x00000800  /**< 4 or 8mA drive strength */
#define IOCFG_IOCURR_16MA               0x00000C00  /**< Up to 16mA drive strength */

#define IOCFG_SLEW_RED                  0x00001000  /**< reduced slew rate */

#define IOCFG_PULLCTL_OFF               0x00006000  /**< no IO pull */
#define IOCFG_PULLCTL_UP                0x00004000  /**< pull up */
#define IOCFG_PULLCTL_DOWN              0x00002000  /**< pull down */

#define IOCFG_EDGEDET_FALLING           0x00010000  /**< edge detection on falling edge */
#define IOCFG_EDGEDET_RISING            0x00020000  /**< edge detection on rising edge */
#define IOCFG_EDGEDET_BOTH              0x00030000  /**< edge detection on both edges */
#define IOCFG_EDGEIRQ_ENABLE            0x00040000  /**< enable interrupt on edge detect */

/* n/a when port_id is AON or AUX */
#define IOCFG_IOMODE_INV                0x01000000  /**< inverted I/O */
#define IOCFG_IOMODE_OPEN_DRAIN         0x04000000  /**< open drain */
#define IOCFG_IOMODE_OPEN_SRC           0x06000000  /**< open source */

/* both enable waking up when port_id is AON or AUX */
#define IOCFG_WUCFG_LOW                 0x10000000
#define IOCFG_WUCFG_HIGH                0x18000000

/* ignored if port_id is AUX */
#define IOCFG_INPUT_ENABLE              0x20000000

#define IOCFG_HYST_ENABLE               0x40000000
/** @} */


/** @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AON_IOC_BASE            (PERIPH_BASE + 0x94000) /**< always-on-IOC base address */
/** @} */

/**
 * AON registers
 */
typedef struct {
    reg32_t IOSTRMIN; /**< IO drive strength minimum */
    reg32_t IOSTRMED; /**< IO drive strength medium */
    reg32_t IOSTRMAX; /**< IO drive strength maximum */
    reg32_t IOCLATCH; /**< IO latch control */
    reg32_t CLK32KCTL; /**< SCLK_LF external output control */
} aon_regs_t;

#define AON                     ((aon_regs_t *) (AON_IOC_BASE)) /**< AON register bank */

#define IOCLATCH_EN             0x00000001 /**< IO controlled by GPIO or peripheral; kept in AON otherwise */

#define CLK32KCTL_OEN           0x00000001 /**< don't output SCLK_LF on DIOs with PORT_ID AON_CLK32K */


#ifdef __cplusplus
}
#endif

#endif /* CC26x0_IOC_H */

/*@}*/
