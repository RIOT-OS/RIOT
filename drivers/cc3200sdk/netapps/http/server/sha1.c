//*****************************************************************************
// Copyright (C) 2014 Texas Instruments Incorporated
//
// All rights reserved. Property of Texas Instruments Incorporated.
// Restricted rights to use, duplicate or disclose this code are
// granted through contract.
// The program may not be used without the written permission of
// Texas Instruments Incorporated or against the terms and conditions
// stipulated in the agreement under which this program has been supplied,
// and under no circumstances can it be used with non-TI connectivity device.
//
//*****************************************************************************

/**
 * @addtogroup sha1
 *
 * @{
 */

#include <inc/hw_common_reg.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_shamd5.h>
#include <inc/hw_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rom.h"
#include "rom_map.h"
#include "shamd5.h"
#include "interrupt.h"
#include "prcm.h"
#include "pin.h"
#include "utils.h"

//
// Flags to check that interrupts were successfully generated.
//
volatile bool g_bContextReadyFlag;
volatile bool g_bParthashReadyFlag;
volatile bool g_bInputReadyFlag;
volatile bool g_bOutputReadyFlag;

void SHAMD5IntHandler(void);
void GenerateHash(unsigned int uiConfig, unsigned char *puiKey1,
        unsigned char *puiData, unsigned char *puiResult,
        unsigned int uiDataLength);

//*****************************************************************************
//
//! SHAMD5IntHandler - Interrupt Handler which handles different interrupts from
//! different sources
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void SHAMD5IntHandler(void) {
    uint32_t ui32IntStatus;
    //
    // Read the SHA/MD5 masked interrupt status.
    //
    ui32IntStatus = MAP_SHAMD5IntStatus(SHAMD5_BASE, true);
    if (ui32IntStatus & SHAMD5_INT_CONTEXT_READY) {
        MAP_SHAMD5IntDisable(SHAMD5_BASE, SHAMD5_INT_CONTEXT_READY);
        g_bContextReadyFlag = true;

    }
    if (ui32IntStatus & SHAMD5_INT_PARTHASH_READY) {
        MAP_SHAMD5IntDisable(SHAMD5_BASE, SHAMD5_INT_PARTHASH_READY);
        g_bParthashReadyFlag = true;

    }
    if (ui32IntStatus & SHAMD5_INT_INPUT_READY) {
        MAP_SHAMD5IntDisable(SHAMD5_BASE, SHAMD5_INT_INPUT_READY);
        g_bInputReadyFlag = true;

    }
    if (ui32IntStatus & SHAMD5_INT_OUTPUT_READY) {
        MAP_SHAMD5IntDisable(SHAMD5_BASE, SHAMD5_INT_OUTPUT_READY);
        g_bOutputReadyFlag = true;

    }

}

//*****************************************************************************
//
//! GenerateHash - Generates the Hash value of the Plain Text
//!
//! \param uiConfig - Configuration Value
//! \param puiKey1 - Key Used
//! \param puiData - Plain Text used
//! \param puiResult - Hash Value Generated
//! \param uiDataLength - DataLength Used
//!
//! \return None
//
//*****************************************************************************

void GenerateHash(unsigned int uiConfig, unsigned char *puiKey1,
        unsigned char *puiData, unsigned char *puiResult,
        unsigned int uiDataLength) {

    //
    // Step1: Reset the Module
    // Step2: Enable Interrupts
    // Step3: Wait for Context Ready Inteerupt
    // Step4: Set the Configuration Parameters (Hash Algorithm)
    // Step5: Set Key depends on Algorithm
    // Step7: Start Hash Generation
    //
    MAP_PRCMPeripheralReset(PRCM_DTHE);
    //
    // Clear the flags
    //
    g_bContextReadyFlag = false;
    g_bInputReadyFlag = false;
    //
    // Enable interrupts.
    //
    MAP_SHAMD5IntEnable(SHAMD5_BASE, SHAMD5_INT_CONTEXT_READY |
    SHAMD5_INT_PARTHASH_READY |
    SHAMD5_INT_INPUT_READY |
    SHAMD5_INT_OUTPUT_READY);
    //
    // Wait for the context ready flag.
    //
    while (!g_bContextReadyFlag) {
    }
    //
    // Configure the SHA/MD5 module.
    //
    MAP_SHAMD5ConfigSet(SHAMD5_BASE, uiConfig);

    //
    // Perform the hashing operation
    //
    MAP_SHAMD5DataProcess(SHAMD5_BASE, puiData, strlen((const char *) puiData),
            puiResult);

}

int SHA1(unsigned char *puiInData, unsigned char *puiOutData) {
    //
    // Enable the module .
    //
    MAP_PRCMPeripheralClkEnable(PRCM_DTHE, PRCM_RUN_MODE_CLK);
    //
    // Enable interrupts.
    //
    MAP_SHAMD5IntRegister(SHAMD5_BASE, SHAMD5IntHandler);

    GenerateHash(SHAMD5_ALGO_SHA1, NULL, puiInData, puiOutData,
            strlen((const char *) puiInData));

    return 1;

}

/// @}
