/*******************************************************************************
* File Name: CyDmac.h
* Version 4.20
*
*  Description:
*   Provides the function definitions for the DMA Controller.
*
*  Note:
*   Documentation of the API's in this file is located in the
*   System Reference Guide provided with PSoC Creator.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOT_CYDMAC_H)
#define CY_BOOT_CYDMAC_H


#include "cytypes.h"
#include "cyfitter.h"
#include "cydevice_trm.h"
#include "CyLib.h"


/***************************************
*    Function Prototypes
***************************************/

/* DMA Controller functions. */
void    CyDmacConfigure(void) ;
uint8   CyDmacError(void) ;
void    CyDmacClearError(uint8 error) ;
uint32  CyDmacErrorAddress(void) ;

/* Channel specific functions. */
uint8    CyDmaChAlloc(void) ;
cystatus CyDmaChFree(uint8 chHandle) ;
cystatus CyDmaChEnable(uint8 chHandle, uint8 preserveTds) ;
cystatus CyDmaChDisable(uint8 chHandle) ;
cystatus CyDmaClearPendingDrq(uint8 chHandle) ;
cystatus CyDmaChPriority(uint8 chHandle, uint8 priority) ;
cystatus CyDmaChSetExtendedAddress(uint8 chHandle, uint16 source, uint16 destination)\
;
cystatus CyDmaChSetInitialTd(uint8 chHandle, uint8 startTd) ;
cystatus CyDmaChSetRequest(uint8 chHandle, uint8 request) ;
cystatus CyDmaChGetRequest(uint8 chHandle) ;
cystatus CyDmaChStatus(uint8 chHandle, uint8 * currentTd, uint8 * state) ;
cystatus CyDmaChSetConfiguration(uint8 chHandle, uint8 burstCount, uint8 requestPerBurst, uint8 tdDone0,
                                 uint8 tdDone1, uint8 tdStop) ;
cystatus CyDmaChRoundRobin(uint8 chHandle, uint8 enableRR) ;

/* Transfer Descriptor functions. */
uint8    CyDmaTdAllocate(void) ;
void     CyDmaTdFree(uint8 tdHandle) ;
uint8    CyDmaTdFreeCount(void) ;
cystatus CyDmaTdSetConfiguration(uint8 tdHandle, uint16 transferCount, uint8 nextTd, uint8 configuration)\
;
cystatus CyDmaTdGetConfiguration(uint8 tdHandle, uint16 * transferCount, uint8 * nextTd, uint8 * configuration)\
;
cystatus CyDmaTdSetAddress(uint8 tdHandle, uint16 source, uint16 destination) ;
cystatus CyDmaTdGetAddress(uint8 tdHandle, uint16 * source, uint16 * destination) ;


/***************************************
* Data Struct Definitions
***************************************/

typedef struct dmac_ch_struct
{
    volatile uint8 basic_cfg[4];
    volatile uint8 action[4];
    volatile uint8 basic_status[4];
    volatile uint8 reserved[4];

} dmac_ch;


typedef struct dmac_cfgmem_struct
{
    volatile uint8 CFG0[4];
    volatile uint8 CFG1[4];

} dmac_cfgmem;


typedef struct dmac_tdmem_struct
{
    volatile uint8  TD0[4];
    volatile uint8  TD1[4];

} dmac_tdmem;


typedef struct dmac_tdmem2_struct
{
    volatile uint16 xfercnt;
    volatile uint8  next_td_ptr;
    volatile uint8  flags;
    volatile uint16 src_adr;
    volatile uint16 dst_adr;
} dmac_tdmem2;


/***************************************
* API Constants
***************************************/

#define CY_DMA_INVALID_CHANNEL      0xFFu   /* Invalid Channel ID */
#define CY_DMA_INVALID_TD           0xFFu   /* Invalid TD */
#define CY_DMA_END_CHAIN_TD         0xFFu   /* End of chain TD */
#define CY_DMA_DISABLE_TD           0xFEu

#define CY_DMA_TD_SIZE              0x08u

/* "u" was removed as workaround for Keil compiler bug */
#define CY_DMA_TD_SWAP_EN           0x80
#define CY_DMA_TD_SWAP_SIZE4        0x40
#define CY_DMA_TD_AUTO_EXEC_NEXT    0x20
#define CY_DMA_TD_TERMIN_EN         0x10
#define CY_DMA_TD_TERMOUT1_EN       0x08
#define CY_DMA_TD_TERMOUT0_EN       0x04
#define CY_DMA_TD_INC_DST_ADR       0x02
#define CY_DMA_TD_INC_SRC_ADR       0x01

#define CY_DMA_NUMBEROF_TDS         128u
#define CY_DMA_NUMBEROF_CHANNELS    ((uint8)(CYDEV_DMA_CHANNELS_AVAILABLE))

/* Action register bits */
#define CY_DMA_CPU_REQ              ((uint8)(1u << 0u))
#define CY_DMA_CPU_TERM_TD          ((uint8)(1u << 1u))
#define CY_DMA_CPU_TERM_CHAIN       ((uint8)(1u << 2u))

/* Basic Status register bits */
#define CY_DMA_STATUS_CHAIN_ACTIVE  ((uint8)(1u << 0u))
#define CY_DMA_STATUS_TD_ACTIVE     ((uint8)(1u << 1u))

/* DMA controller register error bits */
#define CY_DMA_BUS_TIMEOUT          (1u << 1u)
#define CY_DMA_UNPOP_ACC            (1u << 2u)
#define CY_DMA_PERIPH_ERR           (1u << 3u)

/* Round robin bits */
#define CY_DMA_ROUND_ROBIN_ENABLE   ((uint8)(1u << 4u))


/*******************************************************************************
* CyDmaChEnable() / CyDmaChDisable() API constants
*******************************************************************************/
#define CY_DMA_CH_BASIC_CFG_EN           (0x01u)
#define CY_DMA_CH_BASIC_CFG_WORK_SEP     (0x20u)


/***************************************
* Registers
***************************************/

#define CY_DMA_CFG_REG              (*(reg32 *) CYREG_PHUB_CFG)
#define CY_DMA_CFG_PTR              ( (reg32 *) CYREG_PHUB_CFG)

#define CY_DMA_ERR_REG              (*(reg32 *) CYREG_PHUB_ERR)
#define CY_DMA_ERR_PTR              ( (reg32 *) CYREG_PHUB_ERR)

#define CY_DMA_ERR_ADR_REG          (*(reg32 *) CYREG_PHUB_ERR_ADR)
#define CY_DMA_ERR_ADR_PTR          ( (reg32 *) CYREG_PHUB_ERR_ADR)

#define CY_DMA_CH_STRUCT_REG        (*(dmac_ch CYXDATA *) CYDEV_PHUB_CH0_BASE)
#define CY_DMA_CH_STRUCT_PTR        ( (dmac_ch CYXDATA *) CYDEV_PHUB_CH0_BASE)

#define CY_DMA_CFGMEM_STRUCT_REG    (*(dmac_cfgmem CYXDATA *) CYDEV_PHUB_CFGMEM0_BASE)
#define CY_DMA_CFGMEM_STRUCT_PTR    ( (dmac_cfgmem CYXDATA *) CYDEV_PHUB_CFGMEM0_BASE)

#define CY_DMA_TDMEM_STRUCT_REG     (*(dmac_tdmem  CYXDATA *) CYDEV_PHUB_TDMEM0_BASE)
#define CY_DMA_TDMEM_STRUCT_PTR     ( (dmac_tdmem  CYXDATA *) CYDEV_PHUB_TDMEM0_BASE)


/*******************************************************************************
* The following code is OBSOLETE and must not be used.
*
* If the obsoleted macro definitions intended for use in the application use the
* following scheme, redefine your own versions of these definitions:
*    #ifdef <OBSOLETED_DEFINE>
*        #undef  <OBSOLETED_DEFINE>
*        #define <OBSOLETED_DEFINE>      (<New Value>)
*    #endif
*
* Note: Redefine obsoleted macro definitions with caution. They might still be
*       used in the application and their modification might lead to unexpected
*       consequences.
*******************************************************************************/
#define DMA_INVALID_CHANNEL         (CY_DMA_INVALID_CHANNEL)
#define DMA_INVALID_TD              (CY_DMA_INVALID_TD)
#define DMA_END_CHAIN_TD            (CY_DMA_END_CHAIN_TD)
#define DMAC_TD_SIZE                (CY_DMA_TD_SIZE)
#define TD_SWAP_EN                  (CY_DMA_TD_SWAP_EN)
#define TD_SWAP_SIZE4               (CY_DMA_TD_SWAP_SIZE4)
#define TD_AUTO_EXEC_NEXT           (CY_DMA_TD_AUTO_EXEC_NEXT)
#define TD_TERMIN_EN                (CY_DMA_TD_TERMIN_EN)
#define TD_TERMOUT1_EN              (CY_DMA_TD_TERMOUT1_EN)
#define TD_TERMOUT0_EN              (CY_DMA_TD_TERMOUT0_EN)
#define TD_INC_DST_ADR              (CY_DMA_TD_INC_DST_ADR)
#define TD_INC_SRC_ADR              (CY_DMA_TD_INC_SRC_ADR)
#define NUMBEROF_TDS                (CY_DMA_NUMBEROF_TDS)
#define NUMBEROF_CHANNELS           (CY_DMA_NUMBEROF_CHANNELS)
#define CPU_REQ                     (CY_DMA_CPU_REQ)
#define CPU_TERM_TD                 (CY_DMA_CPU_TERM_TD)
#define CPU_TERM_CHAIN              (CY_DMA_CPU_TERM_CHAIN)
#define STATUS_CHAIN_ACTIVE         (CY_DMA_STATUS_CHAIN_ACTIVE)
#define STATUS_TD_ACTIVE            (CY_DMA_STATUS_TD_ACTIVE)
#define DMAC_BUS_TIMEOUT            (CY_DMA_BUS_TIMEOUT)
#define DMAC_UNPOP_ACC              (CY_DMA_UNPOP_ACC)
#define DMAC_PERIPH_ERR             (CY_DMA_PERIPH_ERR)
#define ROUND_ROBIN_ENABLE          (CY_DMA_ROUND_ROBIN_ENABLE)
#define DMA_DISABLE_TD              (CY_DMA_DISABLE_TD)

#define DMAC_CFG                    (CY_DMA_CFG_PTR)
#define DMAC_ERR                    (CY_DMA_ERR_PTR)
#define DMAC_ERR_ADR                (CY_DMA_ERR_ADR_PTR)
#define DMAC_CH                     (CY_DMA_CH_STRUCT_PTR)
#define DMAC_CFGMEM                 (CY_DMA_CFGMEM_STRUCT_PTR)
#define DMAC_TDMEM                  (CY_DMA_TDMEM_STRUCT_PTR)

#endif  /* (CY_BOOT_CYDMAC_H) */


/* [] END OF FILE */
