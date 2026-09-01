/***************************************************************************//**
* \file cy_hppass_csg.h
* \version 1.30.1
*
* Header file for the Comparator Slope Generator (CSG) subsystem of the High Power Programmable Analog Sub-System.
*
********************************************************************************
* \copyright
* Copyright (c) (2024), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
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

/**
* \addtogroup group_hppass_csg
* \{
* This driver provides API functions to configure the Comparator Slope Generator
* within High Power Programmable Analog Sub-System.
*
* The CSG is a flexible block which compares a 10-bit DAC value with
* a selected analog input signal or compares two analog inputs. The output
* of this block is the 1-bit digital compare value that can be used for
* controlling the behavior of the TCPWM in a motor control, power conversion
* and other applications.
*
* The CSG block contains 5 CSG slices (for PSOC C3 devices), each of which can be
* configured and used independently. Each slice contains a DAC and a comparator with
* the dedicated control logic.
* The internal structure and external HW interface of the CSG slice are shown below.
*
* \image html hppass_csg_slice.png width=60%
*
* \section group_hppass_csg_dac DAC
*
* The user has various options to control the DAC data:
* * Single value buffered mode 
* * Two value hysteretic buffered mode
* * Slope mode (rising, falling, triangular) with programmable slope rate
* * Look-Up Table (LUT) mode with programmable data (128 samples).
*
* DAC data update can be triggered by either a HW or FW trigger.
* Available HW DAC trigger sources:
* * Any of the 8 HPPASS triggers
* * DAC divider
* * CSG comparator local output
* * AC trigger.
*
* \section group_hppass_csg_comp Comparator
*
* The Comparator can be configured to compare a signal from the dedicated analog
* input or from the DAC output to another selected analog input.
*
* The CSG comparator output signal has the following post-processing options:
* * Direct output/inverted output
* * Gated output by the blanking trigger
* * Edge detection for the interrupt generation
* * Trigger generation for the Autonomous Controller.
* 
* \section group_hppass_csg_config CSG Configuration
*
* To configure the CSG, the driver uses a configuration structure of type
* \ref cy_stc_hppass_csg_t that must be predefined. This structure holds the
* pointer to the array of CSG slices configuration structures
* \ref cy_stc_hppass_slice_t, as well the pointer to the array of the CSG LUT
* configuration structures \ref cy_stc_hppass_lut_t. Also, this structure
* contains the DAC output selector field \ref cy_en_hppass_dac_out_t, which
* routes the selected DAC output to the HPPASS SAR input for debugging purposes.
*
* \note The total number of LUTs is 2 for the PSOC C3 devices, but the same LUT
* can be used by multiple CSG slices. LUT 0 can be used by CSG slices 0, 1, and 2;
* LUT 1 by slices 3 and 4.
*
* Each instance of the \ref cy_stc_hppass_slice_t contains the comparator
* configuration structure \ref cy_stc_hppass_comp_t and the DAC configuration
* structure \ref cy_stc_hppass_dac_t.
*
* Refer to the Technical Reference Manual for detailed information.
*
* \section group_hppass_csg_code_snippets Code Snippets
* \subsection group_hppass_csg_pwm_slice CSG Analog to PWM conversion
* 
* The example below shows how to configure the CSG slice to generate PWM signal
* on P7_0 with the duty cycle proportional to the analog input voltage
* on the AN_A0 pin.
*
* - Configure the CSG slice 0 to:
*   - Generate a repeating rising slope from 0 to VDDA on DAC 0
*   - Use AN_A0 pin as the positive comparator input and the DAC output as the negative comparator input.
*
* \snippet hppass_csg_pwm/main.c SNIPPET_HPPASS_CSG_PWM_SLICE_CFG
*
* \note The cy_stc_hppass_slice_t::dac::period field is calculated to update the DAC
*       at the frequency of 10240 kHz for CSG clock = 120 MHz. For the the 1024 rising slope values (0 to 1023),
*       the slope period will be 10240 kHz / 1024 = 10 kHz. The HPPASS CSG Slice Personality
*       in the <b>Device Configurator</b> includes the calculator for the DAC period value.
*
* - Configure the CSG block with the CSG slice 0 configuration:
* 
* \snippet hppass_csg_pwm/main.c SNIPPET_HPPASS_CSG_PWM_CSG_CFG
*
* - Configure the Autonomous Controller (AC) STT to start the CSG slice 0 and then stop the AC:
*
* \snippet hppass_csg_pwm/main.c SNIPPET_HPPASS_CSG_PWM_STT_CFG
*
* - Complete the HPPASS configuration with the HPPASS startup and triggers configuration.
*   The output level trigger 0 is used to route the CSG Slice 0 comparator output
*   to the output pin:
*
* \snippet hppass_csg_pwm/main.c SNIPPET_HPPASS_CSG_PWM_HPPASS_CFG
* 
* \note Only usage of the startup parameters recommended by the vendor guarantees the reliable operation
* of the Autonomous Controller.<br>
* Please use the <b>Device Configurator</b> tool to make configurations or refer to the 
* <b>Technical Reference Manual</b> for detailed information.
*
* - Configure the GPIO pin P7_0 as an output and connect it to the HPPASS Output Level Trigger 0 using TrigMux:
* \snippet hppass_csg_pwm/main.c SNIPPET_HPPASS_CSG_PWM_GPIO_INIT
*
* - Enable and assign divider 1 to the CSG with the frequency = CLK_HF3 / 2:
* \snippet hppass_csg_pwm/main.c SNIPPET_HPPASS_CSG_PWM_CSG_CLK
*
* - Initialize the HPPASS block and the Autonomous Controller:
* \snippet hppass_csg_pwm/main.c SNIPPET_HPPASS_CSG_PWM_HPPASS_INIT_AC_START
*
* - Wait for the Autonomous Controller to be ready:
* \snippet hppass_csg_pwm/main.c SNIPPET_HPPASS_CSG_PWM_AC_READY
*
* - Start DAC 0 slope generation:
* \snippet hppass_csg_pwm/main.c SNIPPET_HPPASS_CSG_PWM_DAC_START
*
* \defgroup group_hppass_csg_macros Macros
* \{
*     \defgroup group_csg_cmp_interrupts Comparator Interrupt Masks
*     \defgroup group_csg_dac_interrupts DAC Interrupt Masks
* \}
* \defgroup group_hppass_csg_functions Functions
* \{
*   \defgroup group_hppass_csg_dac DAC functions
*   \defgroup group_hppass_csg_comp Comparator functions
* \}
* \defgroup group_hppass_csg_data_structures Data Structures
* \defgroup group_hppass_csg_enums Enumerated Types
*/


#if !defined(CY_HPPASS_CSG_H)
#define CY_HPPASS_CSG_H

#include "cy_device.h"

#ifdef CY_IP_MXS40MCPASS

#include "cy_hppass_ac.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** \addtogroup group_hppass_csg_macros
* \{
*/

/** The number of CSG LUT registers */
#define CY_HPPASS_CSG_LUT_NUM       (2UL)

/** The number of CSG LUT data entries */
#define CY_HPPASS_CSG_LUT_SIZE      (128UL)

/** The maximum value of CSG DAC_VAL */
#define CY_HPPASS_CSG_DAC_VAL_MAX   (1023U)

/** \addtogroup group_csg_cmp_interrupts
 *  To be used with \ref Cy_HPPASS_Comp_GetInterruptStatus,
 *                  \ref Cy_HPPASS_Comp_GetInterruptStatusMasked,
 *                  \ref Cy_HPPASS_Comp_SetInterrupt,
 *                  \ref Cy_HPPASS_Comp_ClearInterrupt,
 *                  \ref Cy_HPPASS_Comp_SetInterruptMask,
 *                  \ref Cy_HPPASS_Comp_GetInterruptMask
 *  \{
 */
#define CY_HPPASS_INTR_CSG_0_CMP    (0x01U) /**< CSG 0 comparator interrupt mask */
#define CY_HPPASS_INTR_CSG_1_CMP    (0x02U) /**< CSG 1 comparator interrupt mask */
#define CY_HPPASS_INTR_CSG_2_CMP    (0x04U) /**< CSG 2 comparator interrupt mask */
#define CY_HPPASS_INTR_CSG_3_CMP    (0x08U) /**< CSG 3 comparator interrupt mask */
#define CY_HPPASS_INTR_CSG_4_CMP    (0x10U) /**< CSG 4 comparator interrupt mask */

#define CY_HPPASS_INTR_CSG_CMP      (HPPASS_CSG_CMP_INTR_CMP_Msk) /**< Comparator interrupt mask */
/** \} group_csg_cmp_interrupts */


/** \addtogroup group_csg_dac_interrupts
 *  To be used with \ref Cy_HPPASS_DAC_GetInterruptStatus,
 *                  \ref Cy_HPPASS_DAC_GetInterruptStatusMasked,
 *                  \ref Cy_HPPASS_DAC_SetInterrupt,
 *                  \ref Cy_HPPASS_DAC_ClearInterrupt,
 *                  \ref Cy_HPPASS_DAC_SetInterruptMask,
 *                  \ref Cy_HPPASS_DAC_GetInterruptMask
 *  \{
 */
#define CY_HPPASS_INTR_CSG_0_DAC_HW_START   (0x01UL) /**< CSG 0 DAC HW start interrupt mask */
#define CY_HPPASS_INTR_CSG_1_DAC_HW_START   (0x02UL) /**< CSG 1 DAC HW start interrupt mask */
#define CY_HPPASS_INTR_CSG_2_DAC_HW_START   (0x04UL) /**< CSG 2 DAC HW start interrupt mask */
#define CY_HPPASS_INTR_CSG_3_DAC_HW_START   (0x08UL) /**< CSG 3 DAC HW start interrupt mask */
#define CY_HPPASS_INTR_CSG_4_DAC_HW_START   (0x10UL) /**< CSG 4 DAC HW start interrupt mask */

#define CY_HPPASS_INTR_CSG_DAC_HW_START     (HPPASS_CSG_DAC_INTR_DAC_HW_START_Msk) /**< DAC HW start interrupt mask */

#define CY_HPPASS_INTR_CSG_0_DAC_SLOPE_DONE (0x0100UL) /**< CSG 0 DAC SLOPE/LUT done interrupt mask */
#define CY_HPPASS_INTR_CSG_1_DAC_SLOPE_DONE (0x0200UL) /**< CSG 1 DAC SLOPE/LUT done interrupt mask */
#define CY_HPPASS_INTR_CSG_2_DAC_SLOPE_DONE (0x0400UL) /**< CSG 2 DAC SLOPE/LUT done interrupt mask */
#define CY_HPPASS_INTR_CSG_3_DAC_SLOPE_DONE (0x0800UL) /**< CSG 3 DAC SLOPE/LUT done interrupt mask */
#define CY_HPPASS_INTR_CSG_4_DAC_SLOPE_DONE (0x1000UL) /**< CSG 4 DAC SLOPE/LUT done interrupt mask */

#define CY_HPPASS_INTR_CSG_DAC_SLOPE_DONE   (HPPASS_CSG_DAC_INTR_DAC_SLOPE_DONE_Msk) /**< DAC SLOPE/LUT done interrupt mask */

#define CY_HPPASS_INTR_CSG_0_DAC_BUF_EMPTY  (0x010000UL) /**< CSG 0 DAC empty buffer interrupt mask */
#define CY_HPPASS_INTR_CSG_1_DAC_BUF_EMPTY  (0x020000UL) /**< CSG 1 DAC empty buffer interrupt mask */
#define CY_HPPASS_INTR_CSG_2_DAC_BUF_EMPTY  (0x040000UL) /**< CSG 2 DAC empty buffer interrupt mask */
#define CY_HPPASS_INTR_CSG_3_DAC_BUF_EMPTY  (0x080000UL) /**< CSG 3 DAC empty buffer interrupt mask */
#define CY_HPPASS_INTR_CSG_4_DAC_BUF_EMPTY  (0x100000UL) /**< CSG 4 DAC empty buffer interrupt mask */

#define CY_HPPASS_INTR_CSG_DAC_BUF_EMPTY    (HPPASS_CSG_DAC_INTR_DAC_BUF_EMPTY_Msk) /**< DAC empty buffer interrupt mask */

/** Combined DAC interrupt mask */
#define CY_HPPASS_INTR_CSG_DAC              (HPPASS_CSG_DAC_INTR_DAC_HW_START_Msk | \
                                             HPPASS_CSG_DAC_INTR_DAC_SLOPE_DONE_Msk | \
                                             HPPASS_CSG_DAC_INTR_DAC_BUF_EMPTY_Msk)
/** \} group_csg_dac_interrupts */

/** \} group_hppass_csg_macros */


/***************************************
*       Enumerated Types
***************************************/

/**
* \addtogroup group_hppass_csg_enums
* \{
*/

/** The positive comparator input selection */
typedef enum
{
    CY_HPPASS_COMP_POS_A   = 0U, /**< Analog input PA */
    CY_HPPASS_COMP_POS_B   = 1U, /**< Analog input PB */
} cy_en_hppass_comp_pos_t;


/** The negative comparator input selection */
typedef enum
{
    CY_HPPASS_COMP_NEG_DAC = 0U, /**< DAC is negative input */
    CY_HPPASS_COMP_NEG_A   = 2U, /**< Analog input NA */
    CY_HPPASS_COMP_NEG_B   = 3U, /**< Analog input NB */
} cy_en_hppass_comp_neg_t;


/** The blanking mode selection */
typedef enum
{
    CY_HPPASS_COMP_BLANK_DISABLED = 0U, /**< Comparator output is not gated */
    CY_HPPASS_COMP_BLANK_TRIG_HI  = 1U, /**< Comparator output is gated to 0 by a high trigger level */
    CY_HPPASS_COMP_BLANK_TRIG_LO  = 2U, /**< Comparator output is gated to 0 by a low trigger level */
} cy_en_hppass_comp_blank_t;


/** The comparator blanking trigger selection */
typedef enum
{
    CY_HPPASS_COMP_TRIG_DISABLED = 0U,  /**< Disabled */
    CY_HPPASS_COMP_TRIG_0        = 1U,  /**< Trigger 0 */
    CY_HPPASS_COMP_TRIG_1        = 2U,  /**< Trigger 1 */
    CY_HPPASS_COMP_TRIG_2        = 3U,  /**< Trigger 2 */
    CY_HPPASS_COMP_TRIG_3        = 4U,  /**< Trigger 3 */
    CY_HPPASS_COMP_TRIG_4        = 5U,  /**< Trigger 4 */
    CY_HPPASS_COMP_TRIG_5        = 6U,  /**< Trigger 5 */
    CY_HPPASS_COMP_TRIG_6        = 7U,  /**< Trigger 6 */
    CY_HPPASS_COMP_TRIG_7        = 8U,  /**< Trigger 7 */
} cy_en_hppass_comp_trig_t;


/** The comparator output signal mode for Interrupt generation and AC status */
typedef enum
{
    CY_HPPASS_COMP_EDGE_DISABLED = 0U, /**< Disabled, no interrupts will be asserted */
    CY_HPPASS_COMP_EDGE_RISING   = 1U, /**< Rising edge */
    CY_HPPASS_COMP_EDGE_FALLING  = 2U, /**< Falling edge */
    CY_HPPASS_COMP_EDGE_BOTH     = 3U, /**< Both rising and falling edges */
} cy_en_hppass_comp_edge_t;


/** DAC start trigger source selection */
typedef enum
{
    CY_HPPASS_DAC_START_DISABLED = 0U, /**< Disabled */
    CY_HPPASS_DAC_START_TRIG_0   = 1U, /**< Trigger 0 */
    CY_HPPASS_DAC_START_TRIG_1   = 2U, /**< Trigger 1 */
    CY_HPPASS_DAC_START_TRIG_2   = 3U, /**< Trigger 2 */
    CY_HPPASS_DAC_START_TRIG_3   = 4U, /**< Trigger 3 */
    CY_HPPASS_DAC_START_TRIG_4   = 5U, /**< Trigger 4 */
    CY_HPPASS_DAC_START_TRIG_5   = 6U, /**< Trigger 5 */
    CY_HPPASS_DAC_START_TRIG_6   = 7U, /**< Trigger 6 */
    CY_HPPASS_DAC_START_TRIG_7   = 8U, /**< Trigger 7 */
    CY_HPPASS_DAC_START_AC       = 9U  /**< Autonomous Controller */
} cy_en_hppass_dac_start_t;


/** DAC update trigger source selection */
typedef enum
{
    CY_HPPASS_DAC_UPDATE_DISABLED     = 0U,  /**< Disabled */
    CY_HPPASS_DAC_UPDATE_TRIG_0       = 1U,  /**< Trigger 0 */
    CY_HPPASS_DAC_UPDATE_TRIG_1       = 2U,  /**< Trigger 1 */
    CY_HPPASS_DAC_UPDATE_TRIG_2       = 3U,  /**< Trigger 2 */
    CY_HPPASS_DAC_UPDATE_TRIG_3       = 4U,  /**< Trigger 3 */
    CY_HPPASS_DAC_UPDATE_TRIG_4       = 5U,  /**< Trigger 4 */
    CY_HPPASS_DAC_UPDATE_TRIG_5       = 6U,  /**< Trigger 5 */
    CY_HPPASS_DAC_UPDATE_TRIG_6       = 7U,  /**< Trigger 6 */
    CY_HPPASS_DAC_UPDATE_TRIG_7       = 8U,  /**< Trigger 7 */
    CY_HPPASS_DAC_UPDATE_AC           = 9U,  /**< Autonomous Controller */
    CY_HPPASS_DAC_UPDATE_PERIOD_TC    = 10U, /**< Period terminal count */
    CY_HPPASS_DAC_UPDATE_COMP_LVL     = 11U, /**< Comparator output */
    CY_HPPASS_DAC_UPDATE_COMP_LVL_INV = 12U  /**< Inverted comparator output */
} cy_en_hppass_dac_update_t;


/** DAC mode selection */
typedef enum
{
    CY_HPPASS_DAC_MODE_BUFFERED      = 0U, /**< Single Value Buffered Mode */
    CY_HPPASS_DAC_MODE_HYSTERETIC    = 1U, /**< Two Value Hysteretic Buffered Mode */
    CY_HPPASS_DAC_MODE_SLOPE_RISING  = 2U, /**< Rising Slope Mode */
    CY_HPPASS_DAC_MODE_SLOPE_FALLING = 3U, /**< Falling Slope Mode */
    CY_HPPASS_DAC_MODE_SLOPE_TRIANG  = 4U, /**< Triangular Slope Mode */
    CY_HPPASS_DAC_MODE_LUT_FORWARD   = 5U, /**< LUT Forward Mode */
    CY_HPPASS_DAC_MODE_LUT_REVERSE   = 6U, /**< LUT Reverse Mode */
    CY_HPPASS_DAC_MODE_LUT_FWD_REV   = 7U  /**< LUT Forward Reverse Mode */
} cy_en_hppass_dac_mode_t;


/** DAC HW/FW start/stop command mode selection */
typedef enum
{
    CY_HPPASS_DAC_HW = 0U,
    CY_HPPASS_DAC_FW = 1U
} cy_en_hppass_dac_cmd_t;


/** The DAC output selection for the SAR measurement */
typedef enum
{
    CY_HPPASS_DAC_OUT_DISABLED = 0U,  /**< Disabled */
    CY_HPPASS_DAC_OUT_0        = 1U,  /**< DAC 0 */
    CY_HPPASS_DAC_OUT_1        = 2U,  /**< DAC 1 */
    CY_HPPASS_DAC_OUT_2        = 3U,  /**< DAC 2 */
    CY_HPPASS_DAC_OUT_3        = 4U,  /**< DAC 3 */
    CY_HPPASS_DAC_OUT_4        = 5U,  /**< DAC 4 */
} cy_en_hppass_dac_out_t;


/** \} group_hppass_csg_enums */

/***************************************
*       Configuration Structures
***************************************/

/**
* \addtogroup group_hppass_csg_data_structures
* \{
*/


/** The Comparator configuration structure */
typedef struct
{
    cy_en_hppass_comp_pos_t pos;        /**< Comparator Positive Input selection */
    cy_en_hppass_comp_neg_t neg;        /**< Comparator Negative Input selection */
    cy_en_hppass_comp_blank_t blank;    /**< Comparator Blanking Mode.
                                         *   Select a level input trigger from CMP_BLANK_TR_SEL
                                         *   to gate the comparators output of this slice to 0.
                                         *   Polarity of active signal is selectable.
                                         */
    cy_en_hppass_comp_trig_t trig;      /**< Comparator Blanking Trigger selection */
    cy_en_hppass_comp_edge_t edge;      /**< Comparator signal mode for Interrupt generation and AC status */
    bool invert;                        /**< Inversion of the comparator output:
                                         *   - false: direct output
                                         *   - true: inverted output
                                         */
} cy_stc_hppass_comp_t;


/** The DAC period configuration structure */
typedef struct
{
    uint8_t frac;   /**< 5-bit fractional divide value */
    uint16_t intg;  /**< 12-bit integer divide value */
} cy_stc_hppass_dac_period_t;


/** The DAC configuration structure */
typedef struct
{
    cy_en_hppass_dac_start_t start;     /**< DAC trigger start. If input trigger is selected, it should be
                                         *  in the pulse mode.
                                         */
    cy_en_hppass_dac_update_t update;   /**< DAC trigger update. If input trigger is selected, it should be
                                         *   in the pulse mode
                                         */
    cy_en_hppass_dac_mode_t mode;       /**< DAC mode */
    bool continuous;                    /**< Continuous mode. (Only valid for Slope and LUT modes) */
    bool skipTrig;                      /**< Skip the first DAC hardware update trigger after the proper
                                         *   \ref cy_stc_hppass_dac_t::mode is enabled
                                         */
    bool cascade;                       /**< Synchronize parameter update with previous CSG slice in the
                                         *   case of multi-phase operation
                                         */
    bool paramSync;                     /**< Parameter Synchronization enable. If set, the DAC parameters will be
                                         *   set only after \ref Cy_HPPASS_DAC_SetParamSyncReady function call
                                         */
    uint8_t deGlitch;                   /**< DAC de-glitch time in clock cycles.
                                         *   Valid range 0..7 (0 means de-glitching is disabled, 1..7 - de-glitching
                                         *   period in cycles). The de-glitch period should be not grater than the DAC
                                         *   update period.
                                         */
    uint8_t stepSize;                   /**< Static value of 6 bit DAC Step Size. Valid range: 1..64 */
    uint16_t valBuffA;                  /**< DAC A value, usage depends on cy_stc_hppass_dac_t::mode.
                                         *   * Buffered mode - DAC value
                                         *   * Hysteretic Buffered mode - DAC high threshold
                                         *   * Slope modes - DAC start value
                                         *   * LUT modes - LUT start address
                                         */
    uint16_t valBuffB;                  /**< DAC B value, usage depends on cy_stc_hppass_dac_t::mode.
                                         *   * Buffered mode - DAC value
                                         *   * Hysteretic Buffered mode - DAC low threshold
                                         *   * Slope modes - DAC stop value
                                         *   * LUT modes - LUT stop address
                                         */
    cy_stc_hppass_dac_period_t period;   /**< DAC Period divider, clocked from CSG clock */
} cy_stc_hppass_dac_t;


/** The CSG Slice configuration structure */
typedef struct
{
    cy_stc_hppass_comp_t comp;  /**< The Comparator configuration structure */
    cy_stc_hppass_dac_t dac;    /**< The DAC configuration structure */
} cy_stc_hppass_slice_t;


/** The CSG LUT configuration structure */
typedef struct
{
    uint16_t * waveform;    /**< The pointer to array of LUT data words.
                             *   May be NULL if LUT is not used.
                             */
    uint8_t sampleNum;      /**< The number of LUT data words.
                             *   The valid range is 1..\ref CY_HPPASS_CSG_LUT_SIZE.
                             *   The value is ignored if LUT is not used.
                             */
} cy_stc_hppass_lut_t;


/** The Comparator slope generator configuration structure */
typedef struct
{
    cy_stc_hppass_slice_t const * slice[CY_HPPASS_CSG_NUM]; /**< The array of the Slice configuration
                                                             *   structures \ref cy_stc_hppass_slice_t.
                                                             *   May contain NULL pointers for unused Slices
                                                             */
    cy_stc_hppass_lut_t const * lut[CY_HPPASS_CSG_LUT_NUM]; /**< The array of the LUT configuration
                                                             *   structures \ref cy_stc_hppass_lut_t.
                                                             *   May contain NULL pointers for unused LUTs
                                                             */
    cy_en_hppass_dac_out_t dacOut;  /**< DAC output selection for the SAR measurement */
} cy_stc_hppass_csg_t;

/** \} group_hppass_csg_data_structures */


/** \cond Parameter validation macros */
#define CY_HPPASS_CSG_SLICE_NUM_VALID(slcIdx)       (CY_HPPASS_CSG_NUM > (slcIdx))
#define CY_HPPASS_CSG_DAC_STEP_VALID(dacStep)       (64U >= (dacStep) && (0U < (dacStep)))
#define CY_HPPASS_CSG_LUT_ADDR_VALID(lutAddr)       (CY_HPPASS_CSG_LUT_SIZE > (lutAddr))
#define CY_HPPASS_CSG_PERIOD_FRAC_VALID(periodFrac) (32U > (periodFrac))
#define CY_HPPASS_CSG_PERIOD_INTG_VALID(periodIntg) ((4096U >= (periodIntg)) && (0U < (periodIntg)))
#define CY_HPPASS_CSG_DAC_VAL_VALID(dacVal)         (1023U >= (dacVal))
/** \endcond */


/***************************************
*        Function Prototypes
***************************************/

/**
* \addtogroup group_hppass_csg_functions
* \{
*/


/*******************************************************************************
* Function Name: Cy_HPPASS_CSG_Init
****************************************************************************//**
*
* Loads the CSG configuration.
* Can be used after the \ref Cy_HPPASS_Init function to update the CSG settings.
*
* \note If AC is running the initialization of GSC block is not allowed
* (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
*
* \note Since the 'block ready' status is combined SAR and CSG, this particular
* CSG function cannot be protected from reinitialization during active operation,
* so it is on user's responsibility when and from where to call this function to
* reinitialize the CSG block.
*
* \param csgCfg
* Pointer to the CSG configuration structure \ref cy_stc_hppass_csg_t.
* The pointer cannot be NULL.
*
* \return
* The combined status of all the configured slices initialization, \ref cy_en_hppass_status_t.
*
*******************************************************************************/
cy_rslt_t Cy_HPPASS_CSG_Init(const cy_stc_hppass_csg_t * csgCfg);


/*******************************************************************************
* Function Name: Cy_HPPASS_CSG_DeInit
****************************************************************************//**
*
* De-initializes the CSG block, returns register values to default.
*
*
*******************************************************************************/
void Cy_HPPASS_CSG_DeInit(void);


/*******************************************************************************
* Function Name: Cy_HPPASS_CSG_SliceConfig
****************************************************************************//**
*
* Configures comparator and DAC for the specified CSG slice.
*
* \note If AC is running this configuration is not allowed
* (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
*
* \note If the DAC block is currently busy this configuration is not allowed
* (\ref CY_HPPASS_CSG_INVALID_STATE will be returned).
*
* \note Since the 'block ready' status is combined SAR and CSG, this particular
* CSG function cannot be protected from reinitialization during active operation,
* so it is on user's responsibility when and from where to call this function to
* reinitialize the CSG Slice.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
* \param slice
* Pointer to the CSG slice configuration structure \ref cy_stc_hppass_slice_t.
* The pointer cannot be NULL.
*
* \return
* Status of initialization, \ref cy_en_hppass_status_t.
*
*******************************************************************************/
cy_en_hppass_status_t Cy_HPPASS_CSG_SliceConfig(uint8_t sliceIdx, const cy_stc_hppass_slice_t * slice);


/*******************************************************************************
* Function Name: Cy_HPPASS_CSG_LoadWaveform
****************************************************************************//**
*
* Sets waveform data for the specified LUT.
*
* \param lutIdx
* The LUT index. Valid range: 0..1.
*
* \param dataIdx
* Start index of the waveform in the lookup table. Valid range: 0..127.
*
* \param numSamples
* Number of samples for the waveform. Valid range: 1..128.
* \note numSamples is not greater than the waveform array size.
* \note The sum of dataIdx and numSamples should be less or equal to 128.
*
* \param waveform
* The pointer to LUT data array, max size is 128. The pointer cannot be NULL.
*
* \return
* Status of initialization, \ref cy_en_hppass_status_t.
*
*******************************************************************************/
cy_en_hppass_status_t Cy_HPPASS_CSG_LoadWaveform(uint8_t lutIdx, uint8_t dataIdx, uint8_t numSamples, const uint16_t * waveform);


/**
* \addtogroup group_hppass_csg_dac
* \{
*/


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_SetPeriod
****************************************************************************//**
*
* Sets DAC period buffer for the specified CSG slice.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
* \param period
* The DAC period.
* See \ref cy_stc_hppass_dac_period_t.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_SetPeriod(uint8_t sliceIdx, cy_stc_hppass_dac_period_t period)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));
    CY_ASSERT_L2(CY_HPPASS_CSG_PERIOD_FRAC_VALID(period.frac));
    CY_ASSERT_L2(CY_HPPASS_CSG_PERIOD_INTG_VALID(period.intg));

    HPPASS_CSG_SLICE_DAC_PERIOD(HPPASS_BASE, sliceIdx) =
            _VAL2FLD(HPPASS_CSG_SLICE_DAC_PERIOD_PERIOD_FRAC, period.frac) |
            _VAL2FLD(HPPASS_CSG_SLICE_DAC_PERIOD_PERIOD_INT, (uint32_t)period.intg - 1UL);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_SetParamSyncReady
****************************************************************************//**
*
* Sets the ready status of DAC parameter synchronized update
* for the specified CSG slice. Registers DAC_VAL_A, DAC_VAL_B, DAC_PERIOD and
* DAC_CFG.DAC_STEP will be updated simultaneously on a subsequent DAC trigger.
*
* \note This function requires the \ref cy_stc_hppass_dac_t::paramSync setting
* to be enabled.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_SetParamSyncReady(uint8_t sliceIdx)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));

    HPPASS_CSG_SLICE_DAC_PARAM_SYNC(HPPASS_BASE, sliceIdx) = HPPASS_CSG_SLICE_DAC_PARAM_SYNC_READY_Msk;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_IsBusy
****************************************************************************//**
*
* Gets the DAC busy status of the specified CSG slice.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
* \return
* The DAC busy status.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_HPPASS_DAC_IsBusy(uint8_t sliceIdx)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));

    return _FLD2BOOL(HPPASS_CSG_SLICE_DAC_MODE_START_BUSY, HPPASS_CSG_SLICE_DAC_MODE_START(HPPASS_BASE, sliceIdx));
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_Start
****************************************************************************//**
*
* Set hardware start or FW start the DAC selected mode for the
* specified CSG slice.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
* \param command
* DAC start command mode.
* See \ref cy_en_hppass_dac_cmd_t.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_Start(uint8_t sliceIdx, cy_en_hppass_dac_cmd_t command)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));

    HPPASS_CSG_SLICE_DAC_MODE_START(HPPASS_BASE, sliceIdx) =
        (CY_HPPASS_DAC_HW == command) ? (HPPASS_CSG_SLICE_DAC_MODE_START_HW_START_Msk) :
                                        (HPPASS_CSG_SLICE_DAC_MODE_START_FW_START_Msk);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_Stop
****************************************************************************//**
*
* Stop the DAC selected mode for the specified CSG slice.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_Stop(uint8_t sliceIdx)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));

    /* Clear HW and FW Start bits */
    HPPASS_CSG_SLICE_DAC_MODE_START(HPPASS_BASE, sliceIdx) = 0UL;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_SetValue
****************************************************************************//**
*
* Sets value for DAC buffered mode for the specified CSG slice.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
* \param value
* DAC value. Valid range: 0..1023.
*
* \note It could be used to update the only VAL_A register value in other DAC
* modes.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_SetValue(uint8_t sliceIdx, uint16_t value)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));
    CY_ASSERT_L2(CY_HPPASS_CSG_DAC_VAL_VALID(value));

    HPPASS_CSG_SLICE_DAC_VAL_A(HPPASS_BASE, sliceIdx) = _VAL2FLD(HPPASS_CSG_SLICE_DAC_VAL_VALUE, value);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_SetHystereticValues
****************************************************************************//**
*
* Set high and low values for DAC hysteretic mode for the specified CSG slice.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
* \param high
* High value. Valid range: 0..1023.
*
* \param low
* Low value. Valid range: 0..1023.
*
* \note This function is only for the DAC Hysteretic mode
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_SetHystereticValues(uint8_t sliceIdx, uint16_t high, uint16_t low)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));
    CY_ASSERT_L2(CY_HPPASS_CSG_DAC_VAL_VALID(high));
    CY_ASSERT_L2(CY_HPPASS_CSG_DAC_VAL_VALID(low));

    /* Set hysteretic high threshold value into DAC VAL_A buffer */
    HPPASS_CSG_SLICE_DAC_VAL_A(HPPASS_BASE, sliceIdx) =
        _VAL2FLD(HPPASS_CSG_SLICE_DAC_VAL_A_VALUE, high);

    /* Set hysteretic low threshold value into DAC VAL_B buffer */
    HPPASS_CSG_SLICE_DAC_VAL_B(HPPASS_BASE, sliceIdx) =
        _VAL2FLD(HPPASS_CSG_SLICE_DAC_VAL_B_VALUE, low);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_SetSlopeValues
****************************************************************************//**
*
* Set the start, stop, and step values for DAC slope mode for the specified CSG slice.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
* \param start
* Start value. Valid range: 0..1023.
*
* \param stop
* Stop value. Valid range: 0..1023.
*
* \param step
* Step value. Valid range: 1..64.
*
* \note This function is only for the DAC Slope mode
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_SetSlopeValues(uint8_t sliceIdx, uint16_t start, uint16_t stop, uint8_t step)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));
    CY_ASSERT_L2(CY_HPPASS_CSG_DAC_VAL_VALID(start));
    CY_ASSERT_L2(CY_HPPASS_CSG_DAC_VAL_VALID(stop));
    CY_ASSERT_L2(CY_HPPASS_CSG_DAC_STEP_VALID(step));

    /* Set DAC step value */
    CY_REG32_CLR_SET(HPPASS_CSG_SLICE_DAC_CFG(HPPASS_BASE, sliceIdx),
        HPPASS_CSG_SLICE_DAC_CFG_DAC_STEP, (uint32_t)step - 1UL);

    /* Set start value into DAC VAL_A buffer */
    HPPASS_CSG_SLICE_DAC_VAL_A(HPPASS_BASE, sliceIdx) =
        _VAL2FLD(HPPASS_CSG_SLICE_DAC_VAL_A_VALUE, start);

    /* Set stop value into DAC VAL_B buffer */
    HPPASS_CSG_SLICE_DAC_VAL_B(HPPASS_BASE, sliceIdx) =
        _VAL2FLD(HPPASS_CSG_SLICE_DAC_VAL_B_VALUE, stop);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_SetLutAddress
****************************************************************************//**
*
* Sets the start and stop addresses, and step value for DAC LUT mode for the
* specified CSG slice.
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
* \param start
* The start address (in LUT data words). Valid range: 0..127.
*
* \param stop
* The stop address (in LUT data words). Valid range: 0..127.
*
* \note start and stop parameter values are internally converted
* into the LUT physical register address values.
*
* \param step
* Step value (in LUT data words). Valid range: 1..64.
*
* \note This function is only for the DAC LUT mode
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_SetLutAddress(uint8_t sliceIdx, uint8_t start, uint8_t stop, uint8_t step)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));
    CY_ASSERT_L2(CY_HPPASS_CSG_LUT_ADDR_VALID(start));
    CY_ASSERT_L2(CY_HPPASS_CSG_LUT_ADDR_VALID(stop));
    CY_ASSERT_L2(CY_HPPASS_CSG_DAC_STEP_VALID(step));

    /* Set DAC step value */
    CY_REG32_CLR_SET(HPPASS_CSG_SLICE_DAC_CFG(HPPASS_BASE, sliceIdx),
        HPPASS_CSG_SLICE_DAC_CFG_DAC_STEP, (uint32_t)step - 1UL);

    /* Set start value into DAC VAL_A buffer */
    HPPASS_CSG_SLICE_DAC_VAL_A(HPPASS_BASE, sliceIdx) =
        _VAL2FLD(HPPASS_CSG_SLICE_DAC_VAL_A_VALUE, (uint32_t)start << 2U); /* convert to register address */

    /* Set stop value into DAC VAL_B buffer */
    HPPASS_CSG_SLICE_DAC_VAL_B(HPPASS_BASE, sliceIdx) =
        _VAL2FLD(HPPASS_CSG_SLICE_DAC_VAL_B_VALUE, (uint32_t)stop << 2U); /* convert to register address */
}


/** \} group_hppass_csg_dac */


/**
* \addtogroup group_hppass_csg_comp
* \{
*/


/*******************************************************************************
* Function Name: Cy_HPPASS_Comp_GetStatus
****************************************************************************//**
*
* Returns the comparator output state
*
* \param sliceIdx
* The CSG slice. Valid range: 0..4.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_HPPASS_Comp_GetStatus(uint8_t sliceIdx)
{
    CY_ASSERT_L1(CY_HPPASS_CSG_SLICE_NUM_VALID(sliceIdx));

    return _FLD2BOOL(HPPASS_CSG_SLICE_CMP_STATUS_CMP_VAL, HPPASS_CSG_SLICE_CMP_STATUS(HPPASS_BASE, sliceIdx));
}

/** \} group_hppass_csg_comp */


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_GetInterruptStatus
****************************************************************************//**
*
* Gets the interrupt register status for the specified DAC.
*
* \return
* The status of combined interrupt requests for the DAC \ref group_csg_dac_interrupts.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_DAC_GetInterruptStatus(void)
{
    return HPPASS_CSG_DAC_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_ClearInterrupt
****************************************************************************//**
*
* Clears the interrupt for the specified DAC.
*
* \param interrupt
* The mask of interrupts to clear. Each bit of this mask is responsible for
* clearing of the particular interrupt in the DAC \ref group_csg_dac_interrupts.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_ClearInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_CSG_DAC));

    HPPASS_CSG_DAC_INTR(HPPASS_BASE) = CY_HPPASS_INTR_CSG_DAC & interrupt;

    /* This dummy reading is necessary here. It provides a guarantee that
    * interrupt is cleared at returning from this function.
    */
    (void)HPPASS_CSG_DAC_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_SetInterrupt
****************************************************************************//**
*
* Sets a software interrupt request for the specified DAC.
*
* \param interrupt
* The mask of interrupts to set. Each bit of this mask is responsible for
* triggering of the particular interrupt in the DAC \ref group_csg_dac_interrupts.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_SetInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_CSG_DAC));

    HPPASS_CSG_DAC_INTR_SET(HPPASS_BASE) = CY_HPPASS_INTR_CSG_DAC & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_SetInterruptMask
****************************************************************************//**
*
* Enables the interrupt for the specified DAC.
*
* \param interrupt
* The mask of interrupts allowable for activation. Each bit of this mask
* allows triggering of the particular interrupt in the DAC
* \ref group_csg_dac_interrupts.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_DAC_SetInterruptMask(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_CSG_DAC));

    HPPASS_CSG_DAC_INTR_MASK(HPPASS_BASE) = CY_HPPASS_INTR_CSG_DAC & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_GetInterruptMask
****************************************************************************//**
*
* Gets the interrupt mask for the specified DAC.
*
* \return
* The mask of the interrupts allowable for activation \ref group_csg_dac_interrupts.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_DAC_GetInterruptMask(void)
{
    return HPPASS_CSG_DAC_INTR_MASK(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_DAC_GetInterruptStatusMasked
****************************************************************************//**
*
* Gets all DACs interrupt masked status.
*
* \return
* The masked status of combined interrupt requests for the DAC
* \ref group_csg_dac_interrupts.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_DAC_GetInterruptStatusMasked(void)
{
    return HPPASS_CSG_DAC_INTR_MASKED(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_Comp_GetInterruptStatus
****************************************************************************//**
*
* Gets the interrupt register status for the specified comparator.
*
* \return
* The status of combined interrupt requests for the Comparator
* \ref group_csg_cmp_interrupts.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_Comp_GetInterruptStatus(void)
{
    return HPPASS_CSG_CMP_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_Comp_ClearInterrupt
****************************************************************************//**
*
* Clears the interrupt for the specified comparator.
*
* \param interrupt
* The mask of interrupts to clear. Each bit of this mask is responsible for
* clearing of the particular interrupt in the Comparator
* \ref group_csg_cmp_interrupts.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_Comp_ClearInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_CSG_CMP));

    HPPASS_CSG_CMP_INTR(HPPASS_BASE) = CY_HPPASS_INTR_CSG_CMP & interrupt;

    /* This dummy reading is necessary here. It provides a guarantee that
    * interrupt is cleared at returning from this function.
    */
    (void)HPPASS_CSG_CMP_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_Comp_SetInterrupt
****************************************************************************//**
*
* Sets a software interrupt request for the specified comparator.
*
* \param interrupt
* The mask of interrupts to set. Each bit of this mask is responsible for
* triggering of the particular interrupt in the Comparator
* \ref group_csg_cmp_interrupts.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_Comp_SetInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_CSG_CMP));

    HPPASS_CSG_CMP_INTR_SET(HPPASS_BASE) = CY_HPPASS_INTR_CSG_CMP & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_Comp_SetInterruptMask
****************************************************************************//**
*
* Enables the interrupt for the specified comparator.
*
* \param interrupt
* The mask of interrupts allowable for activation. Each bit of this mask
* allows triggering of the particular interrupt in the Comparator
* \ref group_csg_cmp_interrupts.
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_Comp_SetInterruptMask(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_CSG_CMP));

    HPPASS_CSG_CMP_INTR_MASK(HPPASS_BASE) = CY_HPPASS_INTR_CSG_CMP & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_Comp_GetInterruptMask
****************************************************************************//**
*
* Gets the interrupt mask for the specified comparator.
*
* \return
* The mask of the interrupts allowable for activation
* \ref group_csg_cmp_interrupts.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_Comp_GetInterruptMask(void)
{
    return HPPASS_CSG_CMP_INTR_MASK(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_Comp_GetInterruptStatusMasked
****************************************************************************//**
*
* Gets all comparators interrupt masked status.
*
* \return
* The masked status of combined interrupt requests for the Comparator
* \ref group_csg_cmp_interrupts.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_Comp_GetInterruptStatusMasked(void)
{
    return HPPASS_CSG_CMP_INTR_MASKED(HPPASS_BASE);
}

/** \} group_hppass_csg_functions */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXS40MCPASS */

#endif /** !defined(CY_HPPASS_CSG_H) */

/** \} group_hppass_csg */

/* [] END OF FILE */
