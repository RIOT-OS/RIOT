/******************************************************************************
 * Copyright 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

/**
 * @file
 * @ingroup		CC2420
 * @brief		CC2420 dependend functions
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Milan Babel <babel@inf.fu-berlin.de>
 * @version     $Revision: 1775 $
 *
 * @note    	$Id: arch_cc110x.h 1775 2010-01-26 09:37:03Z hillebra $
 */

#include <stdint.h>

/**
 * @brief SPI tx and rx function.
 *
 * @param[in] c byte which should be transmitted.
 *
 * @return Byte which was received after transmitting.
 *
 */
uint8_t cc2420_txrx(uint8_t c);

/**
 * @brief Gets the status of the sfd pin.
 *
 * @return Status of the sfd pin.
 *
 */
uint8_t cc2420_get_sfd(void);

/**
 * @brief Does a hardware reset of the cc2420.
 *
 */
void cc2420_reset(void);

/**
 * @brief Init the SPI interface.
 *
 */
void cc2420_spi_init(void);

/**
 * @brief Selects the cc2420 on the spi bus.
 *
 */
void cc2420_spi_select(void);

/**
 * @brief Unselects the cc2420 on the spi bus.
 *
 */
void cc2420_spi_unselect(void);

/**
 * @brief Enable interrupts on the GDO0 pin.
 *
 */
void cc2420_gdo0_enable(void);

/**
 * @brief Disable interrupts on the GDO0 pin.
 *
 */
void cc2420_gdo0_disable(void);

/**
 * @brief Enable interrupts on the GDO2 pin.
 *
 */
void cc2420_gdo2_enable(void);

/**
 * @brief Disable interrupts on the GDO2 pin.
 *
 */
void cc2420_gdo2_disable(void);

/**
 * @brief Init interrupts.
 *
 */
void cc2420_init_interrupts(void);

/**
 * @brief Function called before send to disable interrupts.
 *
 */
void cc2420_before_send(void);

/**
 * @brief Function called after send to reenable interrupts.
 *
 */
void cc2420_after_send(void);
