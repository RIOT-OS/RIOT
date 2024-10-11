/*
 * Copyright (C) 2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waspmote-pro
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Waspmote pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Waspmote specific pin names.
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef WASPMOTE_PINMAP_H
#define WASPMOTE_PINMAP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Mapping of MCU pins to Waspomte board pins
 * @{
 */
/*
 * DESCRIPTION WASP API PIN               PORT   PIN
 */
#define	USB_XBEE_RX             GPIO_PIN(PORT_E, 0)
#define	USB_XBEE_TX             GPIO_PIN(PORT_E, 1)
#define	DIGITAL1                GPIO_PIN(PORT_E, 3)
#define	DIGITAL0                GPIO_PIN(PORT_E, 4)
#define	DIGITAL7                GPIO_PIN(PORT_C, 4)
#define	DIGITAL8                GPIO_PIN(PORT_C, 5)
#define	DIGITAL6                GPIO_PIN(PORT_C, 6)
#define	DIGITAL5                GPIO_PIN(PORT_C, 7)
#define	DIGITAL2                GPIO_PIN(PORT_A, 2)
#define	DIGITAL4                GPIO_PIN(PORT_A, 3)
#define	DIGITAL3                GPIO_PIN(PORT_A, 4)
#define	MUX_USB_XBEE            GPIO_PIN(PORT_D, 5)
#define	LED0                    GPIO_PIN(PORT_F, 4)
#define	LED1                    GPIO_PIN(PORT_C, 1)
#define	ANA0                    GPIO_PIN(PORT_F, 1)
#define	ANA1                    GPIO_PIN(PORT_F, 2)
#define	ANA2                    GPIO_PIN(PORT_F, 3)
#define	ANA3                    GPIO_PIN(PORT_F, 4)
#define	ANA4                    GPIO_PIN(PORT_F, 5)
#define	ANA5                    GPIO_PIN(PORT_F, 6)
#define	ANA6                    GPIO_PIN(PORT_F, 7)
#define	BAT_MONITOR             GPIO_PIN(PORT_F, 0)
#define	XBEE_PW                 GPIO_PIN(PORT_A, 1)
#define	MUX_PW                  GPIO_PIN(PORT_D, 7)
#define	SENS_PW_5V              GPIO_PIN(PORT_E, 5)
#define	BAT_MONITOR_PW          GPIO_PIN(PORT_A, 6)
#define	SENS_PW_3V3             GPIO_PIN(PORT_E, 2)
#define	MEM_PW                  GPIO_PIN(PORT_A, 5)
#define	SD_PRESENT              GPIO_PIN(PORT_C, 0)
#define	SD_SS                   GPIO_PIN(PORT_B, 0)
#define	SD_SCK                  GPIO_PIN(PORT_B, 1)
#define	SD_MOSI                 GPIO_PIN(PORT_B, 2)
#define	SD_MISO                 GPIO_PIN(PORT_B, 3)
#define	HIB_PIN                 GPIO_PIN(PORT_B, 4)
#define	SOCKET0_SS              GPIO_PIN(PORT_B, 5)
#define	GPS_PW                  GPIO_PIN(PORT_A, 0)
#define	MUX_0                   GPIO_PIN(PORT_B, 6)
#define	MUX_1                   GPIO_PIN(PORT_B, 7)
#define	RDY_ACC                 GPIO_PIN(PORT_E, 6)
#define	RST_RTC                 GPIO_PIN(PORT_E, 7)
#define	I2C_SCL                 GPIO_PIN(PORT_D, 0)
#define	I2C_SDA                 GPIO_PIN(PORT_D, 1)
#define	GPRS_PW                 GPIO_PIN(PORT_C, 3)
#define	MUX_RX                  GPIO_PIN(PORT_D, 2)
#define	MUX_TX                  GPIO_PIN(PORT_D, 3)
#define	XBEE_MON                GPIO_PIN(PORT_A, 7)
#define	GPRS_PIN                GPIO_PIN(PORT_C, 2)
#define	XBEE_SLEEP              GPIO_PIN(PORT_D, 4)
#define RTC_PW                  GPIO_PIN(PORT_G, 2)
#define	RTC_SLEEP               GPIO_PIN(PORT_G, 1)
#define	LOW_BAT_MON             GPIO_PIN(PORT_G, 0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* WASPMOTE_PINMAP_H */
