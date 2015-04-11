/*
 * Copyright 2014, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup     CC2420
 * @brief       CC2420 dependend functions
 *
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Milan Babel <babel@inf.fu-berlin.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef CC2420_ARCH_H
#define CC2420_ARCH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Gets the status of the FIFOP pin.
 *
 * @return Status of the FIFOP pin.
 *
 */
uint8_t cc2420_get_fifop(void);

/**
 * @brief Gets the status of the SFD pin.
 *
 * @return Status of the SFD pin.
 *
 */
uint8_t cc2420_get_sfd(void);

/**
 * @brief Gets the status of the CCA pin
 *
 * @return Status of the CCA pin.
 *
 */
uint8_t cc2420_get_cca(void);

/**
 * @brief Does a hardware reset of the CC2420.
 *
 */
void cc2420_reset(void);

/**
 * @brief Init the SPI interface.
 *
 */
void cc2420_spi_init(void);

/**
 * @brief Selects the CC2420 on the spi bus.
 *
 */
void cc2420_spi_select(void);

/**
 * @brief Unselects the CC2420 on the spi bus.
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

#ifdef __cplusplus
}
#endif

#endif /* CC2420_ARCH_H */
