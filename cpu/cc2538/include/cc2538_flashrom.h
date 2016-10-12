/*
 * cc2538_flashrom.h
 *
 *  Created on: 30.09.2016
 *      Author: geith
 */

#ifndef CPU_CC2538_INCLUDE_CC2538_FLASHROM_H_
#define CPU_CC2538_INCLUDE_CC2538_FLASHROM_H_

#include "cc2538.h"
#include "flashrom.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Flash control register
 */
typedef union {
	cc2538_reg_t FCTL;                /**< Flash Control Register */
	struct {
		cc2538_reg_t ERASE             :  1; /**< Flash erase */
		cc2538_reg_t WRITE             :  1; /**< Flash write */
		cc2538_reg_t CM                :  2; /**< Flash cache mode */
		cc2538_reg_t RESERVED_4        :  1; /**< reserved */
		cc2538_reg_t ABORT             :  1; /**< Flash abort status */
		cc2538_reg_t FULL              :  1; /**< Flash write buffer full */
		cc2538_reg_t BUSY              :  1; /**< Flash busy */
		cc2538_reg_t SEL_INFO_PAGE     :  1; /**<  */
		cc2538_reg_t UPPER_PAGE_ACCESS :  1; /**<  */
		cc2538_reg_t RESERVED          : 22; /**< Reserved bits */
	} FCTLbits;
} cc2538_flash_fctl_t;


/**
 * @brief Flash address register
 */
typedef union {
	cc2538_reg_t FADDR;               /**< Flash Address Register */
	struct {
		//cc2538_reg_t FADDR             : 17; /**< Flash address */

		cc2538_reg_t LOC               :  7; /**< Flash location within row */
		cc2538_reg_t ROW               :  2; /**< Flash row */
		cc2538_reg_t PAGE              :  8; /**< Flash page */
		cc2538_reg_t RESERVED          : 15; /**< Reserved bits */
	} FADDRbits;
} cc2538_flash_faddr_t;

extern cc2538_flash_fctl_t* const FCTL;
//extern cc2538_flash_faddr_t* const FADDR;


#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CPU_CC2538_INCLUDE_CC2538_FLASHROM_H_ */
