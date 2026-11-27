/***************************************************************************//**
* \file cy_cryptolite_trng.h
* \version 2.80
*
* \brief
*  This file provides common constants and parameters
*  for the Cryptolite TRNG driver.
*
********************************************************************************
* Copyright 2022 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/
#if !defined (CY_CRYPTOLITE_TRNG_H)
#define CY_CRYPTOLITE_TRNG_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#if defined(__cplusplus)
extern "C" {
#endif

#include "cy_cryptolite_common.h"

#if (CRYPTOLITE_TRNG_PRESENT == 1)
#if defined(CY_CRYPTOLITE_CFG_TRNG_C)

/** \cond INTERNAL */
#define CY_CRYPTOLITE_TRNG_RO_ENABLE           (1U)
#define CY_CRYPTOLITE_TRNG_RO_DISABLE          (0U)
/** \endcond */

/** \cond INTERNAL */
typedef enum
{
    /** "Selection of the ring oscillator (RO) source: */
    CY_CRYPTOLITE_TRNG_SRC_RO11 = 0,  /**< "0": fixed RO 11 bit. */
    CY_CRYPTOLITE_TRNG_SRC_RO15,      /**< "1": fixed RO 15 bit. */
    CY_CRYPTOLITE_TRNG_SRC_GARO15,    /**< "2": fixed Galois RO 15 bit. */
    CY_CRYPTOLITE_TRNG_SRC_GARO31,    /**< "3": flexible Galois RO 31 bit. */
    CY_CRYPTOLITE_TRNG_SRC_FIRO15,    /**< "4": fixed Fibonacci RO 15 bit. */
    CY_CRYPTOLITE_TRNG_SRC_FIRO31     /**< "5": flexible Fibonacci RO 31 bit. */
} cy_en_cryptolite_trng_ro_sel_t;
/** \endcond */

/******************************************
 * Configuration structure
 ******************************************/
/** \cond INTERNAL */
typedef enum
{
    /** "Selection of the bitstream: */
    CY_CRYPTOLITE_TRMON_BS_DAS = 0, /**< "0": DAS bitstream. */
    CY_CRYPTOLITE_TRMON_BS_RED,     /**< "1": RED bitstream. */
    CY_CRYPTOLITE_TRMON_BS_TR,      /**< "2": TR bitstream.  */
    CY_CRYPTOLITE_TRMON_BS_UNDEF    /**< "3": Undefined.     */
} cy_en_cryptolite_trng_bs_sel_t;
/** \endcond */

/** The structure for storing the TRNG configuration.
*/
typedef struct
{
    /** \cond INTERNAL */
/**
     * "Specifies the clock divider that is used to sample oscillator data.
     * This clock divider is wrt. "clk_sys".
     * "0": sample clock is "clk_sys".
     * "1": sample clock is "clk_sys"/2.
     *
     * "255": sample clock is "clk_sys"/256. */
    uint8_t sampleClockDiv;
    /**
     * "Specifies the clock divider that is used to produce reduced bits.
     * "0": 1 reduced bit is produced for each sample.
     * "1": 1 reduced bit is produced for each 2 samples.
     *
     * "255": 1 reduced bit is produced for each 256 samples. */
    uint8_t reducedClockDiv;
    /**
     * Specifies an initialization delay: number of removed/dropped samples
     * before reduced bits are generated. This field should be programmed
     * in the range [1, 255]. After starting the oscillators,
     * at least the first 2 samples should be removed/dropped to clear the state
     * of internal synchronizers. In addition, it is advised to drop
     * at least the second 2 samples from the oscillators (to circumvent
     * the semi-predictable oscillator startup behavior).
     *
     * This result in the default field value of "3".
     * Field encoding is as follows:
     * "0": 1 sample is dropped.
     * "1": 2 samples are dropped.
     *
     * "255": 256 samples are dropped.
     *
     * The TR_INITIALIZED interrupt cause is set to '1', when
     * the initialization delay is passed. */
    uint8_t initDelay;
    /**
     * "Specifies if the "von Neumann corrector" is disabled or enabled:
     * '0': disabled.
     * '1': enabled.
     * The "von Neumann corrector" post-processes the reduced bits
     * to remove a '0' or '1' bias. The corrector operates on reduced bit pairs
     * ("oldest bit, newest bit"):
     * "00": no bit is produced.
     * "01": '0' bit is produced (oldest bit).
     * "10": '1' bit is produced (oldest bit).
     * "11": no bit is produced.
     * Note that the corrector produces bits at a random pace and at a frequency
     * that is 1/4 of the reduced bit frequency (reduced bits are processed
     * in pairs, and half of the pairs do NOT produce a bit). */
    bool    vnCorrectorEnable;
    /**
     * Specifies if TRNG adaptive proportion(AP) Test is enabled
     * When test fails INTR.TR_AP_DETECT is set to '1'. */
    bool    EnableAPTest;
    /**
     * Specifies if TRNG repetition count(RC) Test is enabled
     * When test fails INTR.TR_RC_DETECT is set to '1'. */
    bool    EnableRCTest;
    /**
     * Specifies if TRNG functionality is stopped on an adaptive proportion
     * test detection (when HW sets INTR.TR_AP_DETECT to '1'):
     * '0': Functionality is NOT stopped.
     * '1': Functionality is stopped (TR_CTL1 fields are set to '0' by HW).
     * valid only if EnableAPTest is set to true */
    bool    stopOnAPDetect;
    /**
     * Specifies if TRNG functionality is stopped on a repetition
     * count test detection (when HW sets INTR.TR_RC_DETECT to '1'):
     * '0': Functionality is NOT stopped.
     * '1': Functionality is stopped (TR_CTL1 fields are set to '0' by HW).
     * valid only if EnableRCTest is set to true */
    bool    stopOnRCDetect;
    /**
     * Polynomial for programmable Galois ring oscillator.
     * The polynomial is represented WITHOUT
     * the high order bit (this bit is always assumed '1').
     * The polynomial should be aligned such that the more
     * significant bits (bit 30 and down) contain the polynomial
     * and the less significant bits (bit 0 and up) contain padding '0's. */
    uint32_t garo31Poly;
    /**
     * Polynomial for programmable Fibonacci ring oscillator.
     * The polynomial is represented WITHOUT
     * the high order bit (this bit is always assumed '1').
     * The polynomial should be aligned such that the more
     * significant bits (bit 30 and down) contain the polynomial
     * and the less significant bits (bit 0 and up) contain padding '0's. */
    uint32_t firo31Poly;
    /**
     * Selection of the bitstream:
     * "0": DAS bitstream.
     * "1": RED bitstream.
     * "2": TR bitstream.
     * "3": Undefined.  */
    cy_en_cryptolite_trng_bs_sel_t monBitStreamSelect;
    /**
     * Cutoff count (legal range is [1, 255]):
     * "0": Illegal.
     * "1": 1 repetition.
     * ..
     * "255": 255 repetitions. */
    uint8_t monCutoffCount8;
    /**
     * Cutoff count (legal range is [1, 65535]).
     * "0": Illegal.
     * "1": 1 occurrence.
     * ...
     * "65535": 65535 occurrences. */
    uint16_t monCutoffCount16;
    /**
     * Window size (minus 1) :
     * "0": 1 bit.
     * ...
     * "65535": 65536 bits. */
    uint16_t monWindowSize;
    /** \endcond */
} cy_stc_cryptolite_trng_config_t;

/**
* \addtogroup group_cryptolite_lld_rng_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng
****************************************************************************//**
*
* Generates a True Random Number.
*
* \param base
* The pointer to the cryptolite instance.
*
* \param randomNum
* The pointer to a generated true random number.
*
* \return
* \ref cy_en_cryptolite_status_t
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Trng
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Trng(CRYPTOLITE_Type *base, uint32_t *randomNum);

/** \} group_cryptolite_lld_rng_functions */



/** \cond INTERNAL */

/* Macros for conditions used by CY_ASSERT calls */
#define CY_CRYPTOLITE_IS_SMPL_CLK_DIV_VALID(clkDiv)     ((clkDiv) <= 255U)
#define CY_CRYPTOLITE_IS_RED_CLK_DIV_VALID(clkDiv)      ((clkDiv) <= 255U)
#define CY_CRYPTOLITE_IS_INIT_DELAY_VALID(delay)        ((delay) <= 255U)
#define CY_CRYPTOLITE_IS_VONN_CORR_VALID(val)           ((uint32_t)(val) <= 1U)
#define CY_CRYPTOLITE_IS_AP_DETECT_VALID(val)           ((uint32_t)(val) <= 1U)
#define CY_CRYPTOLITE_IS_RC_DETECT_VALID(val)           ((uint32_t)(val) <= 1U)

#define CY_CRYPTOLITE_IS_RO11_EN_VALID(val)             ((uint32_t)(val) <= 1U)
#define CY_CRYPTOLITE_IS_RO15_EN_VALID(val)             ((uint32_t)(val) <= 1U)
#define CY_CRYPTOLITE_IS_GARO15_EN_VALID(val)           ((uint32_t)(val) <= 1U)
#define CY_CRYPTOLITE_IS_GARO31_EN_VALID(val)           ((uint32_t)(val) <= 1U)
#define CY_CRYPTOLITE_IS_FIRO15_EN_VALID(val)           ((uint32_t)(val) <= 1U)
#define CY_CRYPTOLITE_IS_FIRO31_EN_VALID(val)           ((uint32_t)(val) <= 1U)

#define CY_CRYPTOLITE_IS_GARO_POLY_VALID(val)           ((val) <= 0x7FFFFFFFUL)
#define CY_CRYPTOLITE_IS_FIRO_POLY_VALID(val)           ((val) <= 0x7FFFFFFFUL)

#define CY_CRYPTOLITE_IS_BS_SELECT_VALID(val)           (((val) == CY_CRYPTOLITE_TRMON_BS_DAS) || \
                                                        ((val) == CY_CRYPTOLITE_TRMON_BS_RED) || \
                                                        ((val) == CY_CRYPTOLITE_TRMON_BS_TR))

#define CY_CRYPTOLITE_IS_CCOUNT8_VALID(val)             ((val) <= 255U)
#define CY_CRYPTOLITE_IS_CCOUNT16_VALID(val)            ((val) <= 65535U)
#define CY_CRYPTOLITE_IS_WINSIZE_VALID(val)             ((val) <= 65535U)

#define CY_CRYPTOLITE_TRNG_AP_DETECT_ERROR_BIT          (0U)
#define CY_CRYPTOLITE_TRNG_RC_DETECT_ERROR_BIT          (1U)
#define CY_CRYPTOLITE_TRNG_AP_DETECT_ERROR              (1U << 0U)
#define CY_CRYPTOLITE_TRNG_RC_DETECT_ERROR              (1U << 1U)

#define CY_CRYPTOLITE_TRNG_RO_ALL_MASK                  (uint32_t)(CRYPTOLITE_TRNG_CTL1_RO11_EN_Msk | \
                                                        CRYPTOLITE_TRNG_CTL1_RO15_EN_Msk | \
                                                        CRYPTOLITE_TRNG_CTL1_GARO15_EN_Msk | CRYPTOLITE_TRNG_CTL1_GARO31_EN_Msk | \
                                                        CRYPTOLITE_TRNG_CTL1_FIRO15_EN_Msk | CRYPTOLITE_TRNG_CTL1_FIRO31_EN_Msk)
/** FW sets this field to '1' to enable the ring oscillator with 11 inverters. */
#define CY_CRYPTOLITE_DEF_TRNG_RO11_EN                  (1UL)

/** FW sets this field to '1' to enable the ring oscillator with 15 inverters. */
#define CY_CRYPTOLITE_DEF_TRNG_RO15_EN                  (1UL)

/**
* FW sets this field to '1' to enable the fixed Galois ring oscillator
* with 15 inverters.
*/
#define CY_CRYPTOLITE_DEF_TRNG_GARO15_EN                (1UL)

/**
* FW sets this field to '1' to enable the programmable Galois ring oscillator with up
* to 31 inverters. The TR_GARO_CTL register specifies the programmable polynomial.
*/
#define CY_CRYPTOLITE_DEF_TRNG_GARO31_EN                (1UL)

/** FW sets this field to '1' to enable the fixed Fibonacci ring oscillator with 15 inverters. */
#define CY_CRYPTOLITE_DEF_TRNG_FIRO15_EN                (1UL)

/**
* FW sets this field to '1' to enable the programmable Fibonacci ring oscillator
* with up to 31 inverters. The TR_FIRO_CTL register specifies the programmable polynomial.
*/
#define CY_CRYPTOLITE_DEF_TRNG_FIRO31_EN                (1UL)

extern const cy_stc_cryptolite_trng_config_t            cy_cryptolite_trngDefaultConfig;

__STATIC_INLINE bool Cy_Cryptolite_Trng_IsInitialized(CRYPTOLITE_Type *base);
__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_GetRoStatus(CRYPTOLITE_Type *base, cy_en_cryptolite_trng_ro_sel_t roSelector);
__STATIC_INLINE void Cy_Cryptolite_Trng_SetRoStatus(CRYPTOLITE_Type *base,
                                                     cy_en_cryptolite_trng_ro_sel_t roSelector, uint8_t roStatus);
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetData(CRYPTOLITE_Type *base);
__STATIC_INLINE void Cy_Cryptolite_Trng_SetGaroPoly(CRYPTOLITE_Type *base, uint32_t poly);
__STATIC_INLINE void Cy_Cryptolite_Trng_SetFiroPoly(CRYPTOLITE_Type *base, uint32_t poly);
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetGaroPoly(CRYPTOLITE_Type *base);
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetFiroPoly(CRYPTOLITE_Type *base);

__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_MonGetHealthStatus(CRYPTOLITE_Type *base);

__STATIC_INLINE uint8_t  Cy_Cryptolite_Trng_MonGetRcRepCount(CRYPTOLITE_Type *base);
__STATIC_INLINE uint16_t Cy_Cryptolite_Trng_MonGetApOccCount(CRYPTOLITE_Type *base);
__STATIC_INLINE uint16_t Cy_Cryptolite_Trng_MonGetApWindowIndex(CRYPTOLITE_Type *base);

__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_MonGetRcCurrentBit(CRYPTOLITE_Type *base);
__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_MonGetApCurrentBit(CRYPTOLITE_Type *base);

__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Trng_MonSetBSSelector(CRYPTOLITE_Type *base, cy_en_cryptolite_trng_bs_sel_t bitStreamSelector);
__STATIC_INLINE cy_en_cryptolite_trng_bs_sel_t Cy_Cryptolite_Trng_MonGetBSSelector(CRYPTOLITE_Type *base);

__STATIC_INLINE void Cy_Cryptolite_Trng_MonEnableApTest(CRYPTOLITE_Type *base);
__STATIC_INLINE void Cy_Cryptolite_Trng_MonDisableApTest(CRYPTOLITE_Type *base);
__STATIC_INLINE void Cy_Cryptolite_Trng_MonEnableRcTest(CRYPTOLITE_Type *base);
__STATIC_INLINE void Cy_Cryptolite_Trng_MonDisableRcTest(CRYPTOLITE_Type *base);

__STATIC_INLINE void Cy_Cryptolite_Trng_MonSetRcCC8(CRYPTOLITE_Type *base, uint8_t ccCount);
__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_MonGetRcCC8(CRYPTOLITE_Type *base);

__STATIC_INLINE void Cy_Cryptolite_Trng_MonSetApCC16(CRYPTOLITE_Type *base, uint16_t ccCount);
__STATIC_INLINE uint16_t Cy_Cryptolite_Trng_MonGetApCC16(CRYPTOLITE_Type *base);

__STATIC_INLINE void Cy_Cryptolite_Trng_MonSetApWinSize(CRYPTOLITE_Type *base, uint16_t windowSize);
__STATIC_INLINE uint16_t Cy_Cryptolite_Trng_MonGetApWinSize(CRYPTOLITE_Type *base);

__STATIC_INLINE void Cy_Cryptolite_Trng_WaitForComplete(CRYPTOLITE_Type *base);
__STATIC_INLINE void Cy_Cryptolite_Trng_WaitForInit(CRYPTOLITE_Type *base);

/** \endcond */


/**
* \addtogroup group_cryptolite_lld_rng_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_Init
****************************************************************************//**
*
* Initialize the TRNG hardware submodule
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param config
* The pointer to the configuration structure. If NULL default config is used.
*
* \return
* \ref cy_en_cryptolite_status_t
*
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Trng
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Trng_Init(CRYPTOLITE_Type *base, cy_stc_cryptolite_trng_config_t *config);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_Enable
****************************************************************************//**
*
* Starts a random number generation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The error / status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Trng_enable_disable
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Trng_Enable(CRYPTOLITE_Type *base);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_Disable
****************************************************************************//**
*
* Stops the random number generation.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The error / status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Trng_enable_disable
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Trng_Disable(CRYPTOLITE_Type *base);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_ReadData
****************************************************************************//**
*
* Reads in blocking mode a generated random number.
*
* \note
* Call this API only after Cy_Cryptolite_Trng_Enable() is successful.
*
* \param base
* The pointer to the CRYPTO instance.
*
* \param randomData
* The pointer to a generated true random number.
*
* \return
* The error / status code. See \ref cy_en_cryptolite_status_t.
*
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Trng
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Trng_ReadData(CRYPTOLITE_Type *base, uint32_t *randomData);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_DeInit
****************************************************************************//**
*
* Clears all TRNG registers by set to hardware default values.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The error / status code. See \ref cy_en_cryptolite_status_t.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Trng
*******************************************************************************/
cy_en_cryptolite_status_t Cy_Cryptolite_Trng_DeInit(CRYPTOLITE_Type *base);


/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_IsEnabled
****************************************************************************//**
*
* Checks if Trng is enabled.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The 'true' if Trng enabled 'false' otherwise.
*
* \funcusage
* \snippet cryptolite/snippet/main.c snippet_Cy_Cryptolite_Trng_enable_disable
*******************************************************************************/
bool Cy_Cryptolite_Trng_IsEnabled(CRYPTOLITE_Type *base);

/** \} group_cryptolite_lld_rng_functions */

__STATIC_INLINE void Cy_Cryptolite_Trng_SetInterruptMask(CRYPTOLITE_Type *base, uint32_t interrupts);
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetInterruptMask(CRYPTOLITE_Type const *base);
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetInterruptStatusMasked(CRYPTOLITE_Type const *base);
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetInterruptStatus(CRYPTOLITE_Type *base);
/*
 * API Implementations
 */


/******************************************************************************
* Function Name: Cy_Cryptolite_Trng_SetInterruptMask
***************************************************************************//**
*
* Masks / unmasks multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param interrupts
* Mask bits. See definitions above.
*
******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_SetInterruptMask(CRYPTOLITE_Type *base, uint32_t interrupts)
{
    REG_CRYPTOLITE_INTR_TRNG_MASK(base) = interrupts;
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Trng_GetInterruptMask
***************************************************************************//**
*
* Reports mask / unmask multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* Mask bits. See definitions above.
*
******************************************************************************/
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetInterruptMask(CRYPTOLITE_Type const *base)
{
    return (REG_CRYPTOLITE_INTR_TRNG_MASK(base));
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Trng_GetInterruptStatusMasked
***************************************************************************//**
*
* Reports states of multiple enabled interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* Source bits. See definitions above.
*
*****************************************************************************/
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetInterruptStatusMasked(CRYPTOLITE_Type const *base)
{
    return (REG_CRYPTOLITE_INTR_TRNG_MASKED(base));
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Trng_GetInterruptStatus
***************************************************************************//**
*
* Reports states of multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* Source bits. See definitions above.
*
******************************************************************************/
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetInterruptStatus(CRYPTOLITE_Type *base)
{
    return (REG_CRYPTOLITE_INTR_TRNG(base));
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Trng_SetInterrupt
***************************************************************************//**
*
* Sets one of more interrupt sources
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param interrupts
* Source bit(s)
*
******************************************************************************/
__STATIC_INLINE void  Cy_Cryptolite_Trng_SetInterrupt(CRYPTOLITE_Type *base, uint32_t interrupts)
{
    REG_CRYPTOLITE_INTR_TRNG_SET(base) = interrupts;
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Trng_ClearInterrupt
***************************************************************************//**
*
* Clears multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param interrupts
* Source bit(s). See definitions above.
*
******************************************************************************/
__STATIC_INLINE void  Cy_Cryptolite_Trng_ClearInterrupt(CRYPTOLITE_Type *base, uint32_t interrupts)
{
    REG_CRYPTOLITE_INTR_TRNG(base) = interrupts;
    /* This dummy reading is necessary here. It provides a guarantee that interrupt is cleared at returning from this function. */
    (void) REG_CRYPTOLITE_INTR_TRNG(base);
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Error_SetInterruptMask
***************************************************************************//**
*
* Masks / unmasks multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param interrupts
* Mask bits. See definitions above.
*
******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Error_SetInterruptMask(CRYPTOLITE_Type *base, uint32_t interrupts)
{
    REG_CRYPTOLITE_INTR_ERROR_MASK(base) = interrupts;
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Error_GetInterruptMask
***************************************************************************//**
*
* Reports mask / unmask multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* Mask bits. See definitions above.
*
******************************************************************************/
__STATIC_INLINE uint32_t Cy_Cryptolite_Error_GetInterruptMask(CRYPTOLITE_Type const *base)
{
    return (REG_CRYPTOLITE_INTR_ERROR_MASK(base));
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Error_GetInterruptStatusMasked
***************************************************************************//**
*
* Reports states of multiple enabled interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* Source bits. See definitions above.
*
*****************************************************************************/
__STATIC_INLINE uint32_t Cy_Cryptolite_Error_GetInterruptStatusMasked(CRYPTOLITE_Type const *base)
{
    return (REG_CRYPTOLITE_INTR_ERROR_MASKED(base));
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Error_GetInterruptStatus
***************************************************************************//**
*
* Reports states of multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* Source bits. See definitions above.
*
******************************************************************************/
__STATIC_INLINE uint32_t Cy_Cryptolite_Error_GetInterruptStatus(CRYPTOLITE_Type *base)
{
    return (REG_CRYPTOLITE_INTR_ERROR(base));
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Error_SetInterrupt
***************************************************************************//**
*
* Sets one of more interrupt sources
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param interrupts
* Source bit(s)
*
******************************************************************************/
__STATIC_INLINE void  Cy_Cryptolite_Error_SetInterrupt(CRYPTOLITE_Type *base, uint32_t interrupts)
{
    REG_CRYPTOLITE_INTR_ERROR_SET(base) = interrupts;
}

/******************************************************************************
* Function Name: Cy_Cryptolite_Error_ClearInterrupt
***************************************************************************//**
*
* Clears multiple interrupt sources.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param interrupts
* Source bit(s). See definitions above.
*
******************************************************************************/
__STATIC_INLINE void  Cy_Cryptolite_Error_ClearInterrupt(CRYPTOLITE_Type *base, uint32_t interrupts)
{
    REG_CRYPTOLITE_INTR_ERROR(base) = interrupts;
    /* This dummy reading is necessary here. It provides a guarantee that interrupt is cleared at returning from this function. */
    (void) REG_CRYPTOLITE_INTR_ERROR(base);
}


/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_IsInitialized
****************************************************************************//**
*
* Checks for TRNG hardware is initialized.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* true when TRNG hardware is initialized, otherwise false.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_Cryptolite_Trng_IsInitialized(CRYPTOLITE_Type *base)
{
    return ((Cy_Cryptolite_Trng_GetInterruptStatus(base) & CRYPTOLITE_INTR_TRNG_INITIALIZED_Msk) != 0U);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_WaitForInit
****************************************************************************//**
*
* Waits for TRNG initialization is complete.
*
* \param base
* The pointer to the CRYPTO instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_WaitForInit(CRYPTOLITE_Type *base)
{
    uint32_t status;
    /* Wait until the TRNG init is complete */
    do
    {
        status = (Cy_Cryptolite_Trng_GetInterruptStatus(base) & CRYPTOLITE_INTR_TRNG_INITIALIZED_Msk);
    }
    while (status == 0U);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_WaitForComplete
****************************************************************************//**
*
* Waits for TRNG random generation is complete.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \note
* After this call the Health Monitor status should be checked to ensure that
* random data is healthy.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_WaitForComplete(CRYPTOLITE_Type *base)
{
    uint32_t status;
    /* Wait until the TRNG random generation is complete */
    do
    {
        status = Cy_Cryptolite_Trng_GetInterruptStatus(base) & (CRYPTOLITE_INTR_TRNG_DATA_AVAILABLE_Msk  |
                                                            CRYPTOLITE_INTR_TRNG_AP_DETECT_Msk |
                                                            CRYPTOLITE_INTR_TRNG_RC_DETECT_Msk);
    }
    while (status == 0U);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_GetRoStatus
****************************************************************************//**
*
* Reads the ring oscillator enable status.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param roSelector
* The ring oscillator selector (see \ref cy_en_cryptolite_trng_ro_sel_t).
*
* \return
* The ring oscillator enabled status (0 or 1).
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_GetRoStatus(CRYPTOLITE_Type *base, cy_en_cryptolite_trng_ro_sel_t roSelector)
{
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8','Intentional typecast to wider 32bit.');
    return (REG_CRYPTOLITE_TRNG_CTL1(base) & (uint32_t)(1U << ((uint32_t)roSelector))) != 0U ? 1U : 0U;
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_SetRoStatus
****************************************************************************//**
*
* Sets the ring oscillator enable status.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param roSelector
* The ring oscillator selector (see \ref cy_en_cryptolite_trng_ro_sel_t).
*
* \param roStatus
* The new ring oscillator status value (0 or 1).
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_SetRoStatus(CRYPTOLITE_Type *base,
                                                        cy_en_cryptolite_trng_ro_sel_t roSelector, uint8_t roStatus)
{
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.6','enum cy_en_cryptolite_trng_ro_sel_t to uint32_t conversion intentional.');
    uint32_t roMask = 1U << (uint32_t)roSelector;
    uint32_t roData = (REG_CRYPTOLITE_TRNG_CTL1(base) & ~roMask) | (((uint32_t)roStatus != 0U) ? roMask : 0U);
    REG_CRYPTOLITE_TRNG_CTL1(base) = roData;
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_GetData
****************************************************************************//**
*
* Reads the generated random number in non-blocking mode.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The generated random number.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetData(CRYPTOLITE_Type *base)
{
    return REG_CRYPTOLITE_TRNG_RESULT(base);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_SetGaroPoly
****************************************************************************//**
*
* Sets the polynomial value for the Galois ring oscillator.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param poly
* The polynomial for the programmable Galois ring oscillator.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_SetGaroPoly(CRYPTOLITE_Type *base, uint32_t poly)
{
    REG_CRYPTOLITE_TRNG_GARO_CTL(base) = poly & CRYPTOLITE_TRNG_GARO_CTL_POLYNOMIAL_Msk;
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_SetFiroPoly
****************************************************************************//**
*
* Sets the polynomial value for the Fibonacci ring oscillator.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param poly
* The polynomial for the programmable Fibonacci ring oscillator.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_SetFiroPoly(CRYPTOLITE_Type *base, uint32_t poly)
{
    REG_CRYPTOLITE_TRNG_FIRO_CTL(base) = poly & CRYPTOLITE_TRNG_FIRO_CTL_POLYNOMIAL_Msk;
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_GetGaroPoly
****************************************************************************//**
*
* Reads the polynomial value of the Galois ring oscillator.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The polynomial of the programmable Galois ring oscillator.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetGaroPoly(CRYPTOLITE_Type *base)
{
    return REG_CRYPTOLITE_TRNG_GARO_CTL(base);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_GetFiroPoly
****************************************************************************//**
*
* Reads the polynomial value of the Fibonacci ring oscillator.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The polynomial of the programmable Fibonacci ring oscillator.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_Cryptolite_Trng_GetFiroPoly(CRYPTOLITE_Type *base)
{
    return REG_CRYPTOLITE_TRNG_FIRO_CTL(base);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetHealthStatus
****************************************************************************//**
*
* Returns the health status of the random number health monitor in two bits.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The health status:
*  - ZERO  - if data is healthy
*  - bit 0 - Adaptive Proportion (AP) error is detected
*  - bit 1 - Repetition Count (RC) error is detected
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_MonGetHealthStatus(CRYPTOLITE_Type *base)
{
    return (uint8_t)((REG_CRYPTOLITE_INTR_TRNG(base) \
                      & (CRYPTOLITE_INTR_TRNG_AP_DETECT_Msk | CRYPTOLITE_INTR_TRNG_RC_DETECT_Msk)) \
                      >> CRYPTOLITE_INTR_TRNG_AP_DETECT_Pos);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonClearHealthStatus
****************************************************************************//**
*
* Clears the health status of the random number health monitor
* in the interrupts register
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_MonClearHealthStatus(CRYPTOLITE_Type *base)
{
    Cy_Cryptolite_Trng_ClearInterrupt(base, CRYPTOLITE_INTR_TRNG_AP_DETECT_Msk | CRYPTOLITE_INTR_TRNG_RC_DETECT_Msk);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetRcRepCount
****************************************************************************//**
*
* Reads the current number of repetitions from the repetition counter
* test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The number of repetitions of the current active bit counter.
*
*******************************************************************************/
__STATIC_INLINE uint8_t  Cy_Cryptolite_Trng_MonGetRcRepCount(CRYPTOLITE_Type *base)
{
    return ((uint8_t)_FLD2VAL(CRYPTOLITE_TRNG_MON_RC_STATUS1_REP_COUNT, REG_CRYPTOLITE_TRNG_MON_RC_STATUS1(base)));
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetApOccCount
****************************************************************************//**
*
* Reads the current Cutoff count number from the adaptive proportion
* test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The Cutoff count number of the adaptive proportion test.
*
*******************************************************************************/
__STATIC_INLINE uint16_t Cy_Cryptolite_Trng_MonGetApOccCount(CRYPTOLITE_Type *base)
{
    return ((uint16_t)_FLD2VAL(CRYPTOLITE_TRNG_MON_AP_STATUS1_OCC_COUNT, REG_CRYPTOLITE_TRNG_MON_AP_STATUS1(base)));
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetApWindowIndex
****************************************************************************//**
*
* Reads the current window index from the adaptive proportion test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The window index of the adaptive proportion test.
*
*******************************************************************************/
__STATIC_INLINE uint16_t Cy_Cryptolite_Trng_MonGetApWindowIndex(CRYPTOLITE_Type *base)
{
    return ((uint16_t)_FLD2VAL(CRYPTOLITE_TRNG_MON_AP_STATUS1_WINDOW_INDEX, REG_CRYPTOLITE_TRNG_MON_AP_STATUS1(base)));
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetRcCurrentBit
****************************************************************************//**
*
* Reads the active bit value from the adaptive proportion test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The active bit value.
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_MonGetRcCurrentBit(CRYPTOLITE_Type *base)
{
    return ((uint8_t)_FLD2VAL(CRYPTOLITE_TRNG_MON_RC_STATUS0_BIT, REG_CRYPTOLITE_TRNG_MON_RC_STATUS0(base)));
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetApCurrentBit
****************************************************************************//**
*
* Reads the active bit value from the repetition counter test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The active bit value.
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_MonGetApCurrentBit(CRYPTOLITE_Type *base)
{
    return ((uint8_t)_FLD2VAL(CRYPTOLITE_TRNG_MON_AP_STATUS0_BIT, REG_CRYPTOLITE_TRNG_MON_AP_STATUS0(base)));
}


/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonSetBSSelector
****************************************************************************//**
*
* Sets the BitStream selector value of the TRNG monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param bitStreamSelector
* The BitStream selector value (see \ref cy_en_cryptolite_trng_bs_sel_t).
*
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_status_t Cy_Cryptolite_Trng_MonSetBSSelector(CRYPTOLITE_Type *base,
                                                                           cy_en_cryptolite_trng_bs_sel_t bitStreamSelector)
{
    cy_en_cryptolite_status_t status = CY_CRYPTOLITE_SUCCESS;

    CY_ASSERT_L3(CY_CRYPTOLITE_IS_BS_SELECT_VALID(bitStreamSelector));

    CY_REG32_CLR_SET(REG_CRYPTOLITE_TRNG_MON_CTL(base), CRYPTOLITE_TRNG_MON_CTL_BITSTREAM_SEL, bitStreamSelector);

    return status;
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetBSSelector
****************************************************************************//**
*
* Reads the BitStream selector value of the TRNG monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The BitStream selector value (see \ref cy_en_cryptolite_trng_bs_sel_t).
*
*******************************************************************************/
__STATIC_INLINE cy_en_cryptolite_trng_bs_sel_t Cy_Cryptolite_Trng_MonGetBSSelector(CRYPTOLITE_Type *base)
{
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8','Intentional typecast to cy_en_cryptolite_trng_bs_sel_t enum.');
    return ((cy_en_cryptolite_trng_bs_sel_t)_FLD2VAL(CRYPTOLITE_TRNG_MON_CTL_BITSTREAM_SEL, REG_CRYPTOLITE_TRNG_MON_CTL(base)));
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonEnableApTest
****************************************************************************//**
*
* Enables the Adaptive proportion (AP) test.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_MonEnableApTest(CRYPTOLITE_Type *base)
{
    CY_REG32_CLR_SET(REG_CRYPTOLITE_TRNG_MON_CTL(base), CRYPTOLITE_TRNG_MON_CTL_AP, 1U);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonDisableApTest
****************************************************************************//**
*
* Disables the Adaptive proportion (AP) test.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_MonDisableApTest(CRYPTOLITE_Type *base)
{
    CY_REG32_CLR_SET(REG_CRYPTOLITE_TRNG_MON_CTL(base), CRYPTOLITE_TRNG_MON_CTL_AP, 0U);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonEnableRcTest
****************************************************************************//**
*
* Enables the Repetition count (RC) test.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_MonEnableRcTest(CRYPTOLITE_Type *base)
{
    CY_REG32_CLR_SET(REG_CRYPTOLITE_TRNG_MON_CTL(base), CRYPTOLITE_TRNG_MON_CTL_RC, 1U);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonDisableRcTest
****************************************************************************//**
*
* Disables the Repetition count (RC) test.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_MonDisableRcTest(CRYPTOLITE_Type *base)
{
    CY_REG32_CLR_SET(REG_CRYPTOLITE_TRNG_MON_CTL(base), CRYPTOLITE_TRNG_MON_CTL_RC, 0U);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonSetRcCC8
****************************************************************************//**
*
* Sets the Cutoff counter value of the repetition counter test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param ccCount
* The new Cutoff counter value.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_MonSetRcCC8(CRYPTOLITE_Type *base, uint8_t ccCount)
{
    REG_CRYPTOLITE_TRNG_MON_RC_CTL(base) = (uint32_t)_VAL2FLD(CRYPTOLITE_TRNG_MON_RC_CTL_CUTOFF_COUNT8, ccCount);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetRcCC8
****************************************************************************//**
*
* Reads the Cutoff counter value of the repetition counter test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The Cutoff counter value.
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_Cryptolite_Trng_MonGetRcCC8(CRYPTOLITE_Type *base)
{
    return ((uint8_t)_FLD2VAL(CRYPTOLITE_TRNG_MON_RC_CTL_CUTOFF_COUNT8, REG_CRYPTOLITE_TRNG_MON_RC_CTL(base)));
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonSetApCC16
****************************************************************************//**
*
* Sets the Cutoff counter value of the adaptive proportion test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param ccCount
* The new Cutoff counter value.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_MonSetApCC16(CRYPTOLITE_Type *base, uint16_t ccCount)
{
    CY_REG32_CLR_SET(REG_CRYPTOLITE_TRNG_MON_AP_CTL(base), CRYPTOLITE_TRNG_MON_AP_CTL_CUTOFF_COUNT16, ccCount);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetApCC16
****************************************************************************//**
*
* Reads the Cutoff counter value of the adaptive proportion test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The Cutoff counter value.
*
*******************************************************************************/
__STATIC_INLINE uint16_t Cy_Cryptolite_Trng_MonGetApCC16(CRYPTOLITE_Type *base)
{
    return ((uint16_t)_FLD2VAL(CRYPTOLITE_TRNG_MON_AP_CTL_CUTOFF_COUNT16, REG_CRYPTOLITE_TRNG_MON_AP_CTL(base)));
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonSetApWinSize
****************************************************************************//**
*
* Sets the window size value of the adaptive proportion test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \param windowSize
* The new windows size value.
*
*******************************************************************************/
__STATIC_INLINE void Cy_Cryptolite_Trng_MonSetApWinSize(CRYPTOLITE_Type *base, uint16_t windowSize)
{
    CY_REG32_CLR_SET(REG_CRYPTOLITE_TRNG_MON_AP_CTL(base), CRYPTOLITE_TRNG_MON_AP_CTL_WINDOW_SIZE, windowSize);
}

/*******************************************************************************
* Function Name: Cy_Cryptolite_Trng_MonGetApWinSize
****************************************************************************//**
*
* Reads the window size value of the adaptive proportion test monitor.
*
* \param base
* The pointer to the CRYPTOLITE instance.
*
* \return
* The windows size value.
*
*******************************************************************************/
__STATIC_INLINE uint16_t Cy_Cryptolite_Trng_MonGetApWinSize(CRYPTOLITE_Type *base)
{
    return ((uint16_t)_FLD2VAL(CRYPTOLITE_TRNG_MON_AP_CTL_WINDOW_SIZE, REG_CRYPTOLITE_TRNG_MON_AP_CTL(base)));
}


#endif /* #if (CY_CRYPTOLITE_CFG_TRNG_C)*/
#endif /* #if CRYPTOLITE_TRNG_PRESENT*/

/** \} group_cryptolite_lld_trng_functions */
#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_TRNG_H) */

/* [] END OF FILE */
