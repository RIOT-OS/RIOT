/***************************************************************************//**
* \file cy_hppass_sar.h
* \version 1.30.1
*
* Header file for the SAR ADC subsystem of the High Power Programmable Analog Sub-System.
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
* \addtogroup group_hppass_sar
* \{
* This driver provides API functions to configure the SAR ADC subsystem
* within High Power Programmable Analog Sub-System.
*
* \defgroup group_hppass_sar_macros Macros
* \{
*   \defgroup group_hppass_sar_direct_samplers Direct Sampler Masks
*   \defgroup group_hppass_sar_muxed_samplers Muxed Sampler Masks
*   \defgroup group_hppass_sar_result_interrupts Result Interrupt Masks
*   \defgroup group_hppass_sar_chan_idx Channel Indexes
*   \defgroup group_hppass_sar_status Result Status Masks
*   \defgroup group_hppass_sar_trigger Sequencer Group Trigger Collision Status Masks
*   \defgroup group_hppass_fifo_trigger FIFO Trigger Masks
    \defgroup group_hppass_sar_group_mask SAR Sequencer Group Masks
* \}
* \defgroup group_hppass_sar_functions Functions
* \{
*   \defgroup group_hppass_sar_chan Channel functions
*   \defgroup group_hppass_sar_fir FIR functions
*   \defgroup group_hppass_sar_fifo FIFO functions
* \}
*
* \defgroup group_hppass_sar_data_structures Data Structures
* \defgroup group_hppass_sar_enums Enumerated Types
*
* \} */

/**
* \addtogroup group_hppass_sar
* \{
* \section group_hppass_sar_initialization HW Initialization and Enable
* Configuration snippet for one shot SAR application.
* Startup SAR state enables SAR.
* One shot SAR measurement state triggers SAR to perform one measurement.
*
* \snippet test_apps/hppass_sar_basic/main.c SNIPPET_HPPASS_CFG_SAR_ONE_SHOT
* Functional snippet
* Application flow:
* - Startup AC microprogram enables SAR
* - Wait for SAR READY
* - Update AC microprogram to perform single measurement on channel 0
* - Steps below are repeated in loop
* - AC triggers the SAR
* - SAR generates SAR_ENTRY_DONE
* - SAR result is stored in CHAN_RESULT register
* - CHAN_RESULT register are printed out
*
* \snippet test_apps/hppass_sar_basic/main.c SNIPPET_HPPASS_FUNC_SAR_ONE_SHOT
*
* \section group_hppass_sar_interrupt Interrupt Configuration and Initialization
* To configure the SAR Group or Results interrupts in addition to the SAR and AC configuration tables, as shown in the snippet above,
* declare your own interrupt configuration structure for each interrupt source, for example:
* \snippet test_apps/hppass_sar_interrupts/main.c SNIPPET_HPPASS_SAR_ENTRYandRESULRS_INTR_CONFIG_STRUCRURE
* and initialize those configurations in the executable code. That code can be designed as a separate function (see the snippet below):
* \snippet test_apps/hppass_sar_interrupts/main.c SNIPPET_HPPASS_SAR_INTR_CONFIG_INITIALIZATION
* Also, initialize the NVIC through a separate function as well.
* \snippet test_apps/hppass_sar_interrupts/main.c SNIPPET_HPPASS_SAR_INTR_NVIC_INITIALIZATION
* The interrupt service routine for each of the interrupt sources must be implemented in the project.
* One of those interrupt service routines is shown below:
* \snippet test_apps/hppass_sar_interrupts/main.c SNIPPET_HPPASS_SAR_INTR_ENTRY0_ISR
* The configuration and initialization sequence for SAR interrupts is shown in the snippet below:
* \snippet test_apps/hppass_sar_interrupts/main.c SNIPPET_HPPASS_SAR_INTR_MAIN
*
* To generate SAR Limit or Limits interrupts, configure one or more limits and determine the limit for each
* SAR channel if needed.
* \snippet test_apps/hppass_sar_interrupts/main.c SNIPPET_HPPASS_SAR_LIMIT_CONFIG
* \snippet test_apps/hppass_sar_interrupts/main.c SNIPPET_HPPASS_SAR_LIMIT_SELECT
* That configurations must be also included in the fields of the SAR configuration structure (\ref cy_stc_hppass_sar_t):
* \snippet test_apps/hppass_sar_interrupts/main.c SNIPPET_HPPASS_SAR_CONFIG_STRUCT_LIMITS_IMPLEMENTATION
*
* \section fifo_interrupts FIFO Interrupts
* Using FIFO with MTB Device Configurator, since the FIFO has a single interrupt vector and need to be integrated with different
* solution personalities (like Motor Control) the Device Configurator itself generates the interrupt handler for entire FIFO
* with possibility to register individual callback for each FIFO buffer. See the generated source cycfg_peripherals.c for details.
* The runtime FIFO interrupt control can be done by the \ref Cy_HPPASS_FIFO_SetInterruptMask.
* The Device Configurator also generates the CYCFG_HPPASS_FIFO_INTR definition - the mask of all the used FIFO interrupt flags,
* which can be used with \ref Cy_HPPASS_FIFO_SetInterruptMask.
* When the HPPASS configurator 'start' feature is used - the Cy_HPPASS_FIFO_SetInterruptMask(CYCFG_HPPASS_FIFO_INTR)
* shall be called automatically during the generated init_cycfg_peripherals(); execution.
* \image html hppass_fifo_intr.png
* The Device Configurator generated HPPASS FIFO interrupt priority is 0 (because it is intended to drive the solution fast control loop),
* however it could be changed manually in the generated cycfg_HPPASS_FIFO_interrupt structure
* before the generated cycfg_HPPASS_FIFO_ISR_Init() execution (when the HPPASS Startup Initialize feature is disabled).
*
* \section FIR_Filter_Design FIR Filter Design
* The finite impulse response (FIR) is a generalized filter structure that allows getting all of the basic response types through varying its coefficients.
* Although the FIR structure allows implementing Low-pass filter (LPF), High-pass filter (HPF), Band-pass filter (BPF), and Band-stop filter (BSF), the recommended filter implementation in HPPASS CSG module is the LPF.
* Such a recommendation is based on a hardware HPPASS SAR FIR limitation, which is a maximum number of tap count equal to 16 taps.
* You can use any available FIR Calculator or FIR Design Tool for coefficient determination.<br>
* However, the chosen tool must allow you to:
* - Configure the filter response type to Low-pass.
* - Specify the Taps Quantity or Filter Order.
* - Set the Sample Frequency.
*
* \note The term <b><i>Tap Quantity</i></b> is similar to the <b><i>Filter Length</i></b> and is equal to the number of multipliers or coefficients in its structure.<br>
* The term <b><i>Filter Order</i></b> determines the number of states or input signal delays in the filter structure and is equal to the adders quantity.<br>
* The Filter Order can be calculated as follows:
* \code
* FilterOrder = TapsQuantity - 1
* \endcode
*
* There are various methods for FIR filter design to calculate FIR coefficients. Some tools allow you to choose the design method,
* but regardless of the exact chosen method, most FIR design tools require determining the desired frequency response parameters.<br>
* Generally, in case of the LPF FIR design that will be:
* \li <b>The frequency specifications:</b>
* - <b><i>Passband or Pass Frequency.</i></b> The passband is the frequency range where the signal passes through the filter with minimal attenuation.
* If only the Pass Frequency parameter is given, it implies that the passband starts from zero frequency and extends up to the specified Pass Frequency.
* - <b><i>Stopband or Stop Frequency.</i></b> The stopband is the frequency range where the signal is significantly attenuated or blocked by the filter.
* If only the Stop Frequency parameter is given, it implies that the stopband starts from the specified Stop Frequency and extends up to half of the sample frequency (Nyquist frequency).
* - Less often either in addition to or instead of the passband and stopband the <b><i>Transition Band</i></b> parameter could be present. The transition band lies between the passband and stopband.
* In this region, the filter's frequency response changes from minimal attenuation (in the passband) to significant attenuation (in the stopband).
* \li <b>The magnitude specifications:</b>
* - <b><i>Passband Ripple.</i></b> This parameter defines the allowed ripple (variation in dB) within the passband.
* - <b><i>Stopband Attenuation.</i></b> It specifies the minimum attenuation (in dB) required in the stopband.
*
* \image html hppass_fir_frequency_response.png
*
* The design method and parameters chosen ultimately determine the values of the filter coefficients.<br>
* After configuring the FIR filter and calculating the coefficients, you must check the obtained coefficient values.
* Due to the hardware HPPASS SAR FIR limitation, all filter coefficients must satisfy the conditions:
* \code
* (abs(Coef[0..15]) < 1) && (sum(abs(Coef[0..15])) < 8)
* \endcode
* If the obtained coefficients didn't comply with these conditions, you must change the FIR filter configurations.
* The most impact parameters to change the coefficients' values are the Sample Frequency and Transition Band width.<br>
* After calculating the coefficients and verifying the conditions, you must convert the floating-point coefficients to a fixed-point representation supported by the hardware.<br>
* The HPPASS SAR FIR stores its coefficients in the less significant 16 bits of the 32-bit registers in Q15 fixed-point format.
* \note The <b><i>Q15 fixed-point format</i></b> implies that the 15 less significant bits of the number represent its fractional part. Because the whole coefficient is a 16-bit value,<br>
* and 15 bits are reserved for the fractional part, the most significant bit is used as a sign extension. That is why the coefficients of the HPPASS SAR FIR cannot have an<br>
* integer part and their absolute value must be less than one, and the accurate coefficient values range is from -1 to 1-1/2^15.<br>
*
* To convert the floating-point coefficients to a Q15 fixed-point representation, use the equation below:
* \code
* Q15coef = round(DECcoef * 2^15)
* \endcode
* \} */

#if !defined(CY_HPPASS_SAR_H)
#define CY_HPPASS_SAR_H

#include "cy_device.h"

#ifdef CY_IP_MXS40MCPASS

#include "cy_hppass_ac.h"

#if defined(__cplusplus)
extern "C" {
#endif


/** \addtogroup group_hppass_sar_macros
*   \{ */

/** \cond INTERNAL */

/** Parameter validation macros */
#define CY_HPPASS_SAR_CHAN_NUM_VALID(chanIdx)   (CY_HPPASS_SAR_CHAN_NUM > (chanIdx))
#define CY_HPPASS_FIFO_NUM_VALID(fifoIdx)       (CY_HPPASS_FIFO_NUM > (fifoIdx))
#define CY_HPPASS_FIR_NUM_VALID(firIdx)         (CY_HPPASS_FIR_NUM > (firIdx))
#define CY_HPPASS_SAR_ADC_TEMP_VALID(val)       ((0 < (val)))
#define CY_HPPASS_SAR_TEMP_VALID(val)           ((-40 <= (val)) && (125 >= (val)))
#define CY_HPPASS_SAR_VREF_VALID(val)           ((1700U <= (val)) && (3600U >= (val)))

/** \endcond */

#define CY_HPPASS_SAR_GRP_NUM           (8U)  /**< The number of Sequencer Groups */
#define CY_HPPASS_SAR_CHAN_NUM          (28U) /**< The number of channels */
#define CY_HPPASS_SAR_SAMP_TIME_NUM     (3U)  /**< The number of sample timers */
#define CY_HPPASS_SAR_LIMIT_NUM         (8U)  /**< The number of limit crossing detection blocks */
#define CY_HPPASS_SAR_DIR_SAMP_NUM      (12U) /**< The number of the direct samplers */
#define CY_HPPASS_SAR_MUX_SAMP_NUM      (4U)  /**< The number of the muxed samplers */
#define CY_HPPASS_FIR_NUM               (2U)  /**< The number of FIR filters */
#define CY_HPPASS_FIR_TAP_NUM           (16U) /**< The number of FIR filter taps */
#define CY_HPPASS_FIFO_NUM              (4U)  /**< The number of the FIFO pieces */

#define CY_HPPASS_SAR_SAMPLERS_DISABLED     (0x0U) /**< All the samplers are disabled. */

/** \addtogroup group_hppass_sar_direct_samplers
*   \{ */
#define CY_HPPASS_SAR_DIRECT_SAMP_0         (0x001U) /**< Direct sampler #0  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_1         (0x002U) /**< Direct sampler #1  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_2         (0x004U) /**< Direct sampler #2  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_3         (0x008U) /**< Direct sampler #3  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_4         (0x010U) /**< Direct sampler #4  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_5         (0x020U) /**< Direct sampler #5  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_6         (0x040U) /**< Direct sampler #6  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_7         (0x080U) /**< Direct sampler #7  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_8         (0x100U) /**< Direct sampler #8  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_9         (0x200U) /**< Direct sampler #9  is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_10        (0x400U) /**< Direct sampler #10 is enabled. */
#define CY_HPPASS_SAR_DIRECT_SAMP_11        (0x800U) /**< Direct sampler #11 is enabled. */

/** Combined Direct samplers mask */
#define CY_HPPASS_SAR_DIRECT_SAMP           (HPPASS_SAR_CFG_SAMP_EN_DIRECT_SAMP_EN_Msk)
/** \} group_hppass_sar_direct_samplers */


/** \addtogroup group_hppass_sar_muxed_samplers
*   \{ */
#define CY_HPPASS_SAR_MUXED_SAMP_0          (0x1U) /**< Muxed sampler #0 is enabled. */
#define CY_HPPASS_SAR_MUXED_SAMP_1          (0x2U) /**< Muxed sampler #1 is enabled. */
#define CY_HPPASS_SAR_MUXED_SAMP_2          (0x4U) /**< Muxed sampler #2 is enabled. */
#define CY_HPPASS_SAR_MUXED_SAMP_3          (0x8U) /**< Muxed sampler #3 is enabled. */

/** Combined Muxed samplers mask */
#define CY_HPPASS_SAR_MUXED_SAMP            (CY_HPPASS_SAR_MUXED_SAMP_0 | \
                                             CY_HPPASS_SAR_MUXED_SAMP_1 | \
                                             CY_HPPASS_SAR_MUXED_SAMP_2 | \
                                             CY_HPPASS_SAR_MUXED_SAMP_3)
/** \} group_hppass_sar_muxed_samplers */

/** \addtogroup group_hppass_sar_group_mask
 *  To be used with \ref Cy_HPPASS_SAR_CrossTalkAdjust
 *   \{
 */
#define CY_HPPASS_SAR_GROUP_0_Msk (0x00001UL) /**< Mask for the SAR Sequencer Group #0 */
#define CY_HPPASS_SAR_GROUP_1_Msk (0x00002UL) /**< Mask for the SAR Sequencer Group #1 */
#define CY_HPPASS_SAR_GROUP_2_Msk (0x00004UL) /**< Mask for the SAR Sequencer Group #2 */
#define CY_HPPASS_SAR_GROUP_3_Msk (0x00008UL) /**< Mask for the SAR Sequencer Group #3 */
#define CY_HPPASS_SAR_GROUP_4_Msk (0x00010UL) /**< Mask for the SAR Sequencer Group #4 */
#define CY_HPPASS_SAR_GROUP_5_Msk (0x00020UL) /**< Mask for the SAR Sequencer Group #5 */
#define CY_HPPASS_SAR_GROUP_6_Msk (0x00040UL) /**< Mask for the SAR Sequencer Group #6 */
#define CY_HPPASS_SAR_GROUP_7_Msk (0x00080UL) /**< Mask for the SAR Sequencer Group #7 */
/** \} group_hppass_sar_group_mask */

/** \addtogroup group_hppass_sar_result_interrupts
 *  To be used with \ref Cy_HPPASS_SAR_Result_GetInterruptStatusMasked,
 *                  \ref Cy_HPPASS_SAR_Result_ClearInterrupt,
 *                  \ref Cy_HPPASS_SAR_Result_GetOverflowStatus,
 *                  \ref Cy_HPPASS_SAR_Result_ClearOverflowStatus
 *   \{
 */
#define CY_HPPASS_INTR_SAR_RESULT_GROUP_0   (0x00001UL) /**< Result interrupt mask for the SAR Sequencer Group #0 */
#define CY_HPPASS_INTR_SAR_RESULT_GROUP_1   (0x00002UL) /**< Result interrupt mask for the SAR Sequencer Group #1 */
#define CY_HPPASS_INTR_SAR_RESULT_GROUP_2   (0x00004UL) /**< Result interrupt mask for the SAR Sequencer Group #2 */
#define CY_HPPASS_INTR_SAR_RESULT_GROUP_3   (0x00008UL) /**< Result interrupt mask for the SAR Sequencer Group #3 */
#define CY_HPPASS_INTR_SAR_RESULT_GROUP_4   (0x00010UL) /**< Result interrupt mask for the SAR Sequencer Group #4 */
#define CY_HPPASS_INTR_SAR_RESULT_GROUP_5   (0x00020UL) /**< Result interrupt mask for the SAR Sequencer Group #5 */
#define CY_HPPASS_INTR_SAR_RESULT_GROUP_6   (0x00040UL) /**< Result interrupt mask for the SAR Sequencer Group #6 */
#define CY_HPPASS_INTR_SAR_RESULT_GROUP_7   (0x00080UL) /**< Result interrupt mask for the SAR Sequencer Group #7 */
#define CY_HPPASS_INTR_SAR_RESULT_GROUP     (HPPASS_SAR_CFG_SAR_RESULT_INTR_ENTRY_DONE_Msk) /**< Combined SAR Sequencer Group result interrupt mask */

#define CY_HPPASS_INTR_SAR_RESULT_FIR_0     (0x10000UL) /**< Result interrupt mask for the FIR #0 */
#define CY_HPPASS_INTR_SAR_RESULT_FIR_1     (0x20000UL) /**< Result interrupt mask for the FIR #1 */
#define CY_HPPASS_INTR_SAR_RESULT_FIR       (HPPASS_SAR_CFG_SAR_RESULT_INTR_FIR_DONE_Msk)   /**< Combined FIR result interrupt mask */

/** Combined RESULT interrupt mask */
#define CY_HPPASS_INTR_SAR_RESULT           (HPPASS_SAR_CFG_SAR_RESULT_INTR_ENTRY_DONE_Msk | \
                                             HPPASS_SAR_CFG_SAR_RESULT_INTR_FIR_DONE_Msk)
/** \} group_hppass_sar_result_interrupts */

/** \addtogroup group_hppass_sar_limit_interrupts
 *  To be used with \ref Cy_HPPASS_SAR_Limit_GetInterruptStatusMasked,
 *                  \ref Cy_HPPASS_SAR_Limit_ClearInterrupt
 *   \{
 */
#define CY_HPPASS_INTR_SAR_LIMIT_GROUP_0    (0x01UL) /**< Limit interrupt mask for the SAR Sequencer Group #0 */
#define CY_HPPASS_INTR_SAR_LIMIT_GROUP_1    (0x02UL) /**< Limit interrupt mask for the SAR Sequencer Group #1 */
#define CY_HPPASS_INTR_SAR_LIMIT_GROUP_2    (0x04UL) /**< Limit interrupt mask for the SAR Sequencer Group #2 */
#define CY_HPPASS_INTR_SAR_LIMIT_GROUP_3    (0x08UL) /**< Limit interrupt mask for the SAR Sequencer Group #3 */
#define CY_HPPASS_INTR_SAR_LIMIT_GROUP_4    (0x10UL) /**< Limit interrupt mask for the SAR Sequencer Group #4 */
#define CY_HPPASS_INTR_SAR_LIMIT_GROUP_5    (0x20UL) /**< Limit interrupt mask for the SAR Sequencer Group #5 */
#define CY_HPPASS_INTR_SAR_LIMIT_GROUP_6    (0x40UL) /**< Limit interrupt mask for the SAR Sequencer Group #6 */
#define CY_HPPASS_INTR_SAR_LIMIT_GROUP_7    (0x80UL) /**< Limit interrupt mask for the SAR Sequencer Group #7 */
#define CY_HPPASS_INTR_SAR_LIMIT            (HPPASS_SAR_CFG_SAR_RANGE_INTR_RANGE_Msk) /**< Combined SAR limit interrupt mask */
/** \} group_hppass_sar_limit_interrupts */

/** \addtogroup group_hppass_fifo_level_interrupts
 *  To be used with \ref Cy_HPPASS_FIFO_GetInterruptStatus,
 *                  \ref Cy_HPPASS_FIFO_GetInterruptStatusMasked,
 *                  \ref Cy_HPPASS_FIFO_SetInterrupt,
 *                  \ref Cy_HPPASS_FIFO_ClearInterrupt,
 *                  \ref Cy_HPPASS_FIFO_SetInterruptMask,
 *                  \ref Cy_HPPASS_FIFO_GetInterruptMask
 *   \{
 */
#define CY_HPPASS_INTR_FIFO_0_LEVEL         (0x1UL) /**< FIFO #0 level interrupt mask */
#define CY_HPPASS_INTR_FIFO_1_LEVEL         (0x2UL) /**< FIFO #1 level interrupt mask */
#define CY_HPPASS_INTR_FIFO_2_LEVEL         (0x4UL) /**< FIFO #2 level interrupt mask */
#define CY_HPPASS_INTR_FIFO_3_LEVEL         (0x8UL) /**< FIFO #3 level interrupt mask */
#define CY_HPPASS_INTR_FIFO                 (HPPASS_MMIO_FIFO_INTR_FIFO_LEVEL_Msk) /**< Combined FIFO level interrupt mask */
/** \} group_hppass_fifo_level_interrupts */

/** \addtogroup group_hppass_fifo_trigger
 *  To be used with \ref Cy_HPPASS_FIFO_Config
 *   \{
 */
#define CY_HPPASS_FIFO_0_TRIG   (0x01UL) /**< FIFO #0 trigger mask */
#define CY_HPPASS_FIFO_1_TRIG   (0x02UL) /**< FIFO #1 trigger mask */
#define CY_HPPASS_FIFO_2_TRIG   (0x04UL) /**< FIFO #2 trigger mask */
#define CY_HPPASS_FIFO_3_TRIG   (0x08UL) /**< FIFO #3 trigger mask */
/** \} group_hppass_fifo_trigger */

/** \addtogroup group_hppass_sar_chan_idx
 *  To be used with \ref Cy_HPPASS_FIFO_Read,
 *                  \ref Cy_HPPASS_FIFO_ReadAll,
 *                  \ref Cy_HPPASS_SAR_Result_ChannelRead
 *  \{
 */
#define CY_HPPASS_SAR_CHAN_0_IDX  (0U)  /**< The Index of the SAR Channel #0 */
#define CY_HPPASS_SAR_CHAN_1_IDX  (1U)  /**< The Index of the SAR Channel #1 */
#define CY_HPPASS_SAR_CHAN_2_IDX  (2U)  /**< The Index of the SAR Channel #2 */
#define CY_HPPASS_SAR_CHAN_3_IDX  (3U)  /**< The Index of the SAR Channel #3 */
#define CY_HPPASS_SAR_CHAN_4_IDX  (4U)  /**< The Index of the SAR Channel #4 */
#define CY_HPPASS_SAR_CHAN_5_IDX  (5U)  /**< The Index of the SAR Channel #5 */
#define CY_HPPASS_SAR_CHAN_6_IDX  (6U)  /**< The Index of the SAR Channel #6 */
#define CY_HPPASS_SAR_CHAN_7_IDX  (7U)  /**< The Index of the SAR Channel #7 */
#define CY_HPPASS_SAR_CHAN_8_IDX  (8U)  /**< The Index of the SAR Channel #8 */
#define CY_HPPASS_SAR_CHAN_9_IDX  (9U)  /**< The Index of the SAR Channel #9 */
#define CY_HPPASS_SAR_CHAN_10_IDX (10U) /**< The Index of the SAR Channel #10 */
#define CY_HPPASS_SAR_CHAN_11_IDX (11U) /**< The Index of the SAR Channel #11 */
#define CY_HPPASS_SAR_CHAN_12_IDX (12U) /**< The Index of the SAR Channel #12 */
#define CY_HPPASS_SAR_CHAN_13_IDX (13U) /**< The Index of the SAR Channel #13 */
#define CY_HPPASS_SAR_CHAN_14_IDX (14U) /**< The Index of the SAR Channel #14 */
#define CY_HPPASS_SAR_CHAN_15_IDX (15U) /**< The Index of the SAR Channel #15 */
#define CY_HPPASS_SAR_CHAN_16_IDX (16U) /**< The Index of the SAR Channel #16 */
#define CY_HPPASS_SAR_CHAN_17_IDX (17U) /**< The Index of the SAR Channel #17 */
#define CY_HPPASS_SAR_CHAN_18_IDX (18U) /**< The Index of the SAR Channel #18 */
#define CY_HPPASS_SAR_CHAN_19_IDX (19U) /**< The Index of the SAR Channel #19 */
#define CY_HPPASS_SAR_CHAN_20_IDX (20U) /**< The Index of the SAR Channel #20 */
#define CY_HPPASS_SAR_CHAN_21_IDX (21U) /**< The Index of the SAR Channel #21 */
#define CY_HPPASS_SAR_CHAN_22_IDX (22U) /**< The Index of the SAR Channel #22 */
#define CY_HPPASS_SAR_CHAN_23_IDX (23U) /**< The Index of the SAR Channel #23 */
#define CY_HPPASS_SAR_CHAN_24_IDX (24U) /**< The Index of the SAR Channel #24 */
#define CY_HPPASS_SAR_CHAN_25_IDX (25U) /**< The Index of the SAR Channel #25 */
#define CY_HPPASS_SAR_CHAN_26_IDX (26U) /**< The Index of the SAR Channel #26 */
#define CY_HPPASS_SAR_CHAN_27_IDX (27U) /**< The Index of the SAR Channel #27 */
#define CY_HPPASS_SAR_FIR_0_IDX   (30U) /**< The Index of the FIR Channel #0 */
#define CY_HPPASS_SAR_FIR_1_IDX   (31U) /**< The Index of the FIR Channel #1 */
/** \} group_hppass_sar_chan_idx */

/** \addtogroup group_hppass_sar_status
 *  To be used with \ref Cy_HPPASS_SAR_Result_GetStatus,
 *                  \ref Cy_HPPASS_SAR_Result_ClearStatus,
 *                  \ref Cy_HPPASS_SAR_Limit_GetStatus,
 *                  \ref Cy_HPPASS_SAR_Limit_ClearStatus
 *  \{
 */
#define CY_HPPASS_SAR_CHAN_0_MSK  (1UL << CY_HPPASS_SAR_CHAN_0_IDX)  /**< The Status mask for the SAR Channel #0 */
#define CY_HPPASS_SAR_CHAN_1_MSK  (1UL << CY_HPPASS_SAR_CHAN_1_IDX)  /**< The Status mask for the SAR Channel #1 */
#define CY_HPPASS_SAR_CHAN_2_MSK  (1UL << CY_HPPASS_SAR_CHAN_2_IDX)  /**< The Status mask for the SAR Channel #2 */
#define CY_HPPASS_SAR_CHAN_3_MSK  (1UL << CY_HPPASS_SAR_CHAN_3_IDX)  /**< The Status mask for the SAR Channel #3 */
#define CY_HPPASS_SAR_CHAN_4_MSK  (1UL << CY_HPPASS_SAR_CHAN_4_IDX)  /**< The Status mask for the SAR Channel #4 */
#define CY_HPPASS_SAR_CHAN_5_MSK  (1UL << CY_HPPASS_SAR_CHAN_5_IDX)  /**< The Status mask for the SAR Channel #5 */
#define CY_HPPASS_SAR_CHAN_6_MSK  (1UL << CY_HPPASS_SAR_CHAN_6_IDX)  /**< The Status mask for the SAR Channel #6 */
#define CY_HPPASS_SAR_CHAN_7_MSK  (1UL << CY_HPPASS_SAR_CHAN_7_IDX)  /**< The Status mask for the SAR Channel #7 */
#define CY_HPPASS_SAR_CHAN_8_MSK  (1UL << CY_HPPASS_SAR_CHAN_8_IDX)  /**< The Status mask for the SAR Channel #8 */
#define CY_HPPASS_SAR_CHAN_9_MSK  (1UL << CY_HPPASS_SAR_CHAN_9_IDX)  /**< The Status mask for the SAR Channel #9 */
#define CY_HPPASS_SAR_CHAN_10_MSK (1UL << CY_HPPASS_SAR_CHAN_10_IDX) /**< The Status mask for the SAR Channel #10 */
#define CY_HPPASS_SAR_CHAN_11_MSK (1UL << CY_HPPASS_SAR_CHAN_11_IDX) /**< The Status mask for the SAR Channel #11 */
#define CY_HPPASS_SAR_CHAN_12_MSK (1UL << CY_HPPASS_SAR_CHAN_12_IDX) /**< The Status mask for the SAR Channel #12 */
#define CY_HPPASS_SAR_CHAN_13_MSK (1UL << CY_HPPASS_SAR_CHAN_13_IDX) /**< The Status mask for the SAR Channel #13 */
#define CY_HPPASS_SAR_CHAN_14_MSK (1UL << CY_HPPASS_SAR_CHAN_14_IDX) /**< The Status mask for the SAR Channel #14 */
#define CY_HPPASS_SAR_CHAN_15_MSK (1UL << CY_HPPASS_SAR_CHAN_15_IDX) /**< The Status mask for the SAR Channel #15 */
#define CY_HPPASS_SAR_CHAN_16_MSK (1UL << CY_HPPASS_SAR_CHAN_16_IDX) /**< The Status mask for the SAR Channel #16 */
#define CY_HPPASS_SAR_CHAN_17_MSK (1UL << CY_HPPASS_SAR_CHAN_17_IDX) /**< The Status mask for the SAR Channel #17 */
#define CY_HPPASS_SAR_CHAN_18_MSK (1UL << CY_HPPASS_SAR_CHAN_18_IDX) /**< The Status mask for the SAR Channel #18 */
#define CY_HPPASS_SAR_CHAN_19_MSK (1UL << CY_HPPASS_SAR_CHAN_19_IDX) /**< The Status mask for the SAR Channel #19 */
#define CY_HPPASS_SAR_CHAN_20_MSK (1UL << CY_HPPASS_SAR_CHAN_20_IDX) /**< The Status mask for the SAR Channel #20 */
#define CY_HPPASS_SAR_CHAN_21_MSK (1UL << CY_HPPASS_SAR_CHAN_21_IDX) /**< The Status mask for the SAR Channel #21 */
#define CY_HPPASS_SAR_CHAN_22_MSK (1UL << CY_HPPASS_SAR_CHAN_22_IDX) /**< The Status mask for the SAR Channel #22 */
#define CY_HPPASS_SAR_CHAN_23_MSK (1UL << CY_HPPASS_SAR_CHAN_23_IDX) /**< The Status mask for the SAR Channel #23 */
#define CY_HPPASS_SAR_CHAN_24_MSK (1UL << CY_HPPASS_SAR_CHAN_24_IDX) /**< The Status mask for the SAR Channel #24 */
#define CY_HPPASS_SAR_CHAN_25_MSK (1UL << CY_HPPASS_SAR_CHAN_25_IDX) /**< The Status mask for the SAR Channel #25 */
#define CY_HPPASS_SAR_CHAN_26_MSK (1UL << CY_HPPASS_SAR_CHAN_26_IDX) /**< The Status mask for the SAR Channel #26 */
#define CY_HPPASS_SAR_CHAN_27_MSK (1UL << CY_HPPASS_SAR_CHAN_27_IDX) /**< The Status mask for the SAR Channel #27 */
#define CY_HPPASS_SAR_FIR_0_MSK   (1UL << CY_HPPASS_SAR_FIR_0_IDX)   /**< The Status mask for the FIR Channel #0 */
#define CY_HPPASS_SAR_FIR_1_MSK   (1UL << CY_HPPASS_SAR_FIR_1_IDX)   /**< The Status mask for the FIR Channel #1 */

/** The Common Status mask for all the ADC and FIR channels */
#define CY_HPPASS_SAR_CHAN_MSK (CY_HPPASS_SAR_CHAN_0_MSK | \
                                CY_HPPASS_SAR_CHAN_1_MSK | \
                                CY_HPPASS_SAR_CHAN_2_MSK | \
                                CY_HPPASS_SAR_CHAN_3_MSK | \
                                CY_HPPASS_SAR_CHAN_4_MSK | \
                                CY_HPPASS_SAR_CHAN_5_MSK | \
                                CY_HPPASS_SAR_CHAN_6_MSK | \
                                CY_HPPASS_SAR_CHAN_7_MSK | \
                                CY_HPPASS_SAR_CHAN_8_MSK | \
                                CY_HPPASS_SAR_CHAN_9_MSK | \
                                CY_HPPASS_SAR_CHAN_10_MSK | \
                                CY_HPPASS_SAR_CHAN_11_MSK | \
                                CY_HPPASS_SAR_CHAN_12_MSK | \
                                CY_HPPASS_SAR_CHAN_13_MSK | \
                                CY_HPPASS_SAR_CHAN_14_MSK | \
                                CY_HPPASS_SAR_CHAN_15_MSK | \
                                CY_HPPASS_SAR_CHAN_16_MSK | \
                                CY_HPPASS_SAR_CHAN_17_MSK | \
                                CY_HPPASS_SAR_CHAN_18_MSK | \
                                CY_HPPASS_SAR_CHAN_19_MSK | \
                                CY_HPPASS_SAR_CHAN_20_MSK | \
                                CY_HPPASS_SAR_CHAN_21_MSK | \
                                CY_HPPASS_SAR_CHAN_22_MSK | \
                                CY_HPPASS_SAR_CHAN_23_MSK | \
                                CY_HPPASS_SAR_CHAN_24_MSK | \
                                CY_HPPASS_SAR_CHAN_25_MSK | \
                                CY_HPPASS_SAR_CHAN_26_MSK | \
                                CY_HPPASS_SAR_CHAN_27_MSK | \
                                CY_HPPASS_SAR_FIR_0_MSK | \
                                CY_HPPASS_SAR_FIR_1_MSK)
/** \} group_hppass_sar_status */

/** \addtogroup group_hppass_sar_trigger
 *  To be used with \ref Cy_HPPASS_SAR_GetTriggerCollisionStatus,
 *                  \ref Cy_HPPASS_SAR_ClearTriggerCollisionStatus
 *  \{
 */
#define CY_HPPASS_SAR_GROUP_0_TRIG_COLLISION (0x01U) /**< Trigger collision mask Group #0 */
#define CY_HPPASS_SAR_GROUP_1_TRIG_COLLISION (0x02U) /**< Trigger collision mask Group #1 */
#define CY_HPPASS_SAR_GROUP_2_TRIG_COLLISION (0x04U) /**< Trigger collision mask Group #2 */
#define CY_HPPASS_SAR_GROUP_3_TRIG_COLLISION (0x08U) /**< Trigger collision mask Group #3 */
#define CY_HPPASS_SAR_GROUP_4_TRIG_COLLISION (0x10U) /**< Trigger collision mask Group #4 */
#define CY_HPPASS_SAR_GROUP_5_TRIG_COLLISION (0x20U) /**< Trigger collision mask Group #5 */
#define CY_HPPASS_SAR_GROUP_6_TRIG_COLLISION (0x40U) /**< Trigger collision mask Group #6 */
#define CY_HPPASS_SAR_GROUP_7_TRIG_COLLISION (0x80U) /**< Trigger collision mask Group #7 */
/** \} group_hppass_sar_trigger */

/** The pointer to the Channel Result register, useful for e.g. DMA descriptor initialization */
#define CY_HPPASS_SAR_CHAN_RSLT_PTR(chanIdx) ((volatile const uint32_t *)&HPPASS_SAR_CHAN_RESULT(HPPASS_BASE, (chanIdx)))

/** The pointer to the FIR Result register, useful for e.g. DMA descriptor initialization */
#define CY_HPPASS_SAR_FIR_RSLT_PTR(firIdx) ((volatile const uint32_t *)&HPPASS_SAR_FIR_RESULT(HPPASS_BASE, (firIdx)))

/** The pointer to the FIFO Read Data register, useful for e.g. DMA descriptor initialization */
#define CY_HPPASS_SAR_FIFO_READ_PTR(fifoIdx) ((volatile const uint32_t *)&HPPASS_MMIO_FIFO_RD_DATA(HPPASS_BASE, (fifoIdx)))
/** \} group_hppass_sar_macros */

/** \cond Internal macros */
/* Value validation macros */
#define CY_HPPASS_SAR_GROUP_HOLD_CNT_VALID(value)   ((0U < (value)) && (256U >= (value)))

#define CY_HPPASS_COND_VALID(cond)          ((CY_HPPASS_SAR_COND_BELOW  == (cond)) || \
                                            (CY_HPPASS_SAR_COND_INSIDE  == (cond)) || \
                                            (CY_HPPASS_SAR_COND_ABOVE   == (cond)) || \
                                            (CY_HPPASS_SAR_COND_OUTSIDE == (cond)))

#define CY_HPPASS_CHAN_VALID(chan)          ((CY_HPPASS_FIR_DISABLED == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_0    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_1    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_2    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_3    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_4    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_5    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_6    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_7    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_8    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_9    == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_10   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_11   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_12   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_13   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_14   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_15   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_16   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_17   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_18   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_19   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_20   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_21   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_22   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_23   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_24   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_25   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_26   == (chan)) || \
                                            (CY_HPPASS_FIR_CHAN_27   == (chan)))

#define CY_HPPASS_LIMIT_VALID(limit)        ((CY_HPPASS_SAR_LIMIT_DISABLED == (limit)) || \
                                            (CY_HPPASS_SAR_LIMIT_0         == (limit)) || \
                                            (CY_HPPASS_SAR_LIMIT_1         == (limit)) || \
                                            (CY_HPPASS_SAR_LIMIT_2         == (limit)) || \
                                            (CY_HPPASS_SAR_LIMIT_3         == (limit)) || \
                                            (CY_HPPASS_SAR_LIMIT_4         == (limit)) || \
                                            (CY_HPPASS_SAR_LIMIT_5         == (limit)) || \
                                            (CY_HPPASS_SAR_LIMIT_6         == (limit)) || \
                                            (CY_HPPASS_SAR_LIMIT_7         == (limit)))

#define CY_HPPASS_FIFO_VALID(fifo)          ((CY_HPPASS_FIFO_DISABLED == (fifo)) || \
                                            (CY_HPPASS_FIFO_0         == (fifo)) || \
                                            (CY_HPPASS_FIFO_1         == (fifo)) || \
                                            (CY_HPPASS_FIFO_2         == (fifo)) || \
                                            (CY_HPPASS_FIFO_3         == (fifo)))

#define CY_HPPASS_AVG_VALID(avg)            ((CY_HPPASS_SAR_AVG_DISABLED == (avg)) || \
                                            (CY_HPPASS_SAR_AVG_2         == (avg)) || \
                                            (CY_HPPASS_SAR_AVG_4         == (avg)) || \
                                            (CY_HPPASS_SAR_AVG_8         == (avg)) || \
                                            (CY_HPPASS_SAR_AVG_16        == (avg)) || \
                                            (CY_HPPASS_SAR_AVG_32        == (avg)))

#define CY_HPPASS_MODE_VALID(mode)          ((CY_HPPASS_FIFO_1_32 == (mode)) || \
                                            (CY_HPPASS_FIFO_2_16  == (mode)) || \
                                            (CY_HPPASS_FIFO_4_8   == (mode)))

#define CY_HPPASS_LEVEL_VALID(mode, level)  ((0U < level) && (((CY_HPPASS_FIFO_1_32 == mode) ? 32U : \
                                                              ((CY_HPPASS_FIFO_2_16 == mode) ? 16U : 8U)) >= level))

#define CY_HPPASS_TRIGMSK_VALID(trigMsk)    (0U == ((trigMsk) & ((uint8_t) ~(CY_HPPASS_FIFO_0_TRIG | \
                                                                             CY_HPPASS_FIFO_1_TRIG | \
                                                                             CY_HPPASS_FIFO_2_TRIG | \
                                                                             CY_HPPASS_FIFO_3_TRIG))))

#define CY_HPPASS_SAR_LIMIT_LOW_VALID(low)      ((65535L > (low))   && (-32768L <= (low)))
#define CY_HPPASS_SAR_LIMIT_HIGH_VALID(high)    ((65535L >= (high)) && (-32768L < (high)))
/** \endcond */


/***************************************
*       Enumerated Types
***************************************/


/**
* \addtogroup group_hppass_sar_enums
* \{
*/


/** SAR voltage reference source */
typedef enum
{
    CY_HPPASS_SAR_VREF_VDDA = 0U, /**< VDDA reference */
    CY_HPPASS_SAR_VREF_EXT  = 1U  /**< External reference */
} cy_en_hppass_sar_vref_t;


/** The limit crossing detection condition */
typedef enum
{
    CY_HPPASS_SAR_COND_BELOW   = 0U, /**< Result < LIMIT_LOW */
    CY_HPPASS_SAR_COND_INSIDE  = 1U, /**< LIMIT_LOW <= result < LIMIT_HIGH */
    CY_HPPASS_SAR_COND_ABOVE   = 2U, /**< Result > LIMIT_HIGH */
    CY_HPPASS_SAR_COND_OUTSIDE = 3U, /**< Result < LIMIT_LOW || result >= LIMIT_HIGH */
} cy_en_hppass_sar_cond_t;


/** Trigger sources selection */
typedef enum
{
    CY_HPPASS_SAR_TRIG_DISABLED = 0U, /**< Triggering is disabled */
    CY_HPPASS_SAR_TRIG_0        = 1U, /**< Trigger 0 */
    CY_HPPASS_SAR_TRIG_1        = 2U, /**< Trigger 1 */
    CY_HPPASS_SAR_TRIG_2        = 3U, /**< Trigger 2 */
    CY_HPPASS_SAR_TRIG_3        = 4U, /**< Trigger 3 */
    CY_HPPASS_SAR_TRIG_4        = 5U, /**< Trigger 4 */
    CY_HPPASS_SAR_TRIG_5        = 6U, /**< Trigger 5 */
    CY_HPPASS_SAR_TRIG_6        = 7U, /**< Trigger 6 */
    CY_HPPASS_SAR_TRIG_7        = 8U, /**< Trigger 7 */
    CY_HPPASS_SAR_TRIG_AC       = 9U, /**< Trigger from the Autonomous Controller */
} cy_en_hppass_sar_trig_t;


/** Sampling Timer selection */
typedef enum
{
    CY_HPPASS_SAR_SAMP_TIME_DISABLED = 0U, /**< Sampling Timer is not used */
    CY_HPPASS_SAR_SAMP_TIME_0        = 1U, /**< Sampling Timer 0 */
    CY_HPPASS_SAR_SAMP_TIME_1        = 2U, /**< Sampling Timer 1 */
    CY_HPPASS_SAR_SAMP_TIME_2        = 3U, /**< Sampling Timer 2 */
} cy_en_hppass_sar_samp_time_t;


/** Sampler Gain selection */
typedef enum
{
    CY_HPPASS_SAR_SAMP_GAIN_1  = 0U, /**< Sampler Gain = 1 */
    CY_HPPASS_SAR_SAMP_GAIN_3  = 1U, /**< Sampler Gain = 3 */
    CY_HPPASS_SAR_SAMP_GAIN_6  = 2U, /**< Sampler Gain = 6 */
    CY_HPPASS_SAR_SAMP_GAIN_12 = 3U, /**< Sampler Gain = 12 */
} cy_en_hppass_sar_samp_gain_t;


/** Input channel selection for FIR filter */
typedef enum
{
    CY_HPPASS_FIR_DISABLED   = 0U,   /**< DISABLED */
    CY_HPPASS_FIR_CHAN_0     = 1U,   /**< Channel 0 */
    CY_HPPASS_FIR_CHAN_1     = 2U,   /**< Channel 1 */
    CY_HPPASS_FIR_CHAN_2     = 3U,   /**< Channel 2 */
    CY_HPPASS_FIR_CHAN_3     = 4U,   /**< Channel 3 */
    CY_HPPASS_FIR_CHAN_4     = 5U,   /**< Channel 4 */
    CY_HPPASS_FIR_CHAN_5     = 6U,   /**< Channel 5 */
    CY_HPPASS_FIR_CHAN_6     = 7U,   /**< Channel 6 */
    CY_HPPASS_FIR_CHAN_7     = 8U,   /**< Channel 7 */
    CY_HPPASS_FIR_CHAN_8     = 9U,   /**< Channel 8 */
    CY_HPPASS_FIR_CHAN_9     = 10U,  /**< Channel 9 */
    CY_HPPASS_FIR_CHAN_10    = 11U,  /**< Channel 10 */
    CY_HPPASS_FIR_CHAN_11    = 12U,  /**< Channel 11 */
    CY_HPPASS_FIR_CHAN_12    = 13U,  /**< Channel 12 */
    CY_HPPASS_FIR_CHAN_13    = 14U,  /**< Channel 13 */
    CY_HPPASS_FIR_CHAN_14    = 15U,  /**< Channel 14 */
    CY_HPPASS_FIR_CHAN_15    = 16U,  /**< Channel 15 */
    CY_HPPASS_FIR_CHAN_16    = 17U,  /**< Channel 16 */
    CY_HPPASS_FIR_CHAN_17    = 18U,  /**< Channel 17 */
    CY_HPPASS_FIR_CHAN_18    = 19U,  /**< Channel 18 */
    CY_HPPASS_FIR_CHAN_19    = 20U,  /**< Channel 19 */
    CY_HPPASS_FIR_CHAN_20    = 21U,  /**< Channel 20 */
    CY_HPPASS_FIR_CHAN_21    = 22U,  /**< Channel 21 */
    CY_HPPASS_FIR_CHAN_22    = 23U,  /**< Channel 22 */
    CY_HPPASS_FIR_CHAN_23    = 24U,  /**< Channel 23 */
    CY_HPPASS_FIR_CHAN_24    = 25U,  /**< Channel 24 */
    CY_HPPASS_FIR_CHAN_25    = 26U,  /**< Channel 25 */
    CY_HPPASS_FIR_CHAN_26    = 27U,  /**< Channel 26 */
    CY_HPPASS_FIR_CHAN_27    = 28U,  /**< Channel 27 */
} cy_en_hppass_fir_chan_t;


/** Limit crossing detector selection */
typedef enum
{
    CY_HPPASS_SAR_LIMIT_DISABLED = 0U, /**< DISABLED */
    CY_HPPASS_SAR_LIMIT_0        = 1U, /**< Limit detector #0 */
    CY_HPPASS_SAR_LIMIT_1        = 2U, /**< Limit detector #1 */
    CY_HPPASS_SAR_LIMIT_2        = 3U, /**< Limit detector #2 */
    CY_HPPASS_SAR_LIMIT_3        = 4U, /**< Limit detector #3 */
    CY_HPPASS_SAR_LIMIT_4        = 5U, /**< Limit detector #4 */
    CY_HPPASS_SAR_LIMIT_5        = 6U, /**< Limit detector #5 */
    CY_HPPASS_SAR_LIMIT_6        = 7U, /**< Limit detector #6 */
    CY_HPPASS_SAR_LIMIT_7        = 8U, /**< Limit detector #7 */
} cy_en_hppass_sar_limit_t;


/** FIFO selection */
typedef enum
{
    CY_HPPASS_FIFO_DISABLED = 0U, /**< DISABLED */
    CY_HPPASS_FIFO_0        = 1U, /**< FIFO #0 */
    CY_HPPASS_FIFO_1        = 2U, /**< FIFO #1 */
    CY_HPPASS_FIFO_2        = 3U, /**< FIFO #2 */
    CY_HPPASS_FIFO_3        = 4U, /**< FIFO #3 */
} cy_en_hppass_fifo_t;


/** FIFO dividing mode selection */
typedef enum
{
    CY_HPPASS_FIFO_1_32 = 0U, /**< The FIFO block is configured as a single 32-entry FIFO buffer (FIFO_0) */
    CY_HPPASS_FIFO_2_16 = 1U, /**< The FIFO block is divided into two 16-entry FIFO buffers (FIFO_0, FIFO_1) */
    CY_HPPASS_FIFO_4_8  = 2U, /**< The FIFO block is divided into four 8-entry FIFO buffers (FIFO_0, FIFO_1, FIFO_2, FIFO_3) */
} cy_en_hppass_fifo_mode_t;


/** SAR averaging selection */
typedef enum
{
    CY_HPPASS_SAR_AVG_DISABLED = 0U, /**< Averaging is disabled */
    CY_HPPASS_SAR_AVG_2        = 1U, /**< Averaging 2 */
    CY_HPPASS_SAR_AVG_4        = 2U, /**< Averaging 4 */
    CY_HPPASS_SAR_AVG_8        = 3U, /**< Averaging 8 */
    CY_HPPASS_SAR_AVG_16       = 4U, /**< Averaging 16 */
    CY_HPPASS_SAR_AVG_32       = 5U, /**< Averaging 32 */
} cy_en_hppass_sar_avg_t;


/** Analog routing/muxing control mode */
typedef enum
{
    CY_HPPASS_SAR_MUX_SEQ = 0U, /**< The MUX is controlled by \ref cy_stc_hppass_sar_grp_t::muxChanIdx */
    CY_HPPASS_SAR_MUX_FW  = 1U, /**< The MUX is controlled by FW only,
                                 *   using \ref Cy_HPPASS_SAR_SetSwitch and
                                 *         \ref Cy_HPPASS_SAR_ClearSwitch
                                 */
    CY_HPPASS_SAR_MUX_AC  = 2U, /**< The MUX is controlled by \ref cy_stc_hppass_stt_mux_t::chanIdx */
} cy_en_hppass_sar_mux_t;


/** Temperature sensor current control */
typedef enum

{
    CY_HPPASS_SAR_TEMP_SENSOR_CURRENT_LO = 0U, /**< The BJT current is 1uA (Low) */
    CY_HPPASS_SAR_TEMP_SENSOR_CURRENT_HI = 1U, /**< Add 8uA to BJT current (High)*/
} cy_en_hppass_sar_temp_sensor_current_t;

/** \} group_hppass_sar_enums */


/***************************************
*       Configuration Structures
***************************************/


/**
* \addtogroup group_hppass_sar_data_structures
* \{
*/


/** The SAR Sequencer Table Entry configuration structure */
typedef struct
{
    uint16_t dirSampMsk;                        /**< Direct samplers enable mask, each bit corresponds to one
                                                 *   of the 12 direct samplers. Zero bit of this mask corresponds
                                                 *   to sampler 0, first bit to sampler 1 and so on. Bits 12 to 15
                                                 *   are not used. When this Entry is triggered, any samplers with
                                                 *   a bit set to '1' will be simultaneously sampled, held, and
                                                 *   then converted. There is a 1-1 correspondence between these
                                                 *   12 samplers and the first 12 result channels. SAR results
                                                 *   can be read using \ref Cy_HPPASS_SAR_Result_ChannelRead
                                                 *   with corresponding channel index.
                                                 */
    uint8_t muxSampMsk;                         /**< Muxed samplers enable mask, each bit corresponds to one of
                                                 *   the 4 muxed samplers. Zero bit of this mask corresponds to
                                                 *   sampler 0, first bit to sampler 1 and so on. Bits 4 to 7
                                                 *   are not used. When this Entry is triggered, any samplers
                                                 *   with a bit set to '1' will be simultaneously sampled, held,
                                                 *   and then converted. Each one of these samplers maps to 4
                                                 *   result channels as determined by the mux setting 0, 1, 2,
                                                 *   or 3 in order, using cy_stc_hppass_sar_grp_t::muxChanIdx
                                                 *   field.
                                                 *   The mapping is:
                                                 *   * bit 0 (sampler 12) can sample 12, 13, 14, 15 channels
                                                 *   * bit 1 (sampler 13) can sample 16, 17, 18, 19 channels
                                                 *   * bit 2 (sampler 14) can sample 20, 21, 22, 23 channels
                                                 *   * bit 3 (sampler 15) can sample 24, 25, 26, 27 channels
                                                 *
                                                 *   SAR results can be read using
                                                 *   \ref Cy_HPPASS_SAR_Result_ChannelRead with corresponding
                                                 *   channel index.
                                                 */
    uint8_t muxChanIdx[CY_HPPASS_SAR_MUX_NUM];  /**< Muxed Sampler Channel selection for sampler 12, 13, 14 and 15.
                                                 *   Valid Range: 0...3
                                                 *   Effective only if the correspondent
                                                 *   \ref cy_stc_hppass_sar_t::muxMode == \ref CY_HPPASS_SAR_MUX_SEQ
                                                 *   (the default option), otherwise ignored.
                                                 */
    cy_en_hppass_sar_trig_t trig;               /**< Input Trigger selection. Pulse and Level triggers may be used.
                                                 *   When the Level trigger is used, trigger event is on the rising
                                                 *   edge of the trigger.
                                                 */
    cy_en_hppass_sar_samp_time_t sampTime;      /**< Sampling Timer Selection. Choose one out of four available
                                                 *   Sample Timers: \ref cy_stc_hppass_sar_t::sampTime
                                                 */
    bool priority;                              /**< Priority: false - low priority, true - high priority
                                                 *   If the application configures a combination of high and low
                                                 *   priority entries, depending on the usage demand on the SAR
                                                 *   from High Priority Entries, a Low Priority Entry might stay in
                                                 *   HOLD state beyond a specified limit, set by
                                                 *   \ref Cy_HPPASS_SAR_SetHoldViolationCnt function. To ensure that
                                                 *   the Low Priority Entry did not stay in HOLD state beyond this
                                                 *   limit, before reading a Low Priority Entry result, check the
                                                 *   \ref Cy_HPPASS_SAR_GetHoldViolationStatus function status to
                                                 *   ensure that the result is within spec.
                                                 *   Refer to device TRM for more details.
                                                 */
    bool continuous;                            /**< Continuous mode. If true - this entry will automatically be
                                                 *   re-triggered when the current conversion of all channels in
                                                 *   this entry has been completed. In general a continuous Entry
                                                 *   should have additional sample time
                                                 *   (cy_stc_hppass_sar_grp_t::sampTime is not
                                                 *   \ref CY_HPPASS_SAR_SAMP_TIME_DISABLED), otherwise this Entry
                                                 *   will be continuously triggered and use the majority of the SAR
                                                 *   performance bandwidth. Sample time is the only way to set the
                                                 *   period of a continuous Entry.
                                                 */
} cy_stc_hppass_sar_grp_t;


/** The limit crossing detection configuration structure
 * \note The limit crossing detection block cannot be assigned to multiple channels/filters with different sign
 *       settings (filters are always signed).
 */
typedef struct
{
    cy_en_hppass_sar_cond_t cond;   /**< Limit Crossing Detection condition */
    int32_t low;                    /**< Low limit. The valid range is -32768 ... 32766 for signed values
                                     *   and 0 ... 65534 for unsigned values.
                                     */
    int32_t high;                   /**< High limit. The valid range is -32767 ... 32767 for signed values
                                     *   and 1 ... 65535 for unsigned values.
                                     */
} cy_stc_hppass_sar_limit_t;


/** The FIR filter Configuration structure */
typedef struct
{
    cy_en_hppass_fir_chan_t chan;           /**< Filtering channel selection */
    bool waitTapInit;                       /**< Wait for 16 samples before the first valid result is available
                                             *   by the \ref Cy_HPPASS_SAR_Result_FirRead function or FIFO.
                                             *   This allows to ignore the FIR results until the FIR pipeline is
                                             *   initialized.
                                             */
    int16_t coef[CY_HPPASS_FIR_TAP_NUM];    /**< FIR Coefficient Values
                                             *   The coefficient values can range from -1+1/2^15 to 1-1/2^15
                                             *   with a resolution of 1/2^15.
                                             *   The coefficient uses twos-complement notation and
                                             *   the decimal point is fixed to the right of the most significant bit.
                                             *   Examples: -0.25 = 0xE000 and 0.25 = 0x2000
                                             *   The max absolute value of the FIR gain must be less than 8
                                             *  (sum of all the coefficients), otherwise the result may overflow
                                             *   allocated 16-bits.
                                             */
    cy_en_hppass_sar_limit_t limit;         /**< Limit crossing detector selection */
    bool result;                            /**< The result is available by the \ref Cy_HPPASS_SAR_Result_FirRead function
                                             *   and the result interrupt is generated.
                                             */
    cy_en_hppass_fifo_t fifo;               /**< FIFO selection */
} cy_stc_hppass_fir_t;


/** The FIFO buffer Configuration structure */
typedef struct
{
    cy_en_hppass_fifo_mode_t mode;      /**< Split mode */
    uint8_t trigMsk;                    /**< Enable trigger mask, see \ref group_hppass_fifo_trigger */
    uint8_t level[CY_HPPASS_FIFO_NUM];  /**< Level settings. Valid range 1..32, 1..16 or 1..8, depends on
                                         *   \ref cy_stc_hppass_fifo_t::mode
                                         */
} cy_stc_hppass_fifo_t;


/** The Channel Configuration structure */
typedef struct
{
    bool diff;                      /**< Differential mode: false - disabled, true - enabled */
    bool sign;                      /**< Signed data mode: false - unsigned, true - signed */
    cy_en_hppass_sar_avg_t avg;     /**< Averaging mode */
    cy_en_hppass_sar_limit_t limit; /**< Limit Crossing Detection block selection */
    bool result;                    /**< The result is available by \ref Cy_HPPASS_SAR_Result_ChannelRead
                                     *   and the result interrupt is generated.
                                     */
    cy_en_hppass_fifo_t fifo;       /**< FIFO selection */
} cy_stc_hppass_sar_chan_t;


/** The SAR configuration structure */
typedef struct
{
    cy_en_hppass_sar_vref_t vref;   /**< SAR voltage reference source */
    bool lowSupply;                 /**< Low Supply mode: Should be set false if VDDA >= 2.7V and true if VDDA < 2.7V */
    bool offsetCal;                 /**< Enable the offset calibration during startup */
    bool linearCal;                 /**< Enable the linearity calibration during startup */
    bool gainCal;                   /**< Enable the gain calibration during startup */
    bool chanId;                    /**< FIFO Channel Number Identifier Enable */
    bool aroute;                    /**< Enable global AROUTE */
    uint16_t dirSampEnMsk;          /**< Static Enables for Direct Samplers.
                                     *   Must be set if in use by application, otherwise it is OFF and must not be
                                     *   selected in any Sequencer Group.
                                     *   A given Sequencer Group is considered invalid if
                                     *   * There are no Samplers enabled and
                                     *   * For the given set of enabled Samplers, they all must be powered ON with
                                     *     this static setting.
                                     *   If the Group is invalid, no action will be taken in the Sequencer if this
                                     *   Group is triggered.
                                     */
    uint8_t muxSampEnMsk;           /**< Static Enables for Muxed Samplers.
                                     *   Must be set if in use by application, otherwise it is OFF and must not be
                                     *   selected in any Sequencer Group. Refer to cy_stc_hppass_sar_t::dirSampEnMsk
                                     *   for details.
                                     */
    uint16_t holdCount;             /**< This is the a global max HOLD count applied to all entries.
                                     *   The resolution of each count is 16 HPPASS high frequency clock counts.
                                     *   For example, if the HPPASS high frequency clock is 160MHz, each count will be
                                     *   16/160MHz = 100ns. The default value for this field is 30 counts
                                     *   (3us at 160MHz).
                                     */
    cy_en_hppass_sar_samp_gain_t dirSampGain[CY_HPPASS_SAR_DIR_SAMP_NUM];   /**< The array of the direct sampler gain
                                                                             *   settings. For the odd samplers which
                                                                             *   are used as a pseudo-differential
                                                                             *   negative the gain must be the same as
                                                                             *   in the correspondent even positive
                                                                             *   sampler.
                                                                             */
    cy_en_hppass_sar_samp_gain_t muxSampGain[CY_HPPASS_SAR_MUX_SAMP_NUM];   /**< The array of the muxed sampler gain
                                                                             *   settings. For the odd samplers which
                                                                             *   are used as a pseudo-differential
                                                                             *   negative the gain must be the same as
                                                                             *   in the correspondent even positive
                                                                             *   sampler.
                                                                             */
    uint16_t sampTime[CY_HPPASS_SAR_SAMP_TIME_NUM];                         /**< The array of the sampling timer
                                                                             *   apertures in clock cycles.
                                                                             *   Valid range is 1..1024
                                                                             */
    cy_stc_hppass_sar_chan_t const * chan[CY_HPPASS_SAR_CHAN_NUM];          /**< Array of pointers to the channel
                                                                             *   configuration structures. May contain
                                                                             *   NULL pointers for unused channels.
                                                                             */
    cy_stc_hppass_sar_grp_t const * grp[CY_HPPASS_SAR_GRP_NUM];             /**< Array of pointers to the SAR Sequencer
                                                                             *   Group configuration structures. May
                                                                             *   contain NULL pointers for unused groups.
                                                                             */
    cy_stc_hppass_sar_limit_t const * limit[CY_HPPASS_SAR_LIMIT_NUM];       /**< Array of pointers to the limit crossing
                                                                             *   detection configuration structures.
                                                                             *   May contain NULL pointers for unused
                                                                             *   ranges.
                                                                             */
    cy_en_hppass_sar_mux_t muxMode[CY_HPPASS_SAR_MUX_NUM];                  /**< Analog routing MUX control mode */
    cy_stc_hppass_fir_t const * fir[CY_HPPASS_FIR_NUM];                     /**< Array of pointers to the FIR
                                                                             *   configuration structures. May contain
                                                                             *   NULL pointers for unused filters.
                                                                             */
    cy_stc_hppass_fifo_t const * fifo;                                      /**< Pointer to the FIFO configuration
                                                                             *   structure. Can be NULL if FIFO is not
                                                                             *   used.
                                                                             */
} cy_stc_hppass_sar_t;


/** \} group_hppass_sar_data_structures */


/***************************************
*        Function Prototypes
***************************************/


/**
* \addtogroup group_hppass_sar_functions
* \{
*/


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Init
****************************************************************************//**
*
* Loads the SAR ADC configuration.
* Can be used after the \ref Cy_HPPASS_Init function to update the SAR settings.
*
* \note If AC is running the initialization of SAR block is not allowed
* (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
*
* \note If the SAR block is currently busy the initialization is not allowed
* (\ref CY_HPPASS_SAR_INVALID_STATE will be returned).
*
* \note Since the 'block ready' status is combined SAR and CSG, this particular
* SAR function cannot be protected from reinitialization during the
* 'block ready' == true condition, so it is on user's responsibility when
* and from where to call this function to reinitialize the SAR block.
*
* \param sarCfg
* Pointer to the SAR configuration structure \ref cy_stc_hppass_sar_t.
* The pointer cannot be NULL.
*
* \return
* Status of initialization, \ref cy_en_hppass_status_t.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_basic/main.c SNIPPET_HPPASS_SAR_CFG
*
*******************************************************************************/
cy_en_hppass_status_t Cy_HPPASS_SAR_Init(const cy_stc_hppass_sar_t * sarCfg);


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_DeInit
****************************************************************************//**
*
* Resets all the SAR registers to their default state.
*
*******************************************************************************/
void Cy_HPPASS_SAR_DeInit(void);


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_IsBusy
****************************************************************************//**
*
* Returns the SAR ADC busy status.
*
* \note Do not use this function with continuous Sequencer Group mode
* \ref cy_stc_hppass_sar_grp_t::continuous.
* See the Device Datasheet Errata section for more details.
*
* \return
* Busy status: true - busy, false - idle.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_IS_BUSY
*
*******************************************************************************/
__STATIC_INLINE bool Cy_HPPASS_SAR_IsBusy(void)
{
    return _FLD2BOOL(HPPASS_SAR_CFG_SAR_STATUS_BUSY, HPPASS_SAR_STATUS(HPPASS_BASE));
}


/** \cond
* Internal function - not to be used in the user code.
* The function suppress the ADC 'busy' condition check
* in case when it is called from the cy_hppass_sar.c file
* to do not repeat this checks repeatedly in 'for' cycles.
* It also contain AC running conditional check.
*/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_SAR_CondBusyCheck(void)
{
    #if !defined CY_HPPASS_SUPPRESS_ADC_BUSY_CHECK
        cy_en_hppass_status_t result = Cy_HPPASS_AC_CondRunCheck();

        if ((CY_HPPASS_SUCCESS == result) && Cy_HPPASS_SAR_IsBusy())
        {
            result = CY_HPPASS_SAR_INVALID_STATE;
        }

        return result;
    #else
        return CY_HPPASS_SUCCESS;
    #endif
}
/** \endcond */


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Limit_Config
****************************************************************************//**
*
* Configures the ADC limit crossing detection block.
*
* \note If AC is running this configuration is not allowed
* (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
*
* \note If the SAR block is currently busy the initialization is not allowed
* (\ref CY_HPPASS_SAR_INVALID_STATE will be returned).
*
* \note Since the 'block ready' status is combined SAR and CSG, this particular
* SAR function cannot be protected from reinitialization during the
* 'block ready' == true condition, so it is on user's responsibility when
* and from where to call this function to reconfigure this SAR sub-block.
*
* \param limIdx
* The index of the limit crossing detection block. Valid range 0..7.
*
* \param limCfg
* Pointer to the limit crossing detection configuration structure
* \ref cy_stc_hppass_sar_limit_t. The pointer cannot be NULL.
*
* \return
* Status, \ref cy_en_hppass_status_t.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_LIM_CFG
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_SAR_Limit_Config(uint8_t limIdx, const cy_stc_hppass_sar_limit_t * limCfg)
{
    cy_en_hppass_status_t result = Cy_HPPASS_SAR_CondBusyCheck();

    if (CY_HPPASS_SUCCESS == result)
    {
        if ((NULL != limCfg) && (CY_HPPASS_SAR_LIMIT_NUM > limIdx))
        {
            CY_ASSERT_L3(CY_HPPASS_COND_VALID(limCfg->cond));
            CY_ASSERT_L2(CY_HPPASS_SAR_LIMIT_LOW_VALID(limCfg->low));
            CY_ASSERT_L2(CY_HPPASS_SAR_LIMIT_HIGH_VALID(limCfg->high));

            HPPASS_SAR_LIMIT_CFG(HPPASS_BASE, limIdx) = (uint32_t)limCfg->cond;
            HPPASS_SAR_LIMIT_LOW(HPPASS_BASE, limIdx) = (uint32_t)limCfg->low;
            HPPASS_SAR_LIMIT_HIGH(HPPASS_BASE, limIdx) = (uint32_t)limCfg->high;
        }
        else
        {
            result = CY_HPPASS_SAR_BAD_PARAM;
        }
    }

    return result;
}

#if ((CY_IP_MXS40MCPASS_VERSION == 1u) && (CY_IP_MXS40MCPASS_VERSION_MINOR == 0u))
/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_CrossTalkAdjust
****************************************************************************//**
*
* Adjusts the sampler offset trimming according to the number of simultaneously
* triggered (sampled and held) samplers, specified by groupMask
* (for single or multiple simultaneously triggered groups with same timing settings)
*
* Each sampler is compensated in relation to the other samplers that are sampled and held simultaneously.<br>
*
* Basic use case:
* - This function has to be called after the following functions calls:
* \ref Cy_HPPASS_Init, \ref Cy_HPPASS_SAR_Init, \ref Cy_HPPASS_SAR_GroupConfig, 
* or \ref Cy_HPPASS_SAR_TempAdjust.
*
* \note The Device Configurator automatically generates a call to this function during startup if the
* appropriate parameter in HPPASS configurations is checked.<br>
*
* Advanced use cases:
* - If a sampler is involved in multiple groups, this function has to be called each time before another
* one of these groups is triggered, to re-compensate the sampler within the another group.
* - If multiple groups are triggered simultaneously (by the same trigger signal), and
* the sampling timer settings for these groups are identical, this function has to be called with combined
* mask of the simultaneously triggered groups.
* 
* \note This function does not perform compensation for four or fewer simultaneously triggered samplers
* (within single or multiple groups), as the impact of compensation in these cases is negligible.
* \warning This function has not be used when VDDA is less than 2.7V, as this can cause
* over-compensation.
*
* \param groupMsk The \ref group_hppass_sar_group_mask "mask" of the simultaneously triggered groups
*
* \funcusage
*
* \snippet test_apps/hppass_sar_cross_talk_adjust/main.c SNIPPET_HPPASS_SAR_XTLK_CALL
*
*******************************************************************************/
void Cy_HPPASS_SAR_CrossTalkAdjust(uint8_t groupMsk);
#else
/** \cond INTERNAL */
__STATIC_INLINE void Cy_HPPASS_SAR_CrossTalkAdjust (uint8_t groupMsk)
{
    (void) groupMsk;
}
/** \endcond INTERNAL */
#endif

/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_GroupConfig
****************************************************************************//**
*
* Configures a single Sequencer Group.
*
* \note If AC is running this configuration is not allowed
* (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
*
* \note If the SAR block is currently busy the initialization is not allowed
* (\ref CY_HPPASS_SAR_INVALID_STATE will be returned).
*
* \note Since the 'block ready' status is combined SAR and CSG, this particular
* SAR function cannot be protected from reinitialization during the
* 'block ready' == true condition, so it is on user's responsibility when
* and from where to call this function to reconfigure this SAR sub-block.
*
* \param grpIdx
* The index of the Sequencer Group. Valid range 0..7.
*
* \param grpCfg
* Pointer to the Sequencer Group configuration structure \ref cy_stc_hppass_sar_grp_t.
* The pointer cannot be NULL.
*
* \return
* Status, \ref cy_en_hppass_status_t.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_basic/main.c SNIPPET_HPPASS_SAR_GRP_CFG
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_SAR_GroupConfig(uint8_t grpIdx, const cy_stc_hppass_sar_grp_t * grpCfg)
{
    cy_en_hppass_status_t result = Cy_HPPASS_SAR_CondBusyCheck();

    if (CY_HPPASS_SUCCESS == result)
    {
        if ((NULL != grpCfg) && (CY_HPPASS_SAR_GRP_NUM > grpIdx))
        {
            HPPASS_SAR_SEQ_GROUP(HPPASS_BASE, grpIdx) = _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_DIRECT_SAMPLER_EN, grpCfg->dirSampMsk) |
                                                        _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_MUXED_SAMPLER_EN, grpCfg->muxSampMsk)  |
                                                        _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_MUX0_SEL, grpCfg->muxChanIdx[0U]) |
                                                        _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_MUX1_SEL, grpCfg->muxChanIdx[1U]) |
                                                        _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_MUX2_SEL, grpCfg->muxChanIdx[2U]) |
                                                        _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_MUX3_SEL, grpCfg->muxChanIdx[3U]) |
                                                        _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_TR_SEL, grpCfg->trig)             |
                                                        _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_SAMP_TIME_SEL, grpCfg->sampTime)  |
                                                        _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_PRIORITY, grpCfg->priority)       |
                                                        _VAL2FLD(HPPASS_SAR_SEQ_ENTRY_CONT, grpCfg->continuous);
        }
        else
        {
            result = CY_HPPASS_SAR_BAD_PARAM;
        }
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_DisableGroupContConvert
****************************************************************************//**
*
* Disables continuous conversion for the specified SAR group
*
* \note If AC is running this configuration is not allowed
* (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
*
* \note to re-enable the continuous conversion, the restart of
* entire SAR block using \ref Cy_HPPASS_AC_Start is needed.
*
* \note Since the 'block ready' status is combined SAR and CSG, this particular
* SAR function cannot be protected from reinitialization during the
* 'block ready' == true condition, so it is on user's responsibility when
* and from where to call this function to reconfigure this SAR sub-block.
*
* \param grpIdx
* The index of the Sequencer Group. Valid range 0..7.
*
* \return
* Status, \ref cy_en_hppass_status_t.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_DIS_GRP_CONT_CONVERT
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_SAR_DisableGroupContConvert(uint8_t grpIdx)
{
    cy_en_hppass_status_t result = Cy_HPPASS_AC_CondRunCheck();

    if (CY_HPPASS_SUCCESS == result)
    {
        if (CY_HPPASS_SAR_GRP_NUM > grpIdx)
        {
            HPPASS_SAR_SEQ_GROUP(HPPASS_BASE, grpIdx) &= ~HPPASS_SAR_SEQ_ENTRY_CONT_Msk;
        }
        else
        {
            result = CY_HPPASS_SAR_BAD_PARAM;
        }
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_GetTriggerCollisionStatus
****************************************************************************//**
*
* Returns the Sequencer Group Trigger Collision Status for all the Groups.
*
* \return 8-bit Group mask (a bit per each Group).
* See \ref group_hppass_sar_trigger.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_GET_TR_COLLISION_STATUS
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_HPPASS_SAR_GetTriggerCollisionStatus(void)
{
    return (uint8_t)HPPASS_SAR_GROUP_TR_COLLISION(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_ClearTriggerCollisionStatus
****************************************************************************//**
*
* Clears the Sequencer Group Trigger Collision Status for the specified Sequences.
*
* \param trigCollision The 8-bit Group mask (a bit per each Group).
* See \ref group_hppass_sar_trigger.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_CLR_TR_COLLISION_STATUS
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_ClearTriggerCollisionStatus(uint8_t trigCollision)
{
    HPPASS_SAR_GROUP_TR_COLLISION(HPPASS_BASE) = trigCollision;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_GetHoldViolationStatus
****************************************************************************//**
*
* Returns the Sequencer Group Hold Violation Status for all the Sequences.
*
* \return 8-bit Group mask (a bit per each Group).
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_GET_HOLD_VIOL_STATUS
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_HPPASS_SAR_GetHoldViolationStatus(void)
{
    return (uint8_t)HPPASS_SAR_GROUP_HOLD_VIOLATION(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_ClearHoldViolationStatus
****************************************************************************//**
*
* Clears the Sequencer Group Hold Violation Status for the specified Sequences.
*
* \param holdViolation The 8-bit Group mask (a bit per each Group).
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_CLR_HOLD_VIOL_STATUS
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_ClearHoldViolationStatus(uint8_t holdViolation)
{
    HPPASS_SAR_GROUP_HOLD_VIOLATION(HPPASS_BASE) = holdViolation;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_SetHoldViolationCnt
****************************************************************************//**
*
* Sets the common maximum hold count value for all the Sequencer Groups.
*
* \param cnt The hold count value.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_SET_HOLD_VIOL_CNT
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_SetHoldViolationCnt(uint16_t cnt)
{
    CY_ASSERT_L2(CY_HPPASS_SAR_GROUP_HOLD_CNT_VALID(cnt));
    HPPASS_SAR_GROUP_HOLD_CNT(HPPASS_BASE) = (uint32_t)cnt - 1UL;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_GetHoldViolationCnt
****************************************************************************//**
*
* Returns the common maximum hold count value for all the Sequencer Groups.
*
* \return
* The hold count value.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_SET_HOLD_VIOL_CNT
*
*******************************************************************************/
__STATIC_INLINE uint16_t Cy_HPPASS_SAR_GetHoldViolationCnt(void)
{
    return (uint16_t)(1UL + HPPASS_SAR_GROUP_HOLD_CNT(HPPASS_BASE));
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_SetTempSensorCurrent
****************************************************************************//**
*
* Selects low or high current for temperature sensor.
*
* \param curr Current:
* false - BJT current is 1uA (default),
* true - Add 8uA to BJT current.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_tempAdjust/main.c SNIPPET_HPPASS_TEMP_CALC
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_SetTempSensorCurrent(cy_en_hppass_sar_temp_sensor_current_t curr)
{
    HPPASS_SAR_TEMPSENSE_CTRL(HPPASS_BASE) = (uint32_t)curr;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_TEMP_Calc
****************************************************************************//**
*
* Calculates the temperature based on two ADC reading results of
* temperature sensor.
* See the device Architecture Technical Reference Manual for more detail.
*
* \param tempHi ADC reading when the selected temp sensor is
* biased with a additional 8uA current.
*
* \param tempLo ADC reading when the selected temp sensor is
* biased with a 1uA current.
*
* \return
* Temperature value.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_tempAdjust/main.c SNIPPET_HPPASS_TEMP_CALC
*
*******************************************************************************/
__STATIC_INLINE int16_t Cy_HPPASS_TEMP_Calc(int16_t tempHi, int16_t tempLo)
{
    CY_ASSERT_L1(CY_HPPASS_SAR_ADC_TEMP_VALID(tempHi));
    CY_ASSERT_L1(CY_HPPASS_SAR_ADC_TEMP_VALID(tempLo));
    CY_ASSERT_L1(tempHi > tempLo);

    float ratio = ((float)tempHi - (float)tempLo) / (float)tempLo;

    float temp = (float)((int32_t)SFLASH_SAR_TEMP_COEF_A) * ratio * ratio * ratio +
                 (float)((int32_t)SFLASH_SAR_TEMP_COEF_B) * ratio * ratio +
                 (float)((int32_t)SFLASH_SAR_TEMP_COEF_C) * ratio +
                 (float)((int32_t)SFLASH_SAR_TEMP_COEF_D);

    return (int16_t)temp;
}

/** \cond BWC */
#define Cy_HPPASS_SAR_Adjust Cy_HPPASS_SAR_TempAdjust /*BWC Macros*/
/** \endcond BWC */

/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_TempAdjust
****************************************************************************//**
*
* Adjusts the offset calibration value according to input temperature value.
*
* \param temp The temperature value.
*
* \param vRef The reference voltage in mV used by the system in the field.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_ADJ
*
*******************************************************************************/
void Cy_HPPASS_SAR_TempAdjust(int16_t temp, uint16_t vRef);


/** \addtogroup group_hppass_sar_fir
*   \{ */

/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_FirConfig
****************************************************************************//**
*
* Initializes the ADC FIR filter.
*
* \note If AC is running this configuration is not allowed
* (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
*
* \note If the SAR block is currently busy the initialization is not allowed
* (\ref CY_HPPASS_SAR_INVALID_STATE will be returned).
*
* \note Since the 'block ready' status is combined SAR and CSG, this particular
* SAR function cannot be protected from reinitialization during the
* 'block ready' == true condition, so it is on user's responsibility when
* and from where to call this function to reconfigure this SAR sub-block.
*
* \param firIdx
* The index of the FIR filter instance. Valid range 0..1.
*
* \param firCfg
* Pointer to the FIR filter configuration structure \ref cy_stc_hppass_fir_t.
* The pointer cannot be NULL.
*
* \return
* Status, \ref cy_en_hppass_status_t.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_fir_fifo/main.c SNIPPET_HPPASS_FIR_CFG
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_FIR_CFG_FUNC
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_SAR_FirConfig(uint8_t firIdx, const cy_stc_hppass_fir_t * firCfg)
{
    cy_en_hppass_status_t result = Cy_HPPASS_SAR_CondBusyCheck();

    if (CY_HPPASS_SUCCESS == result)
    {
        if ((NULL != firCfg) && (CY_HPPASS_FIR_NUM_VALID(firIdx)))
        {
            uint32_t resultMask_Pos = (uint32_t)firIdx + CY_HPPASS_SAR_FIR_0_IDX;
            uint32_t resultMask_Msk = CY_HPPASS_SAR_FIR_0_MSK << firIdx;

            CY_ASSERT_L3(CY_HPPASS_CHAN_VALID(firCfg->chan));
            CY_ASSERT_L3(CY_HPPASS_LIMIT_VALID(firCfg->limit));
            CY_ASSERT_L3(CY_HPPASS_FIFO_VALID(firCfg->fifo));

            HPPASS_SAR_FIR_CFG(HPPASS_BASE, firIdx) = _VAL2FLD(HPPASS_SAR_CFG_FIR_CFG_CHANNEL_SEL, firCfg->chan) |
                                                      _VAL2FLD(HPPASS_SAR_CFG_FIR_CFG_WAIT_TAP_INIT, firCfg->waitTapInit) |
                                                      _VAL2FLD(HPPASS_SAR_CFG_FIR_CFG_RANGE_SEL, firCfg->limit) |
                                                      _VAL2FLD(HPPASS_SAR_CFG_FIR_CFG_FIFO_SEL, firCfg->fifo);

            CY_REG32_CLR_SET(HPPASS_SAR_RESULT_MASK(HPPASS_BASE), resultMask, CY_HPPASS_BOOL_TO_UINT(firCfg->result));

            for (uint8_t i = 0U; i < CY_HPPASS_FIR_TAP_NUM; i++)
            {
                HPPASS_SAR_FIR_COEFS(HPPASS_BASE, firIdx, i) = (uint32_t)firCfg->coef[i];
            }
        }
        else
        {
            result = CY_HPPASS_SAR_BAD_PARAM;
        }
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_FirRead
****************************************************************************//**
*
* Returns the the ADC FIR filter result.
*
* \param firIdx
* The index of the FIR filter instance. Valid range 0..1.
*
* \return FIR filter result
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_FIR_READ
*
*******************************************************************************/
__STATIC_INLINE int16_t Cy_HPPASS_SAR_Result_FirRead(uint8_t firIdx)
{
    CY_ASSERT_L1(CY_HPPASS_FIR_NUM_VALID(firIdx));

    return (int16_t)*CY_HPPASS_SAR_FIR_RSLT_PTR(firIdx);
}

/** \} group_hppass_sar_fir */


/**
* \addtogroup group_hppass_sar_chan
* \{
*/


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_ChannelConfig
****************************************************************************//**
*
* Initializes the ADC channel.
*
* \note If AC is running this configuration is not allowed
* (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
*
* \note If the SAR block is currently busy the initialization is not allowed
* (\ref CY_HPPASS_SAR_INVALID_STATE will be returned).
*
* \note Since the 'block ready' status is combined SAR and CSG, this particular
* SAR function cannot be protected from reinitialization during the
* 'block ready' == true condition, so it is on user's responsibility when
* and from where to call this function to reconfigure this SAR sub-block.
*
* \param chanIdx
* The index of the channel. Valid range 0..27.
*
* \param chanCfg
* Pointer to the channel configuration structure \ref cy_stc_hppass_sar_chan_t.
* The pointer cannot be NULL.
*
* \return
* Status, \ref cy_en_hppass_status_t.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_basic/main.c SNIPPET_HPPASS_SAR_CHAN_CFG
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_SAR_ChannelConfig(uint8_t chanIdx, const cy_stc_hppass_sar_chan_t * chanCfg)
{
    cy_en_hppass_status_t result = Cy_HPPASS_SAR_CondBusyCheck();

    if (CY_HPPASS_SUCCESS == result)
    {
        if ((NULL != chanCfg) &&
            (CY_HPPASS_SAR_CHAN_NUM_VALID(chanIdx)) &&
            (!chanCfg->diff || (CY_HPPASS_SAR_AVG_DISABLED == chanCfg->avg)))
        {
            uint32_t resultMask_Pos = chanIdx;
            uint32_t resultMask_Msk = CY_HPPASS_SAR_CHAN_0_MSK << resultMask_Pos;

            CY_ASSERT_L3(CY_HPPASS_AVG_VALID(chanCfg->avg));

            HPPASS_SAR_CHAN_CFG(HPPASS_BASE, chanIdx) = _VAL2FLD(HPPASS_SAR_CFG_CHAN_CFG_DIFFERENTIAL_EN, chanCfg->diff) |
                                                        _VAL2FLD(HPPASS_SAR_CFG_CHAN_CFG_SIGNED, chanCfg->sign) |
                                                        _VAL2FLD(HPPASS_SAR_CFG_CHAN_CFG_RANGE_SEL, chanCfg->limit) |
                                                        _VAL2FLD(HPPASS_SAR_CFG_CHAN_CFG_AVG_SEL, chanCfg->avg) |
                                                        _VAL2FLD(HPPASS_SAR_CFG_CHAN_CFG_FIFO_SEL, chanCfg->fifo);

            CY_REG32_CLR_SET(HPPASS_SAR_RESULT_MASK(HPPASS_BASE), resultMask, CY_HPPASS_BOOL_TO_UINT(chanCfg->result));
        }
        else
        {
            result = CY_HPPASS_SAR_BAD_PARAM;
        }
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_GetStatus
****************************************************************************//**
*
* Returns the combined status of all the ADC channels and FIRs results.
*
* \return The ADC channels and FIRs results status \ref group_hppass_sar_status.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_RESULT_GET_STATUS
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Result_GetStatus(void)
{
    return HPPASS_SAR_RESULT_STATUS(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_ClearStatus
****************************************************************************//**
*
* Clears all the ADC channels and FIRs results status.
*
* \param mask
* The channels and FIRs status mask \ref group_hppass_sar_status.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_RESULT_CLR_STATUS
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_Result_ClearStatus(uint32_t mask)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(mask, CY_HPPASS_SAR_CHAN_MSK));

    HPPASS_SAR_RESULT_STATUS(HPPASS_BASE) = CY_HPPASS_SAR_CHAN_MSK & mask;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_ChannelRead
****************************************************************************//**
*
* Returns the ADC channel results.
*
* \param chanIdx
* The index of the channel instance. Valid range 0..27.
* \ref group_hppass_sar_chan_idx except FIR items.
*
* \return The ADC channels results.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_basic/main.c SNIPPET_HPPASS_SAR_CHAN_READ
*
*******************************************************************************/
__STATIC_INLINE int16_t Cy_HPPASS_SAR_Result_ChannelRead(uint8_t chanIdx)
{
    CY_ASSERT_L1(CY_HPPASS_SAR_CHAN_NUM_VALID(chanIdx));

    return (int16_t)*CY_HPPASS_SAR_CHAN_RSLT_PTR(chanIdx);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_ChannelPackedRead
****************************************************************************//**
*
* Returns the specified adjacent even/odd channel results from the channel
* result packed register.
*
* \param pairIdx
* The index of the adjacent even/odd channels pair instance. Valid range 0..13.
*
* \return The ADC even (low two bytes) and odd (high two bytes) channels results.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_RESULT_CHAN_PACK_READ
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Result_ChannelPackedRead(uint8_t pairIdx)
{
    CY_ASSERT_L1((CY_HPPASS_SAR_CHAN_NUM / 2U) > pairIdx);

    return HPPASS_SAR_CHAN_RESULT_PACKED(HPPASS_BASE, pairIdx);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_GetOverflowStatus
****************************************************************************//**
*
* Returns the ADC groups and FIRs result overflow status.
*
* \return The ADC groups and FIRs results status \ref group_hppass_sar_result_interrupts
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_RESULT_GET_OVF_STATUS
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Result_GetOverflowStatus(void)
{
    return HPPASS_SAR_RESULT_OVERFLOW(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_ClearOverflowStatus
****************************************************************************//**
*
* Clears the ADC groups and FIRs result overflow status.
*
* \param mask
* The groups and FIRs result overflow status mask \ref group_hppass_sar_result_interrupts
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_RESULT_CLR_OVF_STATUS
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_Result_ClearOverflowStatus(uint32_t mask)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(mask, CY_HPPASS_INTR_SAR_RESULT));

    HPPASS_SAR_RESULT_OVERFLOW(HPPASS_BASE) = CY_HPPASS_INTR_SAR_RESULT & mask;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Limit_GetStatus
****************************************************************************//**
*
* Returns the combined limit crossing detection status of all the ADC and FIR channels.
*
* \return The limit crossing detection status \ref group_hppass_sar_status
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_LIM_GET_STATUS
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Limit_GetStatus(void)
{
    return HPPASS_SAR_LIMIT_STATUS(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Limit_ClearStatus
****************************************************************************//**
*
* Clears the limit crossing detection status for all the ADC and FIR channels.
*
* \param mask
* The limit crossing detection status mask \ref group_hppass_sar_status
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_LIM_CLR_STATUS
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_Limit_ClearStatus(uint32_t mask)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(mask, CY_HPPASS_SAR_CHAN_MSK));

    HPPASS_SAR_LIMIT_STATUS(HPPASS_BASE) = CY_HPPASS_SAR_CHAN_MSK & mask;
}

/** \} group_hppass_sar_chan */


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Limit_GetInterruptStatus
****************************************************************************//**
*
* Returns the limit crossing interrupt register status.
*
* \return
* The status of combined interrupt requests for the SAR
* \ref group_hppass_sar_limit_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_LIM_GET_INT_STATUS
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Limit_GetInterruptStatus(void)
{
    return HPPASS_SAR_LIMIT_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Limit_ClearInterrupt
****************************************************************************//**
*
* Clears the limit crossing interrupt for SAR.
*
* \param interrupt
* The mask of interrupts to clear. Each bit of this mask is responsible for
* clearing of the particular interrupt in the SAR
* \ref group_hppass_sar_limit_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_LIM_CLEAR_INT_STATUS
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_Limit_ClearInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_SAR_LIMIT));

    HPPASS_SAR_LIMIT_INTR(HPPASS_BASE) = CY_HPPASS_INTR_SAR_LIMIT & interrupt;

    /* This dummy reading is necessary here. It provides a guarantee that
    * interrupt is cleared at returning from this function.
    */
    (void)HPPASS_SAR_LIMIT_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Limit_SetInterrupt
****************************************************************************//**
*
* Sets a software interrupt request for the specified limit crossing detection block.
*
* \param interrupt
* The mask of interrupts to set. Each bit of this mask is responsible for
* triggering of the particular interrupt in the SAR
* \ref group_hppass_sar_limit_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_LIM_SET_INT
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_Limit_SetInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_SAR_LIMIT));

    HPPASS_SAR_LIMIT_INTR_SET(HPPASS_BASE) = CY_HPPASS_INTR_SAR_LIMIT & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Limit_SetInterruptMask
****************************************************************************//**
*
* Enables the interrupt for the specified limit crossing detection block.
*
* \param interrupt
* The mask of interrupts allowable for activation. Each bit of this mask
* allows triggering of the particular interrupt in the SAR
* \ref group_hppass_sar_limit_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_LIM_SET_INT_MSK
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_Limit_SetInterruptMask(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_SAR_LIMIT));

    HPPASS_SAR_LIMIT_INTR_MASK(HPPASS_BASE) = CY_HPPASS_INTR_SAR_LIMIT & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Limit_GetInterruptMask
****************************************************************************//**
*
* Returns the interrupt mask for the specified limit crossing detection block.
*
* \return
* The mask of the interrupts allowable for activation
* \ref group_hppass_sar_limit_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_LIM_GET_INT_MSK
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Limit_GetInterruptMask(void)
{
    return HPPASS_SAR_LIMIT_INTR_MASK(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Limit_GetInterruptStatusMasked
****************************************************************************//**
*
* Gets limit crossing interrupt masked status.
*
* \return
* The masked status of combined interrupt requests for the SAR
* \ref group_hppass_sar_limit_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_LIM_GET_INT_MSKED
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Limit_GetInterruptStatusMasked(void)
{
    return HPPASS_SAR_LIMIT_INTR_MASKED(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_GetInterruptStatus
****************************************************************************//**
*
* Gets the SAR result interrupt status (ENTRY done, FIR done).
*
* \return
* The status of combined interrupt requests for the SAR
* \ref group_hppass_sar_result_interrupts.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_basic/main.c SNIPPET_HPPASS_SAR_GET_INT_STATUS
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Result_GetInterruptStatus(void)
{
    return HPPASS_SAR_RESULT_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_ClearInterrupt
****************************************************************************//**
*
* Clears SAR result interrupt
*
* \param interrupt
* The mask of interrupts to clear. Each bit of this mask is responsible for
* clearing of the particular interrupt in the SAR
* \ref group_hppass_sar_result_interrupts.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_basic/main.c SNIPPET_HPPASS_SAR_CLEAR_INT
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_Result_ClearInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_SAR_RESULT));

    HPPASS_SAR_RESULT_INTR(HPPASS_BASE) = CY_HPPASS_INTR_SAR_RESULT & interrupt;

    /* This dummy reading is necessary here. It provides a guarantee that
    * interrupt is cleared at returning from this function.
    */
    (void)HPPASS_SAR_RESULT_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_SetInterrupt
****************************************************************************//**
*
* Sets a software SAR result interrupt request.
*
* \param interrupt
* The mask of interrupts to set. Each bit of this mask is responsible for
* triggering of the particular interrupt in the SAR
* \ref group_hppass_sar_result_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_RESULT_SET_INT
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_Result_SetInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_SAR_RESULT));

    HPPASS_SAR_RESULT_INTR_SET(HPPASS_BASE) = CY_HPPASS_INTR_SAR_RESULT & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_SetInterruptMask
****************************************************************************//**
*
* Enables the SAR result interrupt.
*
* \param interrupt
* The mask of interrupts allowable for activation. Each bit of this mask
* allows triggering of the particular interrupt in the SAR
* \ref group_hppass_sar_result_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_RESULT_SET_INT_MSK
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_Result_SetInterruptMask(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_SAR_RESULT));

    HPPASS_SAR_RESULT_INTR_MASK(HPPASS_BASE) = CY_HPPASS_INTR_SAR_RESULT & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_GetInterruptMask
****************************************************************************//**
*
* Gets the SAR result interrupt mask.
*
* \return
* The mask of the interrupts allowable for activation
* \ref group_hppass_sar_result_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_RESULT_GET_INT_MSK
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Result_GetInterruptMask(void)
{
    return HPPASS_SAR_RESULT_INTR_MASK(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_Result_GetInterruptStatusMasked
****************************************************************************//**
*
* Gets the SAR result interrupt masked status.
*
* \return
* The masked status of combined interrupt requests for the SAR
* \ref group_hppass_sar_result_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_RESULT_GET_INT_MSKED
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_SAR_Result_GetInterruptStatusMasked(void)
{
    return HPPASS_SAR_RESULT_INTR_MASKED(HPPASS_BASE);
}


/**
* \addtogroup group_hppass_sar_fifo
* \{
*/


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_Config
****************************************************************************//**
*
* Initializes the FIFO buffer.
*
* \note If AC is running this configuration is not allowed
* (\ref CY_HPPASS_AC_INVALID_STATE will be returned).
*
* \note If the SAR block is currently busy the initialization is not allowed
* (\ref CY_HPPASS_SAR_INVALID_STATE will be returned).
*
* \note Since the 'block ready' status is combined SAR and CSG, this particular
* SAR function cannot be protected from reinitialization during the
* 'block ready' == true condition, so it is on user's responsibility when
* and from where to call this function to reconfigure this SAR sub-block.
*
* \param fifoCfg
* Pointer to the FIFO buffer structure \ref cy_stc_hppass_fifo_t.
* The pointer cannot be NULL.
*
* \return
* Status, \ref cy_en_hppass_status_t.
*
* \funcusage
*
* \snippet test_apps/hppass_sar_fir_fifo/main.c SNIPPET_HPPASS_SAR_FIFO_CFG_STR
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_CFG
*
*******************************************************************************/
__STATIC_INLINE cy_en_hppass_status_t Cy_HPPASS_FIFO_Config(const cy_stc_hppass_fifo_t * fifoCfg)
{
    cy_en_hppass_status_t result = Cy_HPPASS_SAR_CondBusyCheck();

    CY_ASSERT_L3(CY_HPPASS_MODE_VALID(fifoCfg->mode));
    CY_ASSERT_L2(CY_HPPASS_TRIGMSK_VALID(fifoCfg->trigMsk));

    if (CY_HPPASS_SUCCESS == result)
    {
        if (NULL == fifoCfg)
        {
            result = CY_HPPASS_SAR_BAD_PARAM;
        }
        else
        {
            HPPASS_MMIO_FIFO_CFG(HPPASS_BASE) = _VAL2FLD(HPPASS_MMIO_FIFO_CFG_SEL, fifoCfg->mode) |
                                                _VAL2FLD(HPPASS_MMIO_FIFO_CFG_FIFO_TR_EN, fifoCfg->trigMsk);

            for (uint8_t i = 0U; i < CY_HPPASS_FIFO_NUM; i++)
            {
                CY_ASSERT_L3(CY_HPPASS_LEVEL_VALID(fifoCfg->mode, fifoCfg->level[i]));

                HPPASS_MMIO_FIFO_LEVEL(HPPASS_BASE, i) = _VAL2FLD(HPPASS_MMIO_FIFO_LEVEL_LEVEL, (uint32_t)fifoCfg->level[i] - 1UL);
            }
        }
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_GetSampleCount
****************************************************************************//**
*
* Returns the number of accumulated data words.
*
* \param fifoIdx
* The index of the FIFO part. Valid range 0..3,
* but depends on \ref cy_stc_hppass_fifo_t::mode setting.
*
* \return the number of accumulated data words.
* If fifoIdx is out of range then zero is returned.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_GET_SMP_CNT
*
*******************************************************************************/
__STATIC_INLINE uint8_t Cy_HPPASS_FIFO_GetSampleCount(uint8_t fifoIdx)
{
    uint8_t result = 0U;

    if (CY_HPPASS_FIFO_NUM_VALID(fifoIdx))
    {
        result = (uint8_t)HPPASS_MMIO_FIFO_USED(HPPASS_BASE, fifoIdx);
    }
    else
    {
        CY_ASSERT_L1(false); /* The fifoIdx is invalid */
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_Read
****************************************************************************//**
*
* Returns the FIFO data word and the channel index
*
* \param fifoIdx
* The index of the FIFO part. Valid range 0..3,
* but depends on \ref cy_stc_hppass_fifo_t::mode setting
*
* \param chanIdx
* The pointer to the variable to store channel index
* \ref group_hppass_sar_chan_idx.
* This is optional - the pointer may be NULL
* (if channel index is not needed).
*
* \return the FIFO data word
* If fifoIdx is out of range then zero is returned.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_READ
*
*******************************************************************************/
__STATIC_INLINE int16_t Cy_HPPASS_FIFO_Read(uint8_t fifoIdx, uint8_t * chanIdx)
{
    int16_t result = 0;

    if (CY_HPPASS_FIFO_NUM_VALID(fifoIdx))
    {
        uint32_t rdData = *CY_HPPASS_SAR_FIFO_READ_PTR(fifoIdx);

        result = (int16_t)(uint16_t) _FLD2VAL(CY_HPPASS_FIFO_RD_DATA_RESULT, rdData);

        if (chanIdx != NULL)
        {
            * chanIdx = (uint8_t) _FLD2VAL(CY_HPPASS_FIFO_RD_DATA_CHAN_ID, rdData);
        }
    }
    else
    {
        CY_ASSERT_L1(false); /* The fifoIdx is invalid */
    }

    return result;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_ReadAll
****************************************************************************//**
*
* Loads all the FIFO payload into the array.
*
* \param fifoIdx
* The index of the FIFO buffer. Valid range 0..3,
* but depends on \ref cy_stc_hppass_fifo_t::mode setting
*
* \param data
* Pointer to the data array
* User is responsible for enough array size,
* it depends on \ref cy_stc_hppass_fifo_t::mode setting
*
* \param chanIdxs
* Pointer to the array to store channel indexes
* \ref group_hppass_sar_chan_idx.
* User is responsible for enough array size,
* it depends on \ref cy_stc_hppass_fifo_t::mode setting
* Also this is optional - the pointer may be NULL
* (if channel indexes are not needed).
*
* \return the number of loaded data words
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_READ_ALL
*
*******************************************************************************/
uint8_t Cy_HPPASS_FIFO_ReadAll(uint8_t fifoIdx, int16_t * data, uint8_t * chanIdxs);


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_GetInterruptStatus
****************************************************************************//**
*
* Gets the interrupt register status for the specified FIFO.
*
* \return
* The status of combined interrupt requests for the FIFO
* \ref group_hppass_fifo_level_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_GET_INT_STATUS
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_FIFO_GetInterruptStatus(void)
{
    return HPPASS_FIFO_INTR(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_SetInterrupt
****************************************************************************//**
*
* Sets a software interrupt request for the specified FIFO.
*
* \note This function works slightly different than any regular _SetInterrupt
* function (for example \ref Cy_HPPASS_SetInterrupt), because of
* different HPPASS_FIFO_INTR/INTR_SET level-based register behavior.
* See TRM for details.
*
* \param interrupt
* The mask of interrupts to be set. Each bit of this mask is responsible for
* triggering of the particular interrupt in the FIFO
* \ref group_hppass_fifo_level_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_SET_INT
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_FIFO_SetInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_FIFO));

    HPPASS_FIFO_INTR_SET(HPPASS_BASE) |= (CY_HPPASS_INTR_FIFO & interrupt);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_ClearInterrupt
****************************************************************************//**
*
* Clears the interrupt for the specified FIFO.
*
* \note This function works slightly different than any regular _ClearInterrupt
* function (for example \ref Cy_HPPASS_ClearInterrupt), because of
* different HPPASS_FIFO_INTR/INTR_SET level-based registers behavior.
* See TRM for details.
*
* \param interrupt
* The mask of interrupts to be cleared. Each bit of this mask is responsible for
* triggering of the particular interrupt in the FIFO
* \ref group_hppass_fifo_level_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_CLR_INT
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_FIFO_ClearInterrupt(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_FIFO));

    HPPASS_FIFO_INTR_SET(HPPASS_BASE) &= ~(CY_HPPASS_INTR_FIFO & interrupt);

    /* This dummy reading is necessary here. It provides a guarantee that
    * interrupt is cleared at returning from this function.
    */
    (void)HPPASS_FIFO_INTR_SET(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_SetInterruptMask
****************************************************************************//**
*
* Enables the interrupt for the specified FIFO.
*
* \param interrupt
* The mask of interrupts allowable for activation. Each bit of this mask
* allows triggering of the particular interrupt in the FIFO
* \ref group_hppass_fifo_level_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_SET_INT_MSK
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_FIFO_SetInterruptMask(uint32_t interrupt)
{
    CY_ASSERT_L2(CY_HPPASS_MASK_VALID(interrupt, CY_HPPASS_INTR_FIFO));

    HPPASS_FIFO_INTR_MASK(HPPASS_BASE) = CY_HPPASS_INTR_FIFO & interrupt;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_GetInterruptMask
****************************************************************************//**
*
* Gets the interrupt mask for the specified FIFO.
*
* \return
* The mask of the interrupts allowable for activation
* \ref group_hppass_fifo_level_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_GET_INT_MSK
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_FIFO_GetInterruptMask(void)
{
    return HPPASS_FIFO_INTR_MASK(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_FIFO_GetInterruptStatusMasked
****************************************************************************//**
*
* Gets all FIFOs interrupt masked status.
*
* \return
* The masked status of combined interrupt requests for the FIFO
* \ref group_hppass_fifo_level_interrupts.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_FIFO_GET_INT_MSKED
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_HPPASS_FIFO_GetInterruptStatusMasked(void)
{
    return HPPASS_FIFO_INTR_MASKED(HPPASS_BASE);
}

/** \} group_hppass_sar_fifo */

/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_SetSwitch
****************************************************************************//**
*
* Sets selected AROUTE switch closed. Used for FW control input connections of
* the muxed samplers.
*
* \note This function works only if cy_stc_hppass_sar_t::muxMode is set to
* CY_HPPASS_SAR_MUX_FW for selected mux. See TRM for details.
*
* \param muxMask
* The mask of AROUTE switch.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_SET_SWITCH
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_SetSwitch(uint16_t muxMask)
{
    HPPASS_SAR_AROUTE_FW_CTRL(HPPASS_BASE) = (uint32_t)muxMask;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_ClearSwitch
****************************************************************************//**
*
* Clears selected AROUTE switch open. Used for FW control input connections of
* the muxed samplers.
*
* \note This function works only if cy_stc_hppass_sar_t::muxMode is set to
* CY_HPPASS_SAR_MUX_FW for selected mux. See TRM for details.
*
* \param muxMask
* The mask of AROUTE switch.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_CLEAR_SWITCH
*
*******************************************************************************/
__STATIC_INLINE void Cy_HPPASS_SAR_ClearSwitch(uint16_t muxMask)
{
    HPPASS_SAR_AROUTE_FW_CTRL_CLR(HPPASS_BASE) = (uint32_t)muxMask;
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_GetSwitchStatus
****************************************************************************//**
*
* Reads the current state of the AROUTE switch.
*
* \return
* Read back the current state of the associated switch control.
*
* \funcusage
*
* \snippet hppass/snippet/hppass_sar.c SNIPPET_HPPASS_SAR_GET_SW_STATUS
*
*******************************************************************************/
__STATIC_INLINE uint16_t Cy_HPPASS_SAR_GetSwitchStatus(void)
{
    return (uint16_t)HPPASS_SAR_AROUTE_STATUS(HPPASS_BASE);
}


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_CountsTo_Volts
****************************************************************************//**
*
* Converts the raw ADC result in counts into the analog pin voltage in volts.
* For example, for the 0.534-volt ADC, the return value is 0.534.
* Voltage calculation depends on ADC channel work modes: Averaging,
* Signed, Pseudo-Differential, and Sampler Gain.
*
* The equation:
*
*     V = ((chanCounts + chanOffset * avgMultiplier) * vRef / 1000) / (4095 * avgMultiplier * samplerGain)
*
* where,
* - chanCounts: the raw counts from the SAR 16-bit CHAN_RESULT register
* - avgMultiplier: the multiplier value based on Averaging mode (\ref cy_stc_hppass_sar_chan_t) and the number of samples averaged
*   (\ref cy_en_hppass_sar_avg_t)
*   - \ref CY_HPPASS_SAR_AVG_DISABLED : avgMultiplier is 1<<0
*   - \ref CY_HPPASS_SAR_AVG_16 : avgMultiplier is 1<<4
* - chanOffset: the constant value based on Signed and Differential modes \ref cy_stc_hppass_sar_chan_t
*   - for Signed mode - true and Differential mode - disabled, chanOffset is 2048
*   - for Signed mode - false and Differential mode - enabled, chanOffset is -2048
*   - for all other cases, chanOffset is 0
* - vRef: the SAR ADC reference voltage in millivolts (for example, 3300U).
*
* \note
* This function is valid only for the right-aligned result.
*
* \param chanIdx
* The channel index . Valid range - 0..27.
*
* \param vRef
* The SAR ADC reference voltage in millivolts
*
* \param chanCounts
* Conversion result from \ref Cy_HPPASS_SAR_Result_ChannelRead
*
* \return
* The result in Volts.
*
* \funcusage
*
*******************************************************************************/
float32_t Cy_HPPASS_SAR_CountsTo_Volts(uint8_t chanIdx, uint16_t vRef, int16_t chanCounts);


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_CountsTo_mVolts
****************************************************************************//**
*
* Converts the raw ADC result in counts into the analog pin voltage in millivolts.
* For example, for the 0.534-volts ADC, the return value is 534.
* Voltage calculation depends on ADC channel work modes: Averaging,
* Signed, Pseudo-Differential, and Sampler Gain.
*
* The equation:
*
*     mV = ((chanCounts + chanOffset * avgMultiplier) * vRef) / (4095 * avgMultiplier * samplerGain)
*
* where,
* - chanCounts: the raw counts from the SAR 16-bit CHAN_RESULT register
* - avgMultiplier: the multiplier value based on Averaging mode (\ref cy_stc_hppass_sar_chan_t) and the number of samples averaged
*   (\ref cy_en_hppass_sar_avg_t)
*   - \ref CY_HPPASS_SAR_AVG_DISABLED : avgMultiplier is 1<<0
*   - \ref CY_HPPASS_SAR_AVG_16 : avgMultiplier is 1<<4
* - chanOffset: the constant value based on Signed and Differential modes \ref cy_stc_hppass_sar_chan_t
*   - for Signed mode - true and Differential mode - disabled, chanOffset is 2048
*   - for Signed mode - false and Differential mode - enabled, chanOffset is -2048
*   - for all other cases, chanOffset is 0
* - vRef: the SAR ADC reference voltage in millivolts (for example, 3300U).
*
* \note
* This function is valid only for the right-aligned result.
*
* \param chanIdx
* The channel index . Valid range - 0..27.
*
* \param vRef
* The SAR ADC reference voltage in millivolts
*
* \param chanCounts
* Conversion result from \ref Cy_HPPASS_SAR_Result_ChannelRead
*
* \return
* The result in millivolts.
*
* \funcusage
*
*******************************************************************************/
int16_t Cy_HPPASS_SAR_CountsTo_mVolts(uint8_t chanIdx, uint16_t vRef, int16_t chanCounts);


/*******************************************************************************
* Function Name: Cy_HPPASS_SAR_CountsTo_uVolts
****************************************************************************//**
*
* Converts the raw ADC result in counts into the analog pin voltage in microvolts.
* For example, for the 0.534-volts ADC, the return value is 534000.
* Voltage calculation depends on ADC channel work modes: Averaging,
* Signed, Pseudo-Differential, and Sampler Gain.
*
* The equation:
*
*     uV = ((chanCounts + chanOffset * avgMultiplier) * vRef * 1000)/ (4095 * avgMultiplier * samplerGain)
*
* where,
* - chanCounts: the raw counts from the SAR 16-bit CHAN_RESULT register
* - avgMultiplier: the multiplier value based on Averaging mode (\ref cy_stc_hppass_sar_chan_t) and the number of samples averaged
*   (\ref cy_en_hppass_sar_avg_t)
*   - \ref CY_HPPASS_SAR_AVG_DISABLED : avgMultiplier is 1<<0
*   - \ref CY_HPPASS_SAR_AVG_16 : avgMultiplier is 1<<4
*   - \ref CY_HPPASS_SAR_AVG_32 : avgMultiplier is 1<<4 in Signed mode and 1<<3 in Unsigned mode
* - chanOffset: the constant value based on Signed and Differential modes \ref cy_stc_hppass_sar_chan_t
*   - for Signed mode - true and Differential mode - disabled, chanOffset is 2048
*   - for Signed mode - false and Differential mode - enabled, chanOffset is -2048
*   - for all other cases, chanOffset is 0
* - vRef: the SAR ADC reference voltage in millivolts (for example, 3300U).
*
* \note
* This function is valid only for the right-aligned result.
*
* \param chanIdx
* The channel index . Valid range - 0..27.
*
* \param vRef
* The SAR ADC reference voltage in millivolts
*
* \param chanCounts
* Conversion result from \ref Cy_HPPASS_SAR_Result_ChannelRead
*
* \return
* The result in microvolts.
*
* \funcusage
*
*******************************************************************************/
int32_t Cy_HPPASS_SAR_CountsTo_uVolts(uint8_t chanIdx, uint16_t vRef, int16_t chanCounts);

/** \} group_hppass_sar_functions */

#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXS40MCPASS */

#endif /** !defined(CY_HPPASS_SAR_H) */

/** \} group_hppass_sar */

/* [] END OF FILE */
