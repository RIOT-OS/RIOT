/***************************************************************************//**
* \file cy_cordic.h
* \version 2.10
*
*  This file provides constants and parameter values for
*  the CORDIC driver.
*
********************************************************************************
* \copyright
* Copyright 2023 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#if !defined(CY_CORDIC_H)
#define CY_CORDIC_H


/**
* \addtogroup group_cordic
* \{
* The CORDIC driver provides an easy method to access the CORDIC IP block registers
* and provides simple functionalities for calculating the Trigonometric functions,
* hyperbolic functions and Park Transform.
*
* The functions and other declarations used in this driver are in cy_cordic.h.
* You can include cy_pdl.h to get access to all functions and declarations in the PDL.
*
* \section group_cordic_configuration Configuration Considerations
*
* The CORDIC driver provides the simplest way to calculate the various functions.
* No configuration is needed.
*
* \section group_cordic_more_information More Information
*
* Refer to the technical reference manual (TRM) and the device datasheet.
*
* \section group_cordic_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td >2.10</td>
*     <td>Macro definition of CY_CORDIC_ID changed.</td>
*     <td>Bug Fixes.</td>
*   </tr>
*   <tr>
*     <td>2.0</td>
*     <td>A few APIs are turned to inline APIs. And result parameter is moved
*         as return parameter instead, for a few APIs. </td>
*     <td>To improve performance.</td>
*   </tr>
*   <tr>
*     <td>1.0</td>
*     <td>Initial version</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_cordic_macros Macros
* \defgroup group_cordic_data_structures Data Structures
* \defgroup group_cordic_enumerated_types Enumerated Types
* \defgroup group_cordic_functions Functions
*/

#include "cy_device.h"

#if defined (CY_IP_MXCORDIC)

#include <stddef.h>
#include "cy_syslib.h"

#if defined(__cplusplus)
extern "C" {
#endif

CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 10.8', 8, \
'Intentional typecast from uint32_t to int32_t.')
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 10.1', 10, \
'Intentional use of unsigned type with the operator >>.')

/**
* \addtogroup group_cordic_macros
* \{
*/

/** The driver major version */
#define CY_CORDIC_DRV_VERSION_MAJOR       2U

/** The driver minor version */
#define CY_CORDIC_DRV_VERSION_MINOR       10U

/** The CORDIC driver ID */
#define CY_CORDIC_ID               CY_PDL_DRV_ID(0x63U)

/** Reciprocal of Circular gain in CY_CORDIC_CIRCULAR_GAIN format ((2^23)/1.646760258121) */
#define CY_CORDIC_CIRCULAR_GAIN       (0x4DBA76U)

/** Reciprocal of Hyperbolic gain in CY_CORDIC_HYPERBOLIC_GAIN format
((2^22)/0.828159360960) */
#define CY_CORDIC_HYPERBOLIC_GAIN     (0x4D47A1U)

/** Scaling factor : ((2^15) / 0.828159360960), where 0.828159360960 is the hyperbolic gain */
#define CY_CORDIC_Q15_SCALING_FACTOR    (39568UL)

/** Is operating mode valid */
#define CY_CORDIC_IS_OPERATING_MODE_VALID(mode)      ((CY_CORDIC_OPERATING_MODE_CIRCULAR  == (mode)) || \
                                                       (CY_CORDIC_OPERATING_MODE_HYPERBOLIC == (mode)))

/** Is  rotvec mode valid */
#define CY_CORDIC_IS_ROTVEC_MODE_VALID(mode)         ((CY_CORDIC_ROTVEC_MODE_VECTORING  == (mode)) || \
                                                       (CY_CORDIC_ROTVEC_MODE_ROTATION == (mode)))

/** In circular function, |angle_in_radians| is less than or equal to integer value representing 1.74 radians;
The 1.74 angle in radians is mapped to the integer value(4646111) using: ((angle * ((2^23)/pi)) << 8)
*/
#define CY_CORDIC_IS_CIRCULAR_ROTATION_ANGLE_VALID(angle)              (((-(4646111)) <= (angle >> 8)) && \
                                                                       ((angle >> 8) <= (4646111)))

/** Angle range for Tangent operation [(-2^23) / 2, (2^23 - 1) / 2] */
#define CY_CORDIC_IS_TAN_ANGLE_VALID(angle)              (((-(1 << 23) / 2) <= (angle >> 8)) && \
                                                           ((angle >> 8) <= (((1 << 23) - 1) / 2)))

/** In hyperbolic function, |angle_in_radians| is less than or equal to integer value representing 1.11 radians;
The 1.11 angle in radians is mapped to the integer value(2963898) using: ((angle * ((2^23)/pi)) << 8)
*/
#define CY_CORDIC_IS_HYPERBOLIC_ROTATION_ANGLE_VALID(angle)              (((-(2963898)) <= (angle >> 8)) && \
                                                                       ((angle >> 8) <= (2963898)))


/** Is input within the range [(-2^23), (2^23 - 1)] for vectoring mode operations */
#define CY_CORDIC_IS_INPUT_VALID(x)              (((-(1 << 23)) <= (x)) && \
                                                       ((x) <= ((1 << 23) - 1)))

/** Number of cordic iterations */
#define CY_CORDIC_NUMBER_OF_ITERATIONS         (7U)

/** \cond INTERNAL */

#define CY_IS_BASE_VALID(base)                         (NULL != base)

#define CY_CORDIC_IS_INPUT_NON_ZERO(x)                 (x != 0)

#define CY_CORDIC_IS_INTR_VALID(intr, mask)            ( 0UL == ((intr) & ((uint32_t) ~(mask))) )

#define CY_CORDIC_INTR_MASK                            (MXCORDIC_INTR_MASK_CDEOC_Msk | MXCORDIC_INTR_MASK_CDERR_Msk)

#define CY_CORDIC_INTR_END_OF_CALCULATION  MXCORDIC_INTR_CDEOC_Msk

#define CY_CORDIC_INTR_ERROR_EVENT MXCORDIC_INTR_CDERR_Msk

/* X and Y are divided by MPS value 2 in the Cy_CORDIC_ParkTransformNB to avoid overflow.
X and Y are divided by 4 using CY_CORDIC_MPS_CORRECTION in the Cy_CORDIC_GetParkResult to refactor
the results */
#define CY_CORDIC_MPS_CORRECTION_FACTOR         (4)

/** \endcond */

/**
* \defgroup group_cordic_macros_callback_events CORDIC Callback Events
* Macros to check CORDIC events passed by \ref cy_cb_cordic_handle_events_t callback.
* Each event is encoded in a separate bit and therefore it is possible to
* notify about multiple events.
* \{
*/
/**
* Indicates the end of calculation event.
*/
#define CY_CORDIC_END_OF_CALCULATION_EVENT     (0x00000001UL)

/**
* Indicates the error event.
*/
#define CY_CORDIC_ERROR_EVENT                  (0x00000002UL)

/** \} group_cordic_macros_callback_events */

/** \} group_cordic_macros */


/*******************************************************************************
*                              Type Definitions
*******************************************************************************/
/**
* \addtogroup group_cordic_data_structures
* \{
*/

/**
 * This typedef is used for Input and Output Data representation in blocking & non-blocking functions.
 * CY_CORDIC_Q31_t => 1 Signed bit, 0 Integer bits, 31 fraction bits.
 */
typedef int32_t CY_CORDIC_Q31_t;

/**
 * This typedef is used for Input Data representation in blocking & non-blocking functions.
 * CY_CORDIC_8Q23_t => 1 Signed bit, 8 Integer bits, 23 fraction bits.
 */
typedef int32_t CY_CORDIC_8Q23_t;

/**
 * This typedef is used for Output Data representation in blocking & non-blocking functions.
 * CY_CORDIC_20Q11_t => 1 Signed bit, 20 Integer bits, 11 fraction bits.
 */
typedef int32_t CY_CORDIC_20Q11_t;

/**
 * This typedef is used for Output Data representation in blocking & non-blocking functions.
 * CY_CORDIC_1Q30_t => 1 Signed bit, 1 Integer bits, 30 fraction bits.
 */
typedef int32_t CY_CORDIC_1Q30_t;


/**
* Provides the typedef for the callback function called in the
* \ref Cy_CORDIC_Interrupt to notify the user about occurrences of
* \ref group_cordic_macros_callback_events.
*/
typedef void (* cy_cb_cordic_handle_events_t)(uint32_t event);


/** CORDIC context structure.
* All fields for the context structure are internal. Firmware never reads or
* writes these values. Firmware allocates the structure and provides the
* address of the structure to the driver in function calls. Firmware must
* ensure that the defined instance of this structure remains in scope
* while the drive is in use.
*/
typedef struct cy_stc_cordic_context
{
    /**
    * The pointer to an event callback that is called when any of
    * \ref group_cordic_macros_callback_events occurs.
    */
    cy_cb_cordic_handle_events_t cbEvents;
}cy_stc_cordic_context_t;

/** Park Transform results */
typedef struct cy_stc_cordic_parkTransform_result
{
    int32_t   parkTransformIq; /**< Iq */
    int32_t   parkTransformId; /**< Id */
}
cy_stc_cordic_parkTransform_result_t;

/** \} group_cordic_data_structures */


/***************************************
*       Enumerations
***************************************/
/**
* \addtogroup group_cordic_enumerated_types
* \{
*/
/** This enum has the return values of the CORDIC driver */
typedef enum
{
    CY_CORDIC_SUCCESS       = 0x00U,                                     /**< Operation completed successfully */
    CY_CORDIC_BAD_PARAM     = CY_CORDIC_ID | CY_PDL_STATUS_ERROR | 0x01u, /**< One or more invalid parameters */
} cy_en_cordic_status_t;


/** CORDIC the Circular/Hyperbolic/Linear Operating Modes */
typedef enum
{
    CY_CORDIC_OPERATING_MODE_CIRCULAR       = 0x1U,             /**< Circular Operating Mode */
    CY_CORDIC_OPERATING_MODE_HYPERBOLIC     = 0x3U,             /**< Hyperbolic Operating Mode */
}cy_en_cordic_operating_mode_t;

/** Rotation Vectoring Modes */
typedef enum
{
    CY_CORDIC_ROTVEC_MODE_VECTORING      = 0x0U,             /**< Vectoring Mode */
    CY_CORDIC_ROTVEC_MODE_ROTATION       = 0x1U,             /**< Rotation Mode */
}cy_en_cordic_rotvec_mode_t;

/** X and Y MPS - Magnitude Pre-scalar */
typedef enum
{
    CY_CORDIC_DIVIDE_BY_1      = 0x0U,             /**< MPS - Divide X and Y by 1 */
    CY_CORDIC_DIVIDE_BY_2      = 0x1U,             /**< MPS - Divide X and Y by 2 */
    CY_CORDIC_DIVIDE_BY_4      = 0x2U,             /**< MPS - Divide X and Y by 4 */
}cy_en_cordic_mps_t;

/** \} group_cordic_enumerated_types */


/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_cordic_functions
* \{
*/


/*******************************************************************************
* Function Name: Cy_CORDIC_Enable
****************************************************************************//**
*
* Enables CORDIC IP.
*
* \param base
* The pointer to the CORDIC instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_Enable(MXCORDIC_Type *base);

/*******************************************************************************
* Function Name: Cy_CORDIC_Disable
****************************************************************************//**
*
* Disables CORDIC IP.
*
* \param base
* The pointer to the CORDIC instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_Disable(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_IsBusy
****************************************************************************//**
*
* Checks if there is any computation is currently in progress.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* true: calculation in progress
* false: no calculation in progress
*
*******************************************************************************/
__STATIC_INLINE bool Cy_CORDIC_IsBusy(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_CosNB
****************************************************************************//**
*
* Computes the Cosine for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
* \note
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In circular function, |angle_in_radians| is less than or equal to integer value representing 1.74 radians
*
* \note
* This is a non-blocking function.
* \ref Cy_CORDIC_GetCosResult should be called to obtain the result.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_CosNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_SinNB
****************************************************************************//**
*
* Computes the Sine for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
* \note
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In circular function, |angle_in_radians| is less than or equal to integer value representing 1.74 radians
*
* \note
* This is a non-blocking function.
* \ref Cy_CORDIC_GetSinResult should be called to obtain the result.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_SinNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_TanNB
****************************************************************************//**
*
* Computes the Tangent for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
* \note
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In circular function, |angle_in_radians| is less than or equal to integer value representing 1.74 radians
*
* \note
* This is a non-blocking function.
* \ref Cy_CORDIC_GetTanResult should be called to obtain the result.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_TanNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_ArcTanNB
****************************************************************************//**
*
* Computes the Arc Tangent for the given value (y/x).
*
* \param base
* The pointer to the CORDIC instance.
*
* \param y
* The numerator value.
*
* \param x
* The denominator value.
*
* \note
*  - The input parameters x and y should be left shifted by 8 bits: x = x << 8; y = y << 8;
*  - This is a non-blocking function.
*  - \ref Cy_CORDIC_GetArcTanResult should be called to obtain the result.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_ArcTanNB(MXCORDIC_Type *base, CY_CORDIC_8Q23_t x, CY_CORDIC_8Q23_t y);


/*******************************************************************************
* Function Name: Cy_CORDIC_CoshNB
****************************************************************************//**
*
* Computes the Hyperbolic Cosine for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
* \note
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In hyperbolic function, |angle_in_radians| is less than or equal to integer value representing 1.11 radians
*
* \note
* This is a non-blocking function.
* \ref Cy_CORDIC_GetCoshResult should be called to obtain the result.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_CoshNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_SinNB
****************************************************************************//**
*
* Computes the Hyperbolic Sine for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
* \note
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In hyperbolic function, |angle_in_radians| is less than or equal to integer value representing 1.11 radians
*
* \note
* This is a non-blocking function.
* \ref Cy_CORDIC_GetSinhResult should be called to obtain the result.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_SinhNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_TanNB
****************************************************************************//**
*
* Computes the Hyperbolic Tangent for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
* \note
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In hyperbolic function, |angle_in_radians| is less than or equal to integer value representing 1.11 radians
*
* \note
* This is a non-blocking function.
* \ref Cy_CORDIC_GetTanhResult should be called to obtain the result.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_TanhNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_ArcTanhNB
****************************************************************************//**
*
* Computes the Hyperbolic Arc Tangent for the given value (y/x).
*
* \param base
* The pointer to the CORDIC instance.
*
* \param y
* The numerator value.

* \param x
* The denominator value.
*
* \note
* The input parameters x and y should be left shifted by 8 bits: x = x << 8; y = y << 8;
* This is a non-blocking function.
* \ref Cy_CORDIC_GetArcTanhResult should be called to obtain the result.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_ArcTanhNB(MXCORDIC_Type *base, CY_CORDIC_8Q23_t x, CY_CORDIC_8Q23_t y);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetCosResult
****************************************************************************//**
*
* Returns the result of Cosine operation.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* \ref CY_CORDIC_Q31_t
* \note
* The result should be scaled: (((result) >> 8) / (2^23))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_GetCosResult(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetSinResult
****************************************************************************//**
*
* Returns the result of Sine operation.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* \ref CY_CORDIC_Q31_t
* \note
* The result should be scaled: (((result) >> 8) / (2^23))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_GetSinResult(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetTanResult
****************************************************************************//**
*
* Returns the result of Tangent operation.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* \ref CY_CORDIC_20Q11_t
* \note
* The result should be scaled: ((result) / (2^11))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_20Q11_t Cy_CORDIC_GetTanResult(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetArcTanResult
****************************************************************************//**
*
* Returns the result of Arc Tangent operation.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* \ref CY_CORDIC_Q31_t
* \note
* The result should be scaled: ((result >> 8) * (pi / (2^23)))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_GetArcTanResult(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetCoshResult
****************************************************************************//**
*
* Returns the result of Hyperbolic Cosine operation.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* \ref CY_CORDIC_1Q30_t
* \note
* The result should be scaled: (((result) >> 8) / (2^22))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_1Q30_t Cy_CORDIC_GetCoshResult(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetSinhResult
****************************************************************************//**
*
* Returns the result of Hyperbolic Sine operation.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* \ref CY_CORDIC_1Q30_t
* \note
* The result should be scaled: (((result) >> 8) / (2^22))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_1Q30_t Cy_CORDIC_GetSinhResult(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetTanhResult
****************************************************************************//**
*
* Returns the result of Hyperbolic Tangent operation.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* \ref CY_CORDIC_20Q11_t
* \note
* The result should be scaled: ((result) / (2^11))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_20Q11_t Cy_CORDIC_GetTanhResult(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetArcTanhResult
****************************************************************************//**
*
* Returns the result of Hyperbolic Arc Tangent operation.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* \ref CY_CORDIC_1Q30_t
* \note
* The result should be scaled: ((result >> 8) * (pi / (2^23)))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_GetArcTanhResult(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_ParkTransformNB
****************************************************************************//**
*
* Computes the Park Transform.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param Vref_AngleQ31
* The rotor angle value in radians.

* \param I_Alpha_1Q31
* The current I_Alpha value.
*
* \param I_Beta_1Q31
* The current I_Beta value.
*
* \note
*  - This is a non-blocking function.
*  - The input parameters should be scaled as: ((I_Alpha_1Q31 * 0x800000) << 8); ((I_Beta_1Q31 * 0x800000) << 8);
*    ((angle_in_radians * ((2^23) / pi)) << 8)
*  - \ref Cy_CORDIC_GetParkResult should be called to obtain the result.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_ParkTransformNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t Vref_AngleQ31, CY_CORDIC_Q31_t I_Alpha_1Q31, CY_CORDIC_Q31_t I_Beta_1Q31);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetParkResult
****************************************************************************//**
*
* Returns Park Transform results.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param ptr
* The pointer to the computed values of Park Transform.
* \note
* The result should be scaled as: ((float32_t)ptr.x /(float32_t)(2^23 - 1))
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_GetParkResult(MXCORDIC_Type *base, cy_stc_cordic_parkTransform_result_t *ptr);


/*******************************************************************************
* Function Name: Cy_CORDIC_ClearZ
****************************************************************************//**
*
* Clears the CORDIC Z Data register.
*
* \param base
* The pointer to the CORDIC instance.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_ClearZ(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_SetMode
****************************************************************************//**
*
* Configures the operating modes.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param opmode
* Specifies the Circular/Hyperbolic/Linear mode.

* \param rotvec
* Specifies the Rotation/Vectoring mode.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_SetMode(MXCORDIC_Type *base, cy_en_cordic_operating_mode_t opmode, cy_en_cordic_rotvec_mode_t rotvec);


/*******************************************************************************
* Function Name: Cy_CORDIC_Cos
****************************************************************************//**
*
* Computes the Cosine for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
*
* \return
* The computed Cosine value.
* \ref CY_CORDIC_Q31_t
*
* \note
*  - This is a blocking function.
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In circular function, |angle_in_radians| is less than or equal to integer value representing 1.74 radians
*  - The result should be scaled- as: (((result) >> 8) / (2^23)
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_Cos(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_Sin
****************************************************************************//**
*
* Computes the Sine for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
*
* \return
* The computed Sine value.
* \ref CY_CORDIC_Q31_t
*
* \note
*  - This is a blocking function.
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In circular function, |angle_in_radians| is less than or equal to integer value representing 1.74 radians
*  - The result should be scaled as: (((result) >> 8) / (2^23)
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_Sin(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_Tan
****************************************************************************//**
*
* Computes the Tangent for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
*
* \return
* The computed Tangent value.
* \ref CY_CORDIC_20Q11_t
*
* \note
*  - This is a blocking function.
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In circular function, |angle_in_radians| is less than or equal to integer value representing 1.74 radians
*  - The result should be scaled as: ((result) / (2^11))
*
*******************************************************************************/
CY_CORDIC_20Q11_t Cy_CORDIC_Tan(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_ArcTan
****************************************************************************//**
*
* Computes the Arc Tangent for the given value (y/x).
*
* \param base
* The pointer to the CORDIC instance.
*
* \param y
* The numerator value.
*
* \param x
* The denominator value.
*
* \return
* The computed Arc Tangent value.
* \ref CY_CORDIC_Q31_t
*
* \note
*  - This is a blocking function.
*  - The input parameters x and y should be left shifted by 8 bits: x = x << 8; y = y << 8;
*  - The result should be scaled as: ((result >> 8) * (pi / (2^23)))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_ArcTan(MXCORDIC_Type *base, CY_CORDIC_8Q23_t x, CY_CORDIC_8Q23_t y);


/*******************************************************************************
* Function Name: Cy_CORDIC_Cosh
****************************************************************************//**
*
* Computes the Hyperbolic Cosine for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
*
* \return
* The computed Hyperbolic Cosine value.
* \ref CY_CORDIC_1Q30_t
*
* \note
*  - This is a blocking function.
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In hyperbolic function, |angle_in_radians| is less than or equal to integer value representing 1.11 radians
*  - The result should be scaled as: (((result) >> 8) / (2^22)
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_1Q30_t Cy_CORDIC_Cosh(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_Sinh
****************************************************************************//**
*
* Computes the Hyperbolic Hyperbolic Sine for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
*
* \return
* The computed Hyperbolic Sine value.
* \ref CY_CORDIC_1Q30_t
*
* \note
*  - This is a blocking function.
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In hyperbolic function, |angle_in_radians| is less than or equal to integer value representing 1.11 radians
*  - The result should be scaled as: (((result) >> 8) / (2^22)
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_1Q30_t Cy_CORDIC_Sinh(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_Cosh
****************************************************************************//**
*
* Computes the Hyperbolic Tangent for the given angle in radians.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param angle_in_radians
* The angle in radians.
*
* \return
* The computed Hyperbolic Tangent value.
* \ref CY_CORDIC_20Q11_t
*
* \note
*  - This is a blocking function.
*  - The input angle should be mapped to the integer value using:
*    angle_in_radians = ((angle * ((2^23)/pi)) << 8)
*  - In hyperbolic function, |angle_in_radians| is less than or equal to integer value representing 1.11 radians
*  - The result should be scaled as: ((result) / (2^11))
*
*******************************************************************************/
CY_CORDIC_20Q11_t Cy_CORDIC_Tanh(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians);


/*******************************************************************************
* Function Name: Cy_CORDIC_ArcTanh
****************************************************************************//**
*
* Computes the Hyperbolic Arc Tangent for the given value (y/x).
*
* \param base
* The pointer to the CORDIC instance.
*
* \param y
* The numerator value.
*
* \param x
* The denominator value.
*
* \return
* The computed Hyperbolic Arc Tangent value.
* \ref CY_CORDIC_Q31_t
*
* \note
* - The input parameters x and y should be left shifted by 8 bits: x = x << 8; y = y << 8;
* - This is a blocking function.
* - The result should be scaled as: ((result >> 8) * (pi / (2^23)))
*
*******************************************************************************/
__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_ArcTanh(MXCORDIC_Type *base, CY_CORDIC_8Q23_t x, CY_CORDIC_8Q23_t y);


/*******************************************************************************
* Function Name: Cy_CORDIC_Sqrt
****************************************************************************//**
*
* Computes the Square root of the given value.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param x
* The input value.
*
* \return
* Square root of the given value.
*
* \note
*  - This is a blocking function.
*  - The input parameter x of type float should be converted to Q31: (x * 0x80000000) + 0.5F
*  - The result should be scaled from  Q31 to float : (*result / 0x80000000)
*
*******************************************************************************/
uint32_t Cy_CORDIC_Sqrt(MXCORDIC_Type *base, uint32_t x);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetInterruptStatus
****************************************************************************//**
*
* Returns the interrupt cause register.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* The current status of the interrupt sources. Each constant is a bit field
* value. The value returned may have multiple bits set to indicate the
* current status.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_CORDIC_GetInterruptStatus(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_ClearInterrupt
****************************************************************************//**
*
* Clears the interrupt sources in the interrupt cause register.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param interruptMask
* The interrupt sources to be cleared.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_ClearInterrupt(MXCORDIC_Type *base, uint32_t interruptMask);


/*******************************************************************************
* Function Name: Cy_CORDIC_SetInterrupt
****************************************************************************//**
*
* Sets the interrupt sources in the interrupt set register.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param interruptMask
* The interrupt sources to set in the interrupt set register.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_SetInterrupt(MXCORDIC_Type *base, uint32_t interruptMask);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetInterruptMask
****************************************************************************//**
*
* Returns the interrupt mask register. This register specifies which
* bits from the interrupt cause register can trigger an interrupt event.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* Enabled interrupt sources.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_CORDIC_GetInterruptMask(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_SetInterruptMask
****************************************************************************//**
*
* Writes interrupt mask register.
* This register specifies which bits from the interrupt cause register
* can trigger an interrupt event.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param interruptMask
* The interrupt sources to set in the interrupt set register.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_SetInterruptMask(MXCORDIC_Type *base, uint32_t interruptMask);


/*******************************************************************************
* Function Name: Cy_CORDIC_GetInterruptStatusMasked
****************************************************************************//**
*
* Returns the interrupt masked register. This register contains a
* logical AND of corresponding bits from the interrupt cause and mask
* registers.
* This function is intended to be used in the interrupt service routine to
* identify which of enabled interrupt sources caused the interrupt
* event.
*
* \param base
* The pointer to the CORDIC instance.
*
* \return
* The current status of enabled interrupt sources.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_CORDIC_GetInterruptStatusMasked(MXCORDIC_Type *base);


/*******************************************************************************
* Function Name: Cy_CORDIC_Interrupt
****************************************************************************//**
*
*  CORDIC Interrupt Service Routine.
*
* \param *base
* The pointer to a CAN FD instance.
*
* \param context
* The pointer to the context structure \ref cy_stc_cordic_context_t allocated
* by the user. The structure is used during the CORDIC operation for internal
* configuration. The user must not modify anything in this structure.
*
*******************************************************************************/
void Cy_CORDIC_Interrupt(MXCORDIC_Type *base, cy_stc_cordic_context_t *context);


/*******************************************************************************
* Function Name: Cy_CORDIC_RegisterCallback
****************************************************************************//**
*
* Registers a callback function that notifies that
* \ref group_cordic_macros_callback_events occurred in the
* \ref Cy_CORDIC_Interrupt.
*
* \param base
* The pointer to the CORDIC instance.
*
* \param callback
* The pointer to a callback function.
* See \ref cy_cb_cordic_handle_events_t for the function prototype.
*
* \param context
* The pointer to context structure \ref cy_stc_cordic_context_t allocated by
* the user. The structure is used during the CORDIC operation for internal
* configuration. The user should not modify anything in this structure.
*
* \note
* To remove the callback, pass NULL as the pointer to a callback function.
*
*******************************************************************************/
__STATIC_INLINE void Cy_CORDIC_RegisterCallback(MXCORDIC_Type *base, cy_cb_cordic_handle_events_t callback, cy_stc_cordic_context_t *context);


__STATIC_INLINE void Cy_CORDIC_Enable(MXCORDIC_Type *base)
{
    MXCORDIC_CTL(base) = MXCORDIC_CTL_ENABLED_Msk;
}


__STATIC_INLINE void Cy_CORDIC_Disable(MXCORDIC_Type *base)
{
    MXCORDIC_CTL(base) &= (uint32_t) ~MXCORDIC_CTL_ENABLED_Msk;
}


__STATIC_INLINE bool Cy_CORDIC_IsBusy(MXCORDIC_Type *base)
{
    return _FLD2BOOL(MXCORDIC_STAT_BSY, MXCORDIC_STAT(base));
}


__STATIC_INLINE void Cy_CORDIC_CosNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_CIRCULAR_ROTATION_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_CIRCULAR) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)  |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_CIRCULAR_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;
}


__STATIC_INLINE void Cy_CORDIC_SinNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_CIRCULAR_ROTATION_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_CIRCULAR) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)  |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_CIRCULAR_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;
}


__STATIC_INLINE void Cy_CORDIC_TanNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_TAN_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_CIRCULAR) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)  |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_CIRCULAR_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;
}


__STATIC_INLINE void Cy_CORDIC_ArcTanNB(MXCORDIC_Type *base, CY_CORDIC_8Q23_t x, CY_CORDIC_8Q23_t y)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L1(CY_CORDIC_IS_INPUT_NON_ZERO(x));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_CIRCULAR) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_VECTORING) |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = (uint32_t)x;
    MXCORDIC_CORDY(base) = (uint32_t)y;
    MXCORDIC_CORDZ(base) = 0UL;
}


__STATIC_INLINE void Cy_CORDIC_CoshNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_HYPERBOLIC_ROTATION_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_HYPERBOLIC) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)    |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_HYPERBOLIC_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;
}


__STATIC_INLINE void Cy_CORDIC_SinhNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_HYPERBOLIC_ROTATION_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_HYPERBOLIC) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)    |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_HYPERBOLIC_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;
}


__STATIC_INLINE void Cy_CORDIC_TanhNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_HYPERBOLIC_ROTATION_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_HYPERBOLIC) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)    |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_HYPERBOLIC_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;
}


/* TBD: What limits should be checked for x and y */
__STATIC_INLINE void Cy_CORDIC_ArcTanhNB(MXCORDIC_Type *base, CY_CORDIC_8Q23_t x, CY_CORDIC_8Q23_t y)
{
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L1(CY_CORDIC_IS_INPUT_NON_ZERO(x));
    CY_ASSERT_L2(CY_CORDIC_IS_INPUT_VALID(x));
    CY_ASSERT_L2(CY_CORDIC_IS_INPUT_VALID(y));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_HYPERBOLIC) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_VECTORING)   |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = (uint32_t)x;
    MXCORDIC_CORDY(base) = (uint32_t)y;
    MXCORDIC_CORDZ(base) = 0UL;
}


__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_GetCosResult(MXCORDIC_Type *base)
{
    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRX(base));
}


__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_GetSinResult(MXCORDIC_Type *base)
{
    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRY(base));
}


__STATIC_INLINE CY_CORDIC_20Q11_t Cy_CORDIC_GetTanResult(MXCORDIC_Type *base)
{
    return (CY_CORDIC_20Q11_t)((((CY_CORDIC_Q31_t)MXCORDIC_CORRY(base)) >> ((int32_t)MXCORDIC_CORRY_RESULT_Pos)) /
                (((CY_CORDIC_Q31_t)MXCORDIC_CORRX(base)) >> ((int32_t)MXCORDIC_CORRX_RESULT_Pos) >> 11UL));
}


__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_GetArcTanResult(MXCORDIC_Type *base)
{
    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRZ(base));
}


__STATIC_INLINE CY_CORDIC_1Q30_t Cy_CORDIC_GetCoshResult(MXCORDIC_Type *base)
{
    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRX(base));

}


__STATIC_INLINE CY_CORDIC_1Q30_t Cy_CORDIC_GetSinhResult(MXCORDIC_Type *base)
{
    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRY(base));
}


__STATIC_INLINE CY_CORDIC_20Q11_t Cy_CORDIC_GetTanhResult(MXCORDIC_Type *base)
{
    return (CY_CORDIC_20Q11_t)((((CY_CORDIC_Q31_t)MXCORDIC_CORRY(base)) >> ((int32_t)MXCORDIC_CORRY_RESULT_Pos)) /
             (((CY_CORDIC_Q31_t)MXCORDIC_CORRX(base)) >> ((int32_t)MXCORDIC_CORRX_RESULT_Pos) >> 11UL));
}


__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_GetArcTanhResult(MXCORDIC_Type *base)
{
    return ((int32_t)MXCORDIC_CORRZ(base));
}


__STATIC_INLINE void Cy_CORDIC_ParkTransformNB(MXCORDIC_Type *base, CY_CORDIC_Q31_t Vref_AngleQ31, CY_CORDIC_Q31_t I_Alpha_1Q31, CY_CORDIC_Q31_t I_Beta_1Q31)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_CIRCULAR) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)  |
                         _VAL2FLD(MXCORDIC_CON_MPS, CY_CORDIC_DIVIDE_BY_4)  |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);

    MXCORDIC_CORDX(base) = (uint32_t) I_Beta_1Q31;
    MXCORDIC_CORDY(base) = (uint32_t) I_Alpha_1Q31;
    MXCORDIC_CORDZ(base) = (uint32_t)Vref_AngleQ31;
}


CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 10.3', 2, \'Intentional typecast from uint32_t to int32_t.')
__STATIC_INLINE void Cy_CORDIC_GetParkResult(MXCORDIC_Type *base, cy_stc_cordic_parkTransform_result_t *ptr)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L1(CY_IS_BASE_VALID(ptr));

    ptr->parkTransformIq = CY_CORDIC_MPS_CORRECTION_FACTOR * (((int32_t)MXCORDIC_CORRX(base)) >> MXCORDIC_CORRX_RESULT_Pos);
    ptr->parkTransformId = CY_CORDIC_MPS_CORRECTION_FACTOR * (((int32_t)MXCORDIC_CORRY(base)) >> MXCORDIC_CORRY_RESULT_Pos);
}
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 10.3')


__STATIC_INLINE void Cy_CORDIC_ClearZ(MXCORDIC_Type *base)
{
    MXCORDIC_CORDZ(base) = 0UL;
}


__STATIC_INLINE void Cy_CORDIC_SetMode(MXCORDIC_Type *base, cy_en_cordic_operating_mode_t opmode, cy_en_cordic_rotvec_mode_t rotvec)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L3(CY_CORDIC_IS_OPERATING_MODE_VALID(opmode));
    CY_ASSERT_L3(CY_CORDIC_IS_ROTVEC_MODE_VALID(rotvec));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, opmode)    |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, rotvec)  |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
}


__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_Cos(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_CIRCULAR_ROTATION_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_CIRCULAR) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)  |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_CIRCULAR_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;

    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRX(base));
}


__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_Sin(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_CIRCULAR_ROTATION_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_CIRCULAR) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)  |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_CIRCULAR_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;

    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRY(base));
}


__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_ArcTan(MXCORDIC_Type *base, CY_CORDIC_8Q23_t x, CY_CORDIC_8Q23_t y)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L1(CY_CORDIC_IS_INPUT_NON_ZERO(x));
    CY_ASSERT_L2(CY_CORDIC_IS_INPUT_VALID(x));
    CY_ASSERT_L2(CY_CORDIC_IS_INPUT_VALID(y));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_CIRCULAR) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_VECTORING) |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = (uint32_t)x;
    MXCORDIC_CORDY(base) = (uint32_t)y;
    MXCORDIC_CORDZ(base) = 0UL;

    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRZ(base));
}


__STATIC_INLINE CY_CORDIC_1Q30_t Cy_CORDIC_Cosh(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_HYPERBOLIC_ROTATION_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_HYPERBOLIC) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)    |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_HYPERBOLIC_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;

    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRX(base));
}


__STATIC_INLINE CY_CORDIC_1Q30_t Cy_CORDIC_Sinh(MXCORDIC_Type *base, CY_CORDIC_Q31_t angle_in_radians)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L2(CY_CORDIC_IS_HYPERBOLIC_ROTATION_ANGLE_VALID(angle_in_radians));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_HYPERBOLIC) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_ROTATION)    |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = _VAL2FLD(MXCORDIC_CORDX_DATA, CY_CORDIC_HYPERBOLIC_GAIN);
    MXCORDIC_CORDY(base) = 0UL;
    MXCORDIC_CORDZ(base) = (uint32_t)angle_in_radians;

    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRY(base));
}


__STATIC_INLINE CY_CORDIC_Q31_t Cy_CORDIC_ArcTanh(MXCORDIC_Type *base, CY_CORDIC_8Q23_t x, CY_CORDIC_8Q23_t y)
{
    /* Input parameters verification */
    CY_ASSERT_L1(CY_IS_BASE_VALID(base));
    CY_ASSERT_L1(CY_CORDIC_IS_INPUT_NON_ZERO(x));
    CY_ASSERT_L2(CY_CORDIC_IS_INPUT_VALID(x));
    CY_ASSERT_L2(CY_CORDIC_IS_INPUT_VALID(y));

    MXCORDIC_CON(base) = _VAL2FLD(MXCORDIC_CON_MODE, CY_CORDIC_OPERATING_MODE_HYPERBOLIC) |
                         _VAL2FLD(MXCORDIC_CON_ROTVEC, CY_CORDIC_ROTVEC_MODE_VECTORING)   |
                         _VAL2FLD(MXCORDIC_CON_N_ITER, CY_CORDIC_NUMBER_OF_ITERATIONS);
    MXCORDIC_CORDX(base) = (uint32_t)x;
    MXCORDIC_CORDY(base) = (uint32_t)y;
    MXCORDIC_CORDZ(base) = 0UL;

    return ((CY_CORDIC_Q31_t)MXCORDIC_CORRZ(base));
}


__STATIC_INLINE uint32_t Cy_CORDIC_GetInterruptStatus(MXCORDIC_Type *base)
{
    return (MXCORDIC_INTR(base));
}


__STATIC_INLINE void Cy_CORDIC_ClearInterrupt(MXCORDIC_Type *base, uint32_t interruptMask)
{
    CY_ASSERT_L2(CY_CORDIC_IS_INTR_VALID(interruptMask, CY_CORDIC_INTR_MASK));

    MXCORDIC_INTR(base) = interruptMask;
}


__STATIC_INLINE void Cy_CORDIC_SetInterrupt(MXCORDIC_Type *base, uint32_t interruptMask)
{
    CY_ASSERT_L2(CY_CORDIC_IS_INTR_VALID(interruptMask, CY_CORDIC_INTR_MASK));

    MXCORDIC_INTR_SET(base) = interruptMask;
}


__STATIC_INLINE uint32_t Cy_CORDIC_GetInterruptMask(MXCORDIC_Type *base)
{
    return (MXCORDIC_INTR_MASK(base));
}


__STATIC_INLINE void Cy_CORDIC_SetInterruptMask(MXCORDIC_Type *base, uint32_t interruptMask)
{
    CY_ASSERT_L2(CY_CORDIC_IS_INTR_VALID(interruptMask, CY_CORDIC_INTR_MASK));

    MXCORDIC_INTR_MASK(base) = interruptMask;
}


__STATIC_INLINE uint32_t Cy_CORDIC_GetInterruptStatusMasked(MXCORDIC_Type *base)
{
    return (MXCORDIC_INTR_MASKED(base));
}

__STATIC_INLINE void Cy_CORDIC_RegisterCallback(MXCORDIC_Type *base, cy_cb_cordic_handle_events_t callback, cy_stc_cordic_context_t *context)
{
    /* Suppress a compiler warning about unused variables */
    (void) base;

    context->cbEvents = callback;
}
/** \} group_cordic_functions */

CY_MISRA_BLOCK_END('MISRA C-2012 Rule 10.8')
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 10.1')

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXCORDIC */

#endif /* CY_CORDIC_H */

/** \} group_cordic */

/* [] END OF FILE */
