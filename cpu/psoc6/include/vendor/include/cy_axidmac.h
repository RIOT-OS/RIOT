/***************************************************************************//**
* \file cy_axidmac.h
* \version 1.0
*
* \brief
* The header file of the AXIDMAC driver.
*
********************************************************************************
* \copyright
* Copyright 2018-2021 Cypress Semiconductor Corporation
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
* \addtogroup group_axidmac
* \{
* Configures the AXIDMA Controller block, channels and descriptors.
*
* The functions and other declarations used in this driver are in cy_axidmac.h.
* You can include cy_pdl.h to get access to all functions
* and declarations in the PDL.
*
* The AXIDMA Controller channel can be used in any project to transfer data
* without CPU intervention basing on a hardware trigger signal from another component.
*
* The AXIDMA Controller block has a set of registers, a base hardware address,
* and supports multiple channels. Many API functions for the AXIDMAC driver require
* a base hardware address and channel number.
* Ensure that you use the correct hardware address for the AXIDMA Controller block in use.
*
* Features:
* * Multiple channels (device specific).
* * Four priority levels for each channel.
* * Descriptor chaining.
* * Configurable source and destination addresses.
* * 1D memory copy, 2D memory copy, 3D memory copy descriptor types are supported.
* * Configurable input/output triggers and interrupts.
*
* \section group_axidmac_configuration Configuration Considerations
*
* To set up a AXIDMAC driver, initialize a descriptor,
* initialize and enable a channel, and enable the AXIDMAC block.
*
* To set up a descriptor, provide the configuration parameters for the
* descriptor in the \ref cy_stc_axidmac_descriptor_config_t structure. Then call the
* \ref Cy_AXIDMAC_Descriptor_Init function to initialize the descriptor in SRAM. You can
* modify the source and destination addresses dynamically by calling
* \ref Cy_AXIDMAC_Descriptor_SetSrcAddress and \ref Cy_AXIDMAC_Descriptor_SetDstAddress.
*
* To set up a AXIDMAC channel, provide a filled \ref cy_stc_axidmac_channel_config_t
* structure. Call the \ref Cy_AXIDMAC_Channel_Init function, specifying the channel
* number. Use \ref Cy_AXIDMAC_Channel_Enable to enable the configured AXIDMAC channel.
*
* Call \ref Cy_AXIDMAC_Channel_Enable for each AXIDMAC channel in use.
*
* When configured, another peripheral typically triggers the AXIDMAC channel. The trigger is
* connected to the AXIDMAC channel using the trigger multiplexer. The trigger multiplexer
* driver has a software trigger you can use in firmware to trigger the AXIDMAC channel. See the
* <a href="group__group__trigmux.html">Trigger Multiplexer</a> documentation.
*
* The following is a simplified structure of the AXIDMAC driver API interdependencies
* in a typical user application:
* \image html axidmac.png
*
* <B>NOTE:</B> Even if a AXIDMAC channel is enabled, it is not operational until
* the AXIDMAC block is enabled using function \ref Cy_AXIDMAC_Enable.\n
* <B>NOTE:</B> If the AXIDMAC descriptor is configured to generate an interrupt,
* the interrupt must be enabled using the \ref Cy_AXIDMAC_Channel_SetInterruptMask
* function for each AXIDMAC channel.
*
* <B>Scenario</B>: AXIDMAC with DMA descriptors, DMA config descriptors, source and destination buffer stored in memory
* <B>other then DTCM</B>(CM55 private MEMORY)  such as SOCMEM or FLASH
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Enable
* <B>Scenario</B>: AXIDMAC with DMA descriptors, DMA config descriptors, source and destination buffer stored in <B>DTCM memory</B>
* (CM55 private MEMORY), in this case address need to be remap to external master mapped address
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Enable_DTCM_MEM
*
* \section group_axidmac_more_information More Information.
* See the AXIDMAC chapter of the device technical reference manual (TRM).
*
* \section group_axidmac_changelog Changelog
*
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.0</td>
*     <td>The initial version.</td>
*     <td></td>
*   </tr>
* </table>

* 
* \defgroup group_axidmac_macros Macros
* \defgroup group_axidmac_macros_interrupt_masks Interrupt Masks
* \defgroup group_axidmac_functions Functions
* \{
* \defgroup group_axidmac_block_functions Block Functions
* \defgroup group_axidmac_channel_functions Channel Functions
* \defgroup group_axidmac_descriptor_functions Descriptor Functions
* \}
* \defgroup group_axidmac_data_structures Data Structures
* \defgroup group_axidmac_enums Enumerated Types
*/

#if !defined (CY_AXIDMAC_H)
#define CY_AXIDMAC_H

#include "cy_device.h"

#if defined (CY_IP_MXSAXIDMAC) || defined (CY_IP_MXAXIDMAC)

#include "cy_syslib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 10.1', 5, \
'The operand "" to the operator "&" does not have an essentially unsigned type.')
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 10.3', 5, \
'Implicit conversion from essential type "unsigned 32-bit int" to different or narrower essential type "signed 16-bit int".')
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 10.4', 5, \
'Essential type of the left hand operand (signed) is not the same as that of the right operand (unsigned).')
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 10.8', 5, \
'Value extracted from _VAL2FLD macro will not exceed enum range.')

/******************************************************************************
 * Macro definitions                                                          *
 ******************************************************************************/

/**
* \addtogroup group_axidmac_macros
* \{
*/

/** The driver major version */
#define CY_AXIDMAC_DRV_VERSION_MAJOR       1

/** The driver minor version */
#define CY_AXIDMAC_DRV_VERSION_MINOR       0

/** The AXIDMAC driver identifier */
#define CY_AXIDMAC_ID                      (CY_PDL_DRV_ID(0x4BU))

/** The minimum M/X/Y Count API parameters */
#define CY_AXIDMAC_LOOP_COUNT_MIN          (1UL)
/** The maximum M/X/Y Count API parameters */
#define CY_AXIDMAC_LOOP_COUNT_MAX          (65536UL)

/** The minimum M/X/Y Increment API parameters */
#define CY_AXIDMAC_LOOP_INCREMENT_MIN      (-32768L)
/** The maximum M/X/Y Increment API parameters */
#define CY_AXIDMAC_LOOP_INCREMENT_MAX      (32767L)

/**
* \addtogroup group_axidmac_macros_interrupt_masks Interrupt Masks
* \{
*/

/** Bit 0: Completion of data transfer(s) as specified by the descriptor's interruptType setting. */
#define CY_AXIDMAC_INTR_COMPLETION         (AXI_DMAC_CH_INTR_COMPLETION_Msk)
/** Bit 1: Bus error for a load from the source. */
#define CY_AXIDMAC_INTR_SRC_BUS_ERROR      (AXI_DMAC_CH_INTR_SRC_BUS_ERROR_Msk)
/** Bit 2: Bus error for a store to the destination. */
#define CY_AXIDMAC_INTR_DST_BUS_ERROR      (AXI_DMAC_CH_INTR_DST_BUS_ERROR_Msk)
/** Bit 3: Invalid descriptor type. */
#define CY_AXIDMAC_INTR_INVALID_DESCR_TYPE  (AXI_DMAC_CH_INTR_INVALID_DESCR_TYPE_Msk)
/** Bit 5: The channel is enabled and the current descriptor pointer is "0". */
#define CY_AXIDMAC_INTR_CURR_PTR_NULL      (AXI_DMAC_CH_INTR_CURR_PTR_NULL_Msk)
/** Bit 6: The channel is disabled and the data transfer engine is busy. */
#define CY_AXIDMAC_INTR_ACTIVE_CH_DISABLED (AXI_DMAC_CH_INTR_ACTIVE_CH_DISABLED_Msk)
/** Bit 7: Bus error for a load of the descriptor. */
#define CY_AXIDMAC_INTR_DESCR_BUS_ERROR    (AXI_DMAC_CH_INTR_DESCR_BUS_ERROR_Msk)

/** \} group_axidmac_macros_interrupt_masks */

/** \} group_axidmac_macros */


/**
* \addtogroup group_axidmac_enums
* \{
*/

/** Contains the options for the descriptor type */
typedef enum
{
    CY_AXIDMAC_1D_MEMORY_COPY = 0UL,    /**< 1D Memory copy */
    CY_AXIDMAC_2D_MEMORY_COPY = 1UL,    /**< 2D Memory copy  */
    CY_AXIDMAC_3D_MEMORY_COPY = 2UL,    /**< 3D Memory copy  */
} cy_en_axidmac_descriptor_type_t;

/** Contains the options for the interrupt, trig-in and trig-out type parameters of the descriptor */
typedef enum
{
    CY_AXIDMAC_M_LOOP      = 0U,              /**< One M loop transfer.             */
    CY_AXIDMAC_X_LOOP      = 1U,              /**< One X loop transfer.              */
    CY_AXIDMAC_DESCR       = 2U,              /**< One descriptor transfer.          */
    CY_AXIDMAC_DESCR_CHAIN = 3U               /**< Entire descriptor chain transfer. */
} cy_en_axidmac_trigger_type_t;

/** Contains the options for descriptor retriggering */
typedef enum
{
    CY_AXIDMAC_RETRIG_IM      = 0U,           /**< Retrigger immediately.              */
    CY_AXIDMAC_RETRIG_4CYC    = 1U,           /**< Retrigger after 4 Clk_Slow cycles.  */
    CY_AXIDMAC_RETRIG_16CYC   = 2U,           /**< Retrigger after 16 Clk_Slow cycles. */
    CY_AXIDMAC_WAIT_FOR_REACT = 3U            /**< Wait for trigger reactivation.      */
} cy_en_axidmac_retrigger_t;


/** Contains the options for the state of the channel when the descriptor is completed   */
typedef enum
{
    CY_AXIDMAC_CHANNEL_ENABLED  = 0U,         /**< Channel stays enabled. */
    CY_AXIDMAC_CHANNEL_DISABLED = 1U          /**< Channel is disabled.   */
} cy_en_axidmac_channel_state_t;

/** Contains the return values of the AXIDMAC driver */
typedef enum
{
    CY_AXIDMAC_SUCCESS          = 0x0UL,      /**< Success. */
    CY_AXIDMAC_BAD_PARAM        = CY_AXIDMAC_ID | CY_PDL_STATUS_ERROR | 0x1UL /**< The input parameters passed to the AXIDMAC API are not valid. */
} cy_en_axidmac_status_t;

/** \} group_axidmac_enums */

/** \cond Macros for the conditions used by CY_ASSERT calls */

#define CY_AXIDMAC_IS_LOOP_COUNT_VALID(count)      (((count) >= CY_AXIDMAC_LOOP_COUNT_MIN) && ((count) <= CY_AXIDMAC_LOOP_COUNT_MAX))
#define CY_AXIDMAC_IS_LOOP_INCR_VALID(incr)        (((incr) >= CY_AXIDMAC_LOOP_INCREMENT_MIN) && ((incr) <= CY_AXIDMAC_LOOP_INCREMENT_MAX))
#define CY_AXIDMAC_IS_PRIORITY_VALID(prio)         ((prio) <= 3UL)
#define CY_AXIDMAC_INTR_MASK                       (CY_AXIDMAC_INTR_COMPLETION         | \
                                                 CY_AXIDMAC_INTR_SRC_BUS_ERROR      | \
                                                 CY_AXIDMAC_INTR_DST_BUS_ERROR      | \
                                                 CY_AXIDMAC_INTR_INVALID_DESCR_TYPE | \
                                                 CY_AXIDMAC_INTR_CURR_PTR_NULL      | \
                                                 CY_AXIDMAC_INTR_ACTIVE_CH_DISABLED | \
                                                 CY_AXIDMAC_INTR_DESCR_BUS_ERROR)

#define CY_AXIDMAC_IS_INTR_MASK_VALID(intr)      (0UL == ((intr) & ((uint32_t) ~CY_AXIDMAC_INTR_MASK)))

#define CY_AXIDMAC_IS_RETRIGGER_VALID(retrigger) ((CY_AXIDMAC_RETRIG_IM      == (retrigger)) || \
                                                 (CY_AXIDMAC_RETRIG_4CYC    == (retrigger)) || \
                                                 (CY_AXIDMAC_RETRIG_16CYC   == (retrigger)) || \
                                                 (CY_AXIDMAC_WAIT_FOR_REACT == (retrigger)))

#define CY_AXIDMAC_IS_TRIG_TYPE_VALID(trigType)  ((CY_AXIDMAC_M_LOOP    == (trigType)) || \
                                                 (CY_AXIDMAC_X_LOOP      == (trigType)) || \
                                                 (CY_AXIDMAC_DESCR       == (trigType)) || \
                                                 (CY_AXIDMAC_DESCR_CHAIN == (trigType)))

#define CY_AXIDMAC_IS_CHANNEL_STATE_VALID(state) ((CY_AXIDMAC_CHANNEL_ENABLED  == (state)) || \
                                                 (CY_AXIDMAC_CHANNEL_DISABLED == (state)))


#define CY_AXIDMAC_IS_TYPE_VALID(descrType)      ((CY_AXIDMAC_1D_MEMORY_COPY  == (descrType)) || \
                                                 (CY_AXIDMAC_2D_MEMORY_COPY      == (descrType)) || \
                                                 (CY_AXIDMAC_3D_MEMORY_COPY == (descrType)))

#define CY_AXIDMAC_IS_CH_NR_VALID(chNr)          (CY_AXIDMAC_CH_NR > (chNr))

/** \endcond */

/**
* \addtogroup group_axidmac_data_structures
* \{
*/


/**
* AXIDMAC descriptor structure type. It is a user-declared structure
* allocated in RAM. The AXIDMAC HW requires a pointer to this structure to work with it.
*
* For advanced users: the descriptor can be allocated even in flash, however the user
* have to predefine all the structure items with constants manually,
* because the descriptor "Set" API functions (including \ref Cy_AXIDMAC_Descriptor_Init())
* don't work with read-only descriptors.
*/
typedef struct
{
    uint32_t ctl;                    /*!< Descriptor control */
    uint32_t src;                    /*!< Descriptor source holds the base address of source location */
    uint32_t dst;                    /*!< Descriptor destination holds the base address of destination location*/
    uint32_t mSize;                  /*!< Descriptor memory copy size. Valid range is 0...16777216 */
    uint32_t xSize;                  /*!< Descriptor X loop size. Valid range is 0...65535 */
    int32_t xIncr;                   /*!< Descriptor X loop increment. Valid range is -32768...32767 */
    uint32_t ySize;                  /*!< Descriptor Y loop size. Valid range is 0...65535 */
    int32_t yIncr;                   /*!< Descriptor Y loop increment. Valid range is -32768...32767 */
    uint32_t nextPtr;                /*!< Descriptor next pointer */
} cy_stc_axidmac_descriptor_t;

/** \cond The next type-specific descriptor types are ONLY for internal API implementation. */

typedef cy_stc_axidmac_descriptor_t cy_stc_axidmac_dscr_3d_t;

typedef struct
{
    uint32_t ctl;                     /*!< Descriptor control */
    uint32_t src;                     /*!< Descriptor source holds the base address of source location */
    uint32_t dst;                     /*!< Descriptor destination holds the base address of destination location*/
    uint32_t mSize;                   /*!< Descriptor memory copy size. Valid range is 0...16777216 */
    uint32_t nextPtr;                 /*!< Descriptor next pointer */
} cy_stc_axidmac_dscr_1d_t;

typedef struct
{
    uint32_t ctl;                      /*!< Descriptor control */
    uint32_t src;                      /*!< Descriptor source holds the base address of source location */
    uint32_t dst;                      /*!< Descriptor destination holds the base address of destination location*/
    uint32_t mSize;                    /*!< Descriptor memory copy size. Valid range is 0...16777216 */
    uint32_t xSize;                    /*!< Descriptor X loop size. Valid range is 0...65535 */
    int32_t xIncr;                    /*!< Descriptor X loop increment. Valid range is -32768...32767 */
    uint32_t nextPtr;                  /*!< Descriptor next pointer */
} cy_stc_axidmac_dscr_2d_t;

/** \endcond */


/**
* This structure is a configuration structure pre-initialized by user
* and passed as a parameter to the \ref Cy_AXIDMAC_Descriptor_Init().
* It can be allocated in RAM/flash (on user's choice).
* In case of flash allocation there is a possibility to reinitialize the descriptor in runtime.
* This structure has all the parameters of the descriptor as separate parameters.
* Most of these parameters are represented in the \ref cy_stc_axidmac_descriptor_t structure as bit fields.
*/
typedef struct
{
    cy_en_axidmac_retrigger_t       retrigger;       /**< Specifies whether the AXIDMA controller should wait for the input trigger to be deactivated. */
    cy_en_axidmac_trigger_type_t    interruptType;   /**< Sets the event that triggers an interrupt. See \ref cy_en_axidmac_trigger_type_t. */
    cy_en_axidmac_trigger_type_t    triggerOutType;  /**< Sets the event that triggers an output. See \ref cy_en_axidmac_trigger_type_t. */
    cy_en_axidmac_channel_state_t   channelState;    /**< Specifies whether the channel is enabled or disabled on completion of descriptor see \ref cy_en_axidmac_channel_state_t. */
    cy_en_axidmac_trigger_type_t    triggerInType;   /**< Sets what type of transfer is triggered. See \ref cy_en_axidmac_trigger_type_t. */
    bool                            dataPrefetch;    /**< Source data transfers are initiated as soon as the channel is enabled, the current descriptor pointer is NOT "0"
                                                     *   and there is space available in the channel's data FIFO.
                                                     */
    cy_en_axidmac_descriptor_type_t descriptorType;  /**< The type of the descriptor. See \ref cy_en_axidmac_descriptor_type_t. */
    void *                          srcAddress;      /**< The source address of the transfer. */
    void *                          dstAddress;      /**< The destination address of the transfer. */
    uint32_t                        mCount;          /**< The number of bytes transfers in an M-loop */
    int16_t                         srcXincrement;   /**< The address increment of the source after each X-loop transfer. Valid range is -32768...32767. */
    int16_t                         dstXincrement;   /**< The address increment of the destination after each X-loop transfer. Valid range is -32768...32767. */
    uint32_t                        xCount;          /**< The number of transfers in an X-loop. Valid range (for all descriptors except scatter transfer) is 1...65536.
                                                     *   For memory copy descriptors, the X count is a number of bytes (not a data transfer size).
                                                     *   For scatter descriptors, the X count is a number of [address, data] pairs (two words each). Valid range is 1...32768.
                                                     */
    int16_t                         srcYincrement;   /**< The address increment of the source after each Y-loop transfer. Valid range is -32768...32767. */
    int16_t                         dstYincrement;   /**< The address increment of the destination after each Y-loop transfer. Valid range is -32768...32767. */
    uint32_t                        yCount;          /**< The number of X-loops in the Y-loop. Valid range is 1...65536. */
    cy_stc_axidmac_descriptor_t *   nextDescriptor;  /**< The next descriptor to chain after completion. A NULL value will signify no chaining. */
} cy_stc_axidmac_descriptor_config_t;

/** This structure holds the initialization values for the AXIDMAC channel */
typedef struct
{
    cy_stc_axidmac_descriptor_t * descriptor;     /**< The AXIDMAC descriptor associated with the channel being initialized.           */
    uint32_t priority;                         /**< This parameter specifies the channel's priority.                            */
    bool     enable;                           /**< This parameter specifies if the channel is enabled after initializing.      */
    bool     bufferable;                       /**< This parameter specifies whether a write transaction can complete.
                                                *   without waiting for the destination to accept the write transaction data.
                                                */
} cy_stc_axidmac_channel_config_t;

/** \} group_axidmac_data_structures */


/**
* \addtogroup group_axidmac_functions
* \{
*/

__STATIC_INLINE void     Cy_AXIDMAC_Enable              (AXI_DMAC_Type * base);
__STATIC_INLINE void     Cy_AXIDMAC_Disable             (AXI_DMAC_Type * base);
__STATIC_INLINE uint32_t Cy_AXIDMAC_GetAllChannelStatus (AXI_DMAC_Type * base);
#if defined (CY_IP_MXSAXIDMAC)
__STATIC_INLINE uint32_t Cy_AXIDMAC_GetActiveChannel    (AXI_DMAC_Type const * base);
#elif defined (CY_IP_MXAXIDMAC)
__STATIC_INLINE uint32_t Cy_AXIDMAC_GetActiveSecureChannel      (AXI_DMAC_Type const * base);
__STATIC_INLINE uint32_t Cy_AXIDMAC_GetActiveNonSecureChannel   (AXI_DMAC_Type const * base);
#endif

/**
* \addtogroup group_axidmac_channel_functions
* \{
*/

/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_Init
****************************************************************************//**
*
* Initializes the AXIDMAC channel with a descriptor and other parameters.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* A channel number.
*
* \param config
* The structure that has the initialization information for the
* channel.
*
* \return
* The status /ref cy_en_axidmac_status_t.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Enable
*
*******************************************************************************/
cy_en_axidmac_status_t Cy_AXIDMAC_Channel_Init                    (AXI_DMAC_Type       * base, uint32_t channel, cy_stc_axidmac_channel_config_t const * config);


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_DeInit
****************************************************************************//**
*
* Clears the content of registers corresponding to the channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* A channel number.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Disable
*
*******************************************************************************/
void     Cy_AXIDMAC_Channel_DeInit                   (AXI_DMAC_Type       * base, uint32_t channel);


__STATIC_INLINE void     Cy_AXIDMAC_Channel_SetDescriptor            (AXI_DMAC_Type       * base, uint32_t channel, cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE void     Cy_AXIDMAC_Channel_Enable                   (AXI_DMAC_Type       * base, uint32_t channel);
__STATIC_INLINE void     Cy_AXIDMAC_Channel_Disable                  (AXI_DMAC_Type       * base, uint32_t channel);
__STATIC_INLINE void     Cy_AXIDMAC_Channel_SetPriority              (AXI_DMAC_Type       * base, uint32_t channel, uint32_t priority);
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetPriority              (AXI_DMAC_Type const * base, uint32_t channel);
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetStatus                (AXI_DMAC_Type const * base, uint32_t channel);
__STATIC_INLINE   void * Cy_AXIDMAC_Channel_GetCurrentSrcAddress     (AXI_DMAC_Type           const * base, uint32_t channel);
__STATIC_INLINE   void * Cy_AXIDMAC_Channel_GetCurrentDstAddress     (AXI_DMAC_Type           const * base, uint32_t channel);
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetCurrentXloopIndex     (AXI_DMAC_Type const * base, uint32_t channel);
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetCurrentYloopIndex     (AXI_DMAC_Type const * base, uint32_t channel);
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetCurrentMloopIndex     (AXI_DMAC_Type const * base, uint32_t channel);
__STATIC_INLINE cy_stc_axidmac_descriptor_t *
                         Cy_AXIDMAC_Channel_GetCurrentDescriptor     (AXI_DMAC_Type const * base, uint32_t channel);
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetInterruptStatus       (AXI_DMAC_Type const * base, uint32_t channel);
__STATIC_INLINE void     Cy_AXIDMAC_Channel_ClearInterrupt           (AXI_DMAC_Type       * base, uint32_t channel, uint32_t interrupt);
__STATIC_INLINE void     Cy_AXIDMAC_Channel_SetInterrupt             (AXI_DMAC_Type       * base, uint32_t channel, uint32_t interrupt);
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetInterruptMask         (AXI_DMAC_Type const * base, uint32_t channel);
__STATIC_INLINE void     Cy_AXIDMAC_Channel_SetInterruptMask         (AXI_DMAC_Type       * base, uint32_t channel, uint32_t interrupt);
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetInterruptStatusMasked (AXI_DMAC_Type const * base, uint32_t channel);
__STATIC_INLINE void     Cy_AXIDMAC_Channel_SetSwTrigger             (AXI_DMAC_Type   * base, uint32_t channel);


/** \} group_axidmac_channel_functions */


/**
* \addtogroup group_axidmac_descriptor_functions
* \{
*/

 /*******************************************************************************
 * Function Name: Cy_AXIDMAC_Descriptor_Init
 ****************************************************************************//**
 *
 * Initializes the descriptor structure in SRAM from a pre-initialized
 * configuration structure.
 * This function initializes only the descriptor and not the channel.
 *
 * \param descriptor
 * The descriptor structure instance.
 *
 * \param config
 * This is a configuration structure that has all initialization information for
 * the descriptor.
 *
 * \return
 * The status /ref cy_en_axidmac_status_t.
 *
 * \funcusage
 * \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Enable
 *
 *******************************************************************************/
 cy_en_axidmac_status_t Cy_AXIDMAC_Descriptor_Init  (cy_stc_axidmac_descriptor_t * descriptor, cy_stc_axidmac_descriptor_config_t const * config);


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_DeInit
****************************************************************************//**
*
* Clears the content of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_Deinit
*
*******************************************************************************/
void Cy_AXIDMAC_Descriptor_DeInit(cy_stc_axidmac_descriptor_t * descriptor);


/*******************************************************************************
* Function Name: Cy_DMAC_Descriptor_SetNextDescriptor
****************************************************************************//**
*
* Sets a Next Descriptor parameter for the specified descriptor.
*
* Based on the descriptor type, the offset of the address for the next descriptor
* register may vary:
* For the 1D memory copy descriptor type, this register is at offset 0x10.
* For the 2D memory copy descriptor type, this register is at offset 0x18.
* For the 3D memory copy descriptor type, this register is at offset 0x20.
*
* \param descriptor
* The descriptor structure instance.
*
* \param nextDescriptor
* The pointer to the next descriptor.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
void Cy_AXIDMAC_Descriptor_SetNextDescriptor   (cy_stc_axidmac_descriptor_t * descriptor, cy_stc_axidmac_descriptor_t const * nextDescriptor);


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetDescriptorType
****************************************************************************//**
*
* Sets the descriptor's type for the specified descriptor.
* Moves the next descriptor pointer and M data count values into the proper
* offset in accordance to the actual descriptor type.
*
* During the descriptor's type change, carefully set up the settings starting
* from dstAddress and all the rest below in the \ref cy_stc_axidmac_descriptor_config_t
* structure. This is because the content of the descriptor registers might be
* lost/overridden by other descriptor settings due to the
* different registers structure for different descriptor types.
*
* \param descriptor
* The descriptor structure instance.
*
* \param descriptorType
* The descriptor type \ref cy_en_axidmac_descriptor_type_t.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
void Cy_AXIDMAC_Descriptor_SetDescriptorType   (cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_descriptor_type_t descriptorType);


__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetSrcAddress       (cy_stc_axidmac_descriptor_t * descriptor, void const * srcAddress);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetDstAddress       (cy_stc_axidmac_descriptor_t * descriptor, void const * dstAddress);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetMloopDataCount(cy_stc_axidmac_descriptor_t * descriptor, uint32_t mCount);


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetXloopDataCount
****************************************************************************//**
*
* Sets the number of data elements to transfer in the X loop
* for the specified descriptor (for 2D and 3D memory copy descriptors).
*
*
* \param descriptor
* The descriptor structure instance.
*
* \param xCount
* The number of data elements to transfer in the X loop.
* For all the rest of the descriptors (except 1D memory copy), the valid range is 1...65536.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
void Cy_AXIDMAC_Descriptor_SetXloopDataCount   (cy_stc_axidmac_descriptor_t * descriptor, uint32_t xCount);


__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetYloopDataCount(cy_stc_axidmac_descriptor_t * descriptor, uint32_t yCount);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetXloopSrcIncrement(cy_stc_axidmac_descriptor_t * descriptor, int16_t srcXincrement);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetXloopDstIncrement(cy_stc_axidmac_descriptor_t * descriptor, int16_t dstXincrement);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetYloopSrcIncrement(cy_stc_axidmac_descriptor_t * descriptor, int16_t srcYincrement);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetYloopDstIncrement(cy_stc_axidmac_descriptor_t * descriptor, int16_t dstYincrement);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetInterruptType    (cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_trigger_type_t interruptType);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetTriggerInType    (cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_trigger_type_t triggerInType);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetTriggerOutType(cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_trigger_type_t triggerOutType);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetRetrigger        (cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_retrigger_t retrigger);
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetChannelState     (cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_channel_state_t channelState);


/*******************************************************************************
* Function Name: Cy_DMAC_Descriptor_GetNextDescriptor
****************************************************************************//**
*
* Returns a next descriptor address of the specified descriptor.
*
* Based on the descriptor type, the offset of the address for the next descriptor
* register may vary:
* For the 1D memory copy descriptor type, this register is at offset 0x10.
* For the 2D memory copy descriptor type, this register is at offset 0x18.
* For the 3D memory copy descriptor type, this register is at offset 0x20.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The pointer to the next descriptor.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
 cy_stc_axidmac_descriptor_t *    Cy_AXIDMAC_Descriptor_GetNextDescriptor   (cy_stc_axidmac_descriptor_t const * descriptor);


__STATIC_INLINE cy_en_axidmac_descriptor_type_t Cy_AXIDMAC_Descriptor_GetDescriptorType(cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE void *                          Cy_AXIDMAC_Descriptor_GetSrcAddress       (cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE void *                          Cy_AXIDMAC_Descriptor_GetDstAddress       (cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE uint32_t                        Cy_AXIDMAC_Descriptor_GetMloopDataCount(cy_stc_axidmac_descriptor_t const * descriptor);


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetXloopDataCount
****************************************************************************//**
*
* Returns the number of data elements for the X loop of the specified
* descriptor (for 2D and 3D memory copy descriptors).
*
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The number of data elements to transfer in the X loop.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
uint32_t                        Cy_AXIDMAC_Descriptor_GetXloopDataCount   (cy_stc_axidmac_descriptor_t const * descriptor);


__STATIC_INLINE uint32_t                        Cy_AXIDMAC_Descriptor_GetYloopDataCount   (cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE int16_t                        Cy_AXIDMAC_Descriptor_GetXloopSrcIncrement(cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE int16_t                        Cy_AXIDMAC_Descriptor_GetXloopDstIncrement(cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE int16_t                        Cy_AXIDMAC_Descriptor_GetYloopSrcIncrement(cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE int16_t                        Cy_AXIDMAC_Descriptor_GetYloopDstIncrement(cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_axidmac_trigger_type_t    Cy_AXIDMAC_Descriptor_GetInterruptType    (cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_axidmac_trigger_type_t    Cy_AXIDMAC_Descriptor_GetTriggerInType    (cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_axidmac_trigger_type_t    Cy_AXIDMAC_Descriptor_GetTriggerOutType   (cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_axidmac_retrigger_t       Cy_AXIDMAC_Descriptor_GetRetrigger        (cy_stc_axidmac_descriptor_t const * descriptor);
__STATIC_INLINE cy_en_axidmac_channel_state_t   Cy_AXIDMAC_Descriptor_GetChannelState     (cy_stc_axidmac_descriptor_t const * descriptor);

/** \} group_axidmac_descriptor_functions */


/***************************************
*    In-line Function Implementation
***************************************/


/**
* \addtogroup group_axidmac_block_functions
* \{
*/


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Enable
****************************************************************************//**
*
* Enables the AXIDMAC block.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Enable
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Enable(AXI_DMAC_Type * base)
{
    AXIDMAC_CTL(base) |= AXI_DMAC_CTL_ENABLED_Msk;
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Disable
****************************************************************************//**
*
* Disables the AXIDMAC block.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Disable
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Disable(AXI_DMAC_Type * base)
{
    AXIDMAC_CTL(base) &= (uint32_t) ~AXI_DMAC_CTL_ENABLED_Msk;
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_GetAllChannelStatus
****************************************************************************//**
*
* Returns the channel enabled status of all channels in a AXIDMAC block. Example if return value is 7 then channel 0,1,2 are enabled.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \return
* Returns a bit-field with all of the currently enabled channels in the
* AXIDMAC block.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_GetAllChannelStatus
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_GetAllChannelStatus(AXI_DMAC_Type * base)
{
    return(_FLD2VAL(AXI_DMAC_STATUS_CH_EN, AXIDMAC_STATUS(base)));
}

#if defined (CY_IP_MXSAXIDMAC)
/*******************************************************************************
* Function Name: Cy_AXIDMAC_GetActiveChannel
****************************************************************************//**
*
* Returns the status of the active/pending channels of the AXIDMAC block.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \return
* Returns a bit-field with all of the currently active/pending secure channels in the
* AXIDMAC block.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_GetActiveChannel
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_GetActiveChannel(AXI_DMAC_Type const * base)
{
    return(_FLD2VAL(AXI_DMAC_ACTIVE_ACTIVE, AXIDMAC_ACTIVE(base)));
}

#elif defined (CY_IP_MXAXIDMAC)
/*******************************************************************************
* Function Name: Cy_AXIDMAC_GetActiveSecureChannel
****************************************************************************//**
*
* Returns the status of the active/pending secure channels of the AXIDMAC block.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \return
* Returns a bit-field with all of the currently active/pending secure channels
* in the AXIDMAC block.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_GetActiveChannel
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_GetActiveSecureChannel(AXI_DMAC_Type const * base)
{
    return(_FLD2VAL(AXI_DMAC_ACTIVE_SEC_ACTIVE, AXIDMAC_ACTIVE_SEC(base)));
}

/*******************************************************************************
* Function Name: Cy_AXIDMAC_GetActiveNonSecureChannel
****************************************************************************//**
*
* Returns the status of the active/pending non-secure channels of the AXIDMAC block.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \return
* Returns a bit-field with all of the currently active/pending non-secure channels
* in the AXIDMAC block.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_GetActiveChannel
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_GetActiveNonSecureChannel(AXI_DMAC_Type const * base)
{
    return(_FLD2VAL(AXI_DMAC_ACTIVE_NONSEC_ACTIVE, AXIDMAC_ACTIVE_NONSEC(base)));
}
#endif


/** \} group_axidmac_block_functions */


/**
* \addtogroup group_axidmac_descriptor_functions
* \{
*/


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetSrcAddress
****************************************************************************//**
*
* Sets the source address parameter for the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \param srcAddress
* The source address value for the descriptor.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetSrcAddress(cy_stc_axidmac_descriptor_t * descriptor, void const * srcAddress)
{
    descriptor->src = (uint32_t) srcAddress;
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetSrcAddress
****************************************************************************//**
*
* Returns the source address of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The source address value of the descriptor.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void * Cy_AXIDMAC_Descriptor_GetSrcAddress(cy_stc_axidmac_descriptor_t const * descriptor)
{
    return ((void *) descriptor->src);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetDstAddress
****************************************************************************//**
*
* Sets the destination address parameter for the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \param dstAddress
* The destination address value for the descriptor.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetDstAddress(cy_stc_axidmac_descriptor_t * descriptor, void const * dstAddress)
{
    descriptor->dst = (uint32_t) dstAddress;
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetDstAddress
****************************************************************************//**
*
* Returns the destination address of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The destination address value of the descriptor.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void * Cy_AXIDMAC_Descriptor_GetDstAddress(cy_stc_axidmac_descriptor_t const * descriptor)
{
    return ((void *) descriptor->dst);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetInterruptType
****************************************************************************//**
*
* Sets the interrupt type parameter for the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \param interruptType
* The interrupt type set for the descriptor. \ref cy_en_axidmac_trigger_type_t
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetInterruptType(cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_trigger_type_t interruptType)
{
    CY_ASSERT_L3(CY_AXIDMAC_IS_TRIG_TYPE_VALID(interruptType));

    CY_REG32_CLR_SET(descriptor->ctl, AXI_DMAC_CH_DESCR_CTL_INTR_TYPE, interruptType);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetInterruptType
****************************************************************************//**
*
* Returns the Interrupt-Type of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The Interrupt-Type \ref cy_en_axidmac_trigger_type_t.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE cy_en_axidmac_trigger_type_t Cy_AXIDMAC_Descriptor_GetInterruptType(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8','Intentional typecast to cy_en_axidmac_trigger_type_t enum.');
    return((cy_en_axidmac_trigger_type_t) _FLD2VAL(AXI_DMAC_CH_DESCR_CTL_INTR_TYPE, descriptor->ctl));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetTriggerInType
****************************************************************************//**
*
* Sets the Trigger In Type parameter for the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \param triggerInType
* The Trigger In Type parameter \ref cy_en_axidmac_trigger_type_t
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetTriggerInType(cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_trigger_type_t triggerInType)
{
    CY_ASSERT_L3(CY_AXIDMAC_IS_TRIG_TYPE_VALID(triggerInType));

    CY_REG32_CLR_SET(descriptor->ctl, AXI_DMAC_CH_DESCR_CTL_TR_IN_TYPE, triggerInType);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetTriggerInType
****************************************************************************//**
*
* Returns the Trigger In Type of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The Trigger In Type \ref cy_en_axidmac_trigger_type_t
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE cy_en_axidmac_trigger_type_t Cy_AXIDMAC_Descriptor_GetTriggerInType(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8','Intentional typecast to cy_en_axidmac_trigger_type_t enum.');
    return((cy_en_axidmac_trigger_type_t) _FLD2VAL(AXI_DMAC_CH_DESCR_CTL_TR_IN_TYPE, descriptor->ctl));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetTriggerOutType
****************************************************************************//**
*
* Sets the Trigger Out Type parameter for the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \param triggerOutType
* The Trigger Out Type set for the descriptor. \ref cy_en_axidmac_trigger_type_t
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetTriggerOutType(cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_trigger_type_t triggerOutType)
{
    CY_ASSERT_L3(CY_AXIDMAC_IS_TRIG_TYPE_VALID(triggerOutType));

    CY_REG32_CLR_SET(descriptor->ctl, AXI_DMAC_CH_DESCR_CTL_TR_OUT_TYPE, triggerOutType);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetTriggerOutType
****************************************************************************//**
*
* Returns the Trigger Out Type of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The Trigger Out Type parameter \ref cy_en_axidmac_trigger_type_t.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE cy_en_axidmac_trigger_type_t Cy_AXIDMAC_Descriptor_GetTriggerOutType(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8','Intentional typecast to cy_en_axidmac_trigger_type_t enum.');
    return((cy_en_axidmac_trigger_type_t) _FLD2VAL(AXI_DMAC_CH_DESCR_CTL_TR_OUT_TYPE, descriptor->ctl));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetRetrigger
****************************************************************************//**
*
* Sets the retrigger value which specifies whether the controller should
* wait for the input trigger to be deactivated.
*
* \param descriptor
* The descriptor structure instance.
*
* \param retrigger
* The \ref cy_en_axidmac_retrigger_t parameter specifies whether the controller
* should wait for the input trigger to be deactivated.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetRetrigger(cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_retrigger_t retrigger)
{
    CY_ASSERT_L3(CY_AXIDMAC_IS_RETRIGGER_VALID(retrigger));

    CY_REG32_CLR_SET(descriptor->ctl, AXI_DMAC_CH_DESCR_CTL_WAIT_FOR_DEACT, retrigger);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetRetrigger
****************************************************************************//**
*
* Returns a value which specifies whether the controller should
* wait for the input trigger to be deactivated.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The Retrigger setting \ref cy_en_axidmac_retrigger_t.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE cy_en_axidmac_retrigger_t Cy_AXIDMAC_Descriptor_GetRetrigger(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8','Intentional typecast to cy_en_axidmac_retrigger_t enum.');
    return((cy_en_axidmac_retrigger_t) _FLD2VAL(AXI_DMAC_CH_DESCR_CTL_WAIT_FOR_DEACT, descriptor->ctl));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetDescriptorType
****************************************************************************//**
*
* Returns the descriptor's type of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The descriptor type \ref cy_en_axidmac_descriptor_type_t
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE cy_en_axidmac_descriptor_type_t Cy_AXIDMAC_Descriptor_GetDescriptorType(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8','Intentional typecast to cy_en_axidmac_descriptor_type_t enum.');
    return((cy_en_axidmac_descriptor_type_t) _FLD2VAL(AXI_DMAC_CH_DESCR_CTL_DESCR_TYPE, descriptor->ctl));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetChannelState
****************************************************************************//**
*
* Sets the channel state on completion of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \param channelState
* The channel state \ref cy_en_axidmac_channel_state_t.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetChannelState(cy_stc_axidmac_descriptor_t * descriptor, cy_en_axidmac_channel_state_t channelState)
{
    CY_ASSERT_L3(CY_AXIDMAC_IS_CHANNEL_STATE_VALID(channelState));

    CY_REG32_CLR_SET(descriptor->ctl, AXI_DMAC_CH_DESCR_CTL_CH_DISABLE, channelState);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetChannelState
****************************************************************************//**
*
* Returns the channel state on completion of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The Channel State setting \ref cy_en_axidmac_channel_state_t
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE cy_en_axidmac_channel_state_t Cy_AXIDMAC_Descriptor_GetChannelState(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.8','Intentional typecast to cy_en_axidmac_channel_state_t enum.');
    return((cy_en_axidmac_channel_state_t) _FLD2VAL(AXI_DMAC_CH_DESCR_CTL_CH_DISABLE, descriptor->ctl));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetMloopDataCount
****************************************************************************//**
*
* Sets the number of data elements for the M loop of the specified descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \param mCount
* The number of M loops to execute. The valid range is 1 ... 65536.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetMloopDataCount(cy_stc_axidmac_descriptor_t * descriptor, uint32_t mCount)
{   
    CY_ASSERT_L2(CY_AXIDMAC_IS_LOOP_COUNT_VALID(mCount));
    /* Convert the data count from the user's range (1-65536) into the machine range (0-65535). */
    CY_REG32_CLR_SET(descriptor->mSize, AXI_DMAC_CH_DESCR_M_SIZE_M_COUNT, mCount - 1UL);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetMloopDataCount
****************************************************************************//**
*
* Returns the number of M loops to execute in the specified
* descriptor.
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The number of M loops to execute The range is 1 ... 65536.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Descriptor_GetMloopDataCount(cy_stc_axidmac_descriptor_t const * descriptor)
{
    /* Convert the data count from the machine range (0-65535) into the user's range (1-65536). */
    return (_FLD2VAL(AXI_DMAC_CH_DESCR_M_SIZE_M_COUNT, descriptor->mSize) + 1UL);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetXloopSrcIncrement
****************************************************************************//**
*
* Sets the source increment parameter for the X loop of the specified
* descriptor (for 2D or 3D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \param srcXincrement
* The value of the source increment. The valid range is -32768 ... 32767.
* Top most bit is a sign bit i.e., 0x8000 indicates -32768.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetXloopSrcIncrement(cy_stc_axidmac_descriptor_t * descriptor, int16_t srcXincrement)
{
    CY_ASSERT_L1(CY_AXIDMAC_1D_MEMORY_COPY != Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));

    CY_REG32_CLR_SET(descriptor->xIncr, AXI_DMAC_CH_DESCR_X_INCR_SRC_X, srcXincrement);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetXloopSrcIncrement
****************************************************************************//**
*
* Returns the source increment parameter for the X loop of the specified
* descriptor (for 2D or 3D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The value of the source increment. The range is -32768 ... 32767.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE int16_t Cy_AXIDMAC_Descriptor_GetXloopSrcIncrement(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_ASSERT_L1(CY_AXIDMAC_1D_MEMORY_COPY != Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));

    return ((int16_t) _FLD2VAL(AXI_DMAC_CH_DESCR_X_INCR_SRC_X, descriptor->xIncr));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetXloopDstIncrement
****************************************************************************//**
*
* Sets the destination increment parameter for the X loop for the specified
* descriptor (for 2D or 3D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \param dstXincrement
* The value of the destination increment. The valid range is -32768 ... 32767.
* Top most bit is a sign bit i.e., 0x8000 indicates -32768.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetXloopDstIncrement(cy_stc_axidmac_descriptor_t * descriptor, int16_t dstXincrement)
{
    CY_ASSERT_L1(CY_AXIDMAC_1D_MEMORY_COPY != Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));

    CY_REG32_CLR_SET(descriptor->xIncr, AXI_DMAC_CH_DESCR_X_INCR_DST_X, dstXincrement);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetXloopDstIncrement
****************************************************************************//**
*
* Returns the destination increment parameter for the X loop of the specified
* descriptor (for 2D or 3D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The value of the destination increment. The range is -32768 ... 32767.
* Top most bit is a sign bit i.e., 0x8000 indicates -32768.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE int16_t Cy_AXIDMAC_Descriptor_GetXloopDstIncrement(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_ASSERT_L1(CY_AXIDMAC_1D_MEMORY_COPY != Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));

    return ((int16_t) _FLD2VAL(AXI_DMAC_CH_DESCR_X_INCR_DST_X, descriptor->xIncr));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetYloopDataCount
****************************************************************************//**
*
* Sets the number of data elements for the Y loop of the specified descriptor
* (for 3D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \param yCount
* The number of X loops to execute in the Y loop. The valid range is 1 ... 65536.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetYloopDataCount(cy_stc_axidmac_descriptor_t * descriptor, uint32_t yCount)
{
    CY_ASSERT_L1(CY_AXIDMAC_3D_MEMORY_COPY == Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));
    CY_ASSERT_L2(CY_AXIDMAC_IS_LOOP_COUNT_VALID(yCount));
    /* Convert the data count from the user's range (1-65536) into the machine range (0-65535). */
    CY_REG32_CLR_SET(descriptor->ySize, AXI_DMAC_CH_DESCR_Y_SIZE_Y_COUNT, yCount - 1UL);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetYloopDataCount
****************************************************************************//**
*
* Returns the number of X loops to execute in the Y loop of the specified
* descriptor (for 3D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The number of X loops to execute in the Y loop. The range is 1 ... 65536.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Descriptor_GetYloopDataCount(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_ASSERT_L1(CY_AXIDMAC_3D_MEMORY_COPY == Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));
    /* Convert the data count from the machine range (0-65535) into the user's range (1-65536). */
    return (_FLD2VAL(AXI_DMAC_CH_DESCR_Y_SIZE_Y_COUNT, descriptor->ySize) + 1UL);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetYloopSrcIncrement
****************************************************************************//**
*
* Sets the source increment parameter for the Y loop for the specified
* descriptor (for 3D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \param srcYincrement
* The value of the source increment. The valid range is -32768 ... 32767.
* Top most bit is a sign bit i.e., 0x8000 indicates -32768.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetYloopSrcIncrement(cy_stc_axidmac_descriptor_t * descriptor, int16_t srcYincrement)
{
    CY_ASSERT_L1(CY_AXIDMAC_3D_MEMORY_COPY == Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));

    CY_REG32_CLR_SET(descriptor->yIncr, AXI_DMAC_CH_DESCR_Y_INCR_SRC_Y, srcYincrement);
}


/*******************************************************************************
* Function Name: Cy_DMAC_Descriptor_GetYloopSrcIncrement
****************************************************************************//**
*
* Returns the source increment parameter for the Y loop of the specified
* descriptor (for 2D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The value of source increment. The range is -32768 ... 32767.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE int16_t Cy_AXIDMAC_Descriptor_GetYloopSrcIncrement(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_ASSERT_L1(CY_AXIDMAC_3D_MEMORY_COPY == Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));

    return ((int16_t) _FLD2VAL(AXI_DMAC_CH_DESCR_Y_INCR_SRC_Y, descriptor->yIncr));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_SetYloopDstIncrement
****************************************************************************//**
*
* Sets the destination increment parameter for the Y loop of the specified
* descriptor (for 3D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \param dstYincrement
* The value of the destination increment. The valid range is -32768 ... 32767.
* Top most bit is a sign bit i.e., 0x8000 indicates -32768.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_SetterFunctions
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Descriptor_SetYloopDstIncrement(cy_stc_axidmac_descriptor_t * descriptor, int16_t dstYincrement)
{
    CY_ASSERT_L1(CY_AXIDMAC_3D_MEMORY_COPY == Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));

    CY_REG32_CLR_SET(descriptor->yIncr, AXI_DMAC_CH_DESCR_Y_INCR_DST_Y, dstYincrement);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Descriptor_GetYloopDstIncrement
****************************************************************************//**
*
* Returns the destination increment parameter for the Y loop of the specified
* descriptor (for 3D descriptors only).
*
* \param descriptor
* The descriptor structure instance.
*
* \return
* The value of the destination increment. The range is -32768 ... 32767.
* Top most bit is a sign bit i.e., 0x8000 indicates -32768.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_GetterFunctions
*
*******************************************************************************/
__STATIC_INLINE int16_t Cy_AXIDMAC_Descriptor_GetYloopDstIncrement(cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_ASSERT_L1(CY_AXIDMAC_3D_MEMORY_COPY == Cy_AXIDMAC_Descriptor_GetDescriptorType(descriptor));

    return ((int16_t) _FLD2VAL(AXI_DMAC_CH_DESCR_Y_INCR_DST_Y, descriptor->yIncr));
}

/** \} group_axidmac_descriptor_functions */


/**
* \addtogroup group_axidmac_channel_functions
* \{
*/


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_SetDescriptor
****************************************************************************//**
*
* Sets a descriptor as current for the specified AXIDMAC channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \param descriptor
* This is the descriptor to be associated with the channel.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Enable
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Channel_SetDescriptor(AXI_DMAC_Type * base, uint32_t channel, cy_stc_axidmac_descriptor_t const * descriptor)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    AXIDMAC_CH_CURR(base, channel) = (uint32_t)descriptor;
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_Enable
****************************************************************************//**
*
* Enables a AXIDMAC channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The  channel number.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Enable
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Channel_Enable(AXI_DMAC_Type * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    AXIDMAC_CH_CTL(base, channel) |= AXI_DMAC_CH_CTL_ENABLED_Msk;
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_Disable
****************************************************************************//**
*
* Disables a AXIDMAC channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Disable
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Channel_Disable(AXI_DMAC_Type * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    AXIDMAC_CH_CTL(base, channel) &= (uint32_t) ~AXI_DMAC_CH_CTL_ENABLED_Msk;
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_SetPriority
****************************************************************************//**
*
* The function is used to set a priority for the AXIDMAC channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \param priority
* The priority to be set for the AXIDMAC channel. The allowed values are 0,1,2,3.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Enable
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Channel_SetPriority(AXI_DMAC_Type * base, uint32_t channel, uint32_t priority)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));
    CY_ASSERT_L2(CY_AXIDMAC_IS_PRIORITY_VALID(priority));

    CY_REG32_CLR_SET(AXIDMAC_CH_CTL(base, channel), AXI_DMAC_CH_CTL_PRIO, priority);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetPriority
****************************************************************************//**
*
* Returns the priority of the AXIDMAC channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* The priority of the channel.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Disable
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetPriority(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return ((uint32_t) _FLD2VAL(AXI_DMAC_CH_CTL_PRIO, AXIDMAC_CH_CTL(base, channel)));
}

/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetStatus
****************************************************************************//**
*
* Returns the enable status of the AXIDMAC channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* The status of the channel.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Disable
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetStatus(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return ((uint32_t) _FLD2VAL(AXI_DMAC_CH_STATUS_ENABLED, AXIDMAC_CH_CTL(base, channel)));
}

/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetCurrentSrcAddress
****************************************************************************//**
*
* Returns the source address being used for the current transfer.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* Returns the pointer to the source of transfer.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Channel_GetCurrentSrcAddress
*
*******************************************************************************/
__STATIC_INLINE void * Cy_AXIDMAC_Channel_GetCurrentSrcAddress(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return ((void *)(AXIDMAC_CH_DESCR_SRC(base, channel)));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetCurrentDstAddress
****************************************************************************//**
*
* Returns the destination address being used for the current transfer.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* Returns the pointer to the destination of transfer.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Channel_GetCurrentSrcAddress
*
*******************************************************************************/
__STATIC_INLINE void * Cy_AXIDMAC_Channel_GetCurrentDstAddress(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return ((void *)(AXIDMAC_CH_DESCR_DST(base, channel)));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetCurrentXloopIndex
****************************************************************************//**
*
* Returns the current transfer X-loop index.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* Returns the current transfer X-loop index.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Channel_GetCurrentloopIndex
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetCurrentXloopIndex(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return (_FLD2VAL(AXI_DMAC_CH_IDX_X, AXIDMAC_CH_IDX(base, channel)));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetCurrentYloopIndex
****************************************************************************//**
*
* Returns the current transfer Y-loop index.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* Returns the current transfer Y-loop index.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Channel_GetCurrentloopIndex
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetCurrentYloopIndex(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return (_FLD2VAL(AXI_DMAC_CH_IDX_Y, AXIDMAC_CH_IDX(base, channel)));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetCurrentMloopIndex
****************************************************************************//**
*
* Returns the current transfer M-loop index.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* Returns the current transfer M-loop index.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Channel_GetCurrentloopIndex
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetCurrentMloopIndex(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return (_FLD2VAL(AXI_DMAC_CH_M_IDX_M, AXIDMAC_CH_IDX(base, channel)));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_SetSwTrigger
****************************************************************************//**
*
* The function is used to set a SW trigger for a channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \note Channel must have been enabled before setting software trigger.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Enable
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Channel_SetSwTrigger(AXI_DMAC_Type * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    CY_REG32_CLR_SET(AXIDMAC_CH_TR_CMD(base, channel), AXI_DMAC_CH_TR_CMD_ACTIVATE, 1);
}

/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetCurrentDescriptor
****************************************************************************//**
*
* Returns the descriptor that is active in the channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* The pointer to the descriptor associated with the channel.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_Descriptor_Deinit
*
*******************************************************************************/
__STATIC_INLINE cy_stc_axidmac_descriptor_t * Cy_AXIDMAC_Channel_GetCurrentDescriptor(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return ((cy_stc_axidmac_descriptor_t*)(AXIDMAC_CH_CURR(base, channel)));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetInterruptStatus
****************************************************************************//**
*
* Returns the interrupt status of the specified channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* The interrupt status, see \ref group_axidmac_macros_interrupt_masks.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_GetInterruptStatus
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetInterruptStatus(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return (AXIDMAC_CH_INTR(base, channel));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_ClearInterrupt
****************************************************************************//**
*
* Clears the interrupt status of the specified channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \param interrupt
* The interrupt mask, see \ref group_axidmac_macros_interrupt_masks.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_ClearInterrupt
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Channel_ClearInterrupt(AXI_DMAC_Type * base, uint32_t channel, uint32_t interrupt)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));
    CY_ASSERT_L2(CY_AXIDMAC_IS_INTR_MASK_VALID(interrupt));

    AXIDMAC_CH_INTR(base, channel) = interrupt;
    /* This dummy reading is necessary here. It provides a guarantee that interrupt is cleared at returning from this function. */
    (void) AXIDMAC_CH_INTR(base, channel);
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_SetInterrupt
****************************************************************************//**
*
* Sets the interrupt for the specified channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \param interrupt
* The interrupt mask. See \ref group_axidmac_macros_interrupt_masks.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_SetInterruptMask
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Channel_SetInterrupt(AXI_DMAC_Type * base, uint32_t channel, uint32_t interrupt)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));
    CY_ASSERT_L2(CY_AXIDMAC_IS_INTR_MASK_VALID(interrupt));

    AXIDMAC_CH_INTR_SET(base, channel) = interrupt;
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetInterruptMask
****************************************************************************//**
*
* Returns the interrupt mask value of the specified channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* The interrupt mask value. See \ref group_dmac_macros_interrupt_masks.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_SetInterruptMask
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetInterruptMask(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return (AXIDMAC_CH_INTR_MASK(base, channel));
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_SetInterruptMask
****************************************************************************//**
*
* Sets an interrupt mask value for the specified channel.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \param interrupt
* The interrupt mask, see \ref group_axidmac_macros_interrupt_masks.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_SetInterruptMask
*
*******************************************************************************/
__STATIC_INLINE void Cy_AXIDMAC_Channel_SetInterruptMask(AXI_DMAC_Type * base, uint32_t channel, uint32_t interrupt)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));
    CY_ASSERT_L2(CY_AXIDMAC_IS_INTR_MASK_VALID(interrupt));
    AXIDMAC_CH_INTR_MASK(base, channel) = interrupt;
}


/*******************************************************************************
* Function Name: Cy_AXIDMAC_Channel_GetInterruptStatusMasked
****************************************************************************//**
*
* Returns the logical AND of the corresponding INTR and INTR_MASK fields
* in a single-load operation.
*
* \param base
* The pointer to the hardware AXIDMAC block.
*
* \param channel
* The channel number.
*
* \return
* The masked interrupt status. See \ref group_axidmac_macros_interrupt_masks.
*
* \funcusage
* \snippet axidmac/snippet/main.c snippet_Cy_AXIDMAC_ClearInterrupt
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_AXIDMAC_Channel_GetInterruptStatusMasked(AXI_DMAC_Type const * base, uint32_t channel)
{
    CY_ASSERT_L1(CY_AXIDMAC_IS_CH_NR_VALID(channel));

    return (AXIDMAC_CH_INTR_MASKED(base, channel));
}

/** \} group_axidmac_channel_functions */

/** \} group_axidmac_functions */
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 10.8')
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 10.4')
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 10.3')
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 10.1')
#if defined(__cplusplus)
}
#endif

#endif /* CY_IP_MXSAXIDMAC || CY_IP_MXAXIDMAC */

#endif  /* !defined (CY_AXI_DMAC_H) */

/** \} group_axidmac */


/* [] END OF FILE */
