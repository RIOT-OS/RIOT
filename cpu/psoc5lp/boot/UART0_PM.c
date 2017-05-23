/*******************************************************************************
* File Name: UART0_PM.c
* Version 2.30
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART0.h"


/***************************************
* Local data allocation
***************************************/

static UART0_BACKUP_STRUCT  UART0_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: UART0_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART0_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART0_SaveConfig(void)
{
    #if (CY_UDB_V0)

        #if(UART0_CONTROL_REG_REMOVED == 0u)
            UART0_backup.cr = UART0_CONTROL_REG;
        #endif /* End UART0_CONTROL_REG_REMOVED */

        #if( (UART0_RX_ENABLED) || (UART0_HD_ENABLED) )
            UART0_backup.rx_period = UART0_RXBITCTR_PERIOD_REG;
            UART0_backup.rx_mask = UART0_RXSTATUS_MASK_REG;
            #if (UART0_RXHW_ADDRESS_ENABLED)
                UART0_backup.rx_addr1 = UART0_RXADDRESS1_REG;
                UART0_backup.rx_addr2 = UART0_RXADDRESS2_REG;
            #endif /* End UART0_RXHW_ADDRESS_ENABLED */
        #endif /* End UART0_RX_ENABLED | UART0_HD_ENABLED*/

        #if(UART0_TX_ENABLED)
            #if(UART0_TXCLKGEN_DP)
                UART0_backup.tx_clk_ctr = UART0_TXBITCLKGEN_CTR_REG;
                UART0_backup.tx_clk_compl = UART0_TXBITCLKTX_COMPLETE_REG;
            #else
                UART0_backup.tx_period = UART0_TXBITCTR_PERIOD_REG;
            #endif /*End UART0_TXCLKGEN_DP */
            UART0_backup.tx_mask = UART0_TXSTATUS_MASK_REG;
        #endif /*End UART0_TX_ENABLED */


    #else /* CY_UDB_V1 */

        #if(UART0_CONTROL_REG_REMOVED == 0u)
            UART0_backup.cr = UART0_CONTROL_REG;
        #endif /* End UART0_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: UART0_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART0_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART0_RestoreConfig(void)
{

    #if (CY_UDB_V0)

        #if(UART0_CONTROL_REG_REMOVED == 0u)
            UART0_CONTROL_REG = UART0_backup.cr;
        #endif /* End UART0_CONTROL_REG_REMOVED */

        #if( (UART0_RX_ENABLED) || (UART0_HD_ENABLED) )
            UART0_RXBITCTR_PERIOD_REG = UART0_backup.rx_period;
            UART0_RXSTATUS_MASK_REG = UART0_backup.rx_mask;
            #if (UART0_RXHW_ADDRESS_ENABLED)
                UART0_RXADDRESS1_REG = UART0_backup.rx_addr1;
                UART0_RXADDRESS2_REG = UART0_backup.rx_addr2;
            #endif /* End UART0_RXHW_ADDRESS_ENABLED */
        #endif  /* End (UART0_RX_ENABLED) || (UART0_HD_ENABLED) */

        #if(UART0_TX_ENABLED)
            #if(UART0_TXCLKGEN_DP)
                UART0_TXBITCLKGEN_CTR_REG = UART0_backup.tx_clk_ctr;
                UART0_TXBITCLKTX_COMPLETE_REG = UART0_backup.tx_clk_compl;
            #else
                UART0_TXBITCTR_PERIOD_REG = UART0_backup.tx_period;
            #endif /*End UART0_TXCLKGEN_DP */
            UART0_TXSTATUS_MASK_REG = UART0_backup.tx_mask;
        #endif /*End UART0_TX_ENABLED */

    #else /* CY_UDB_V1 */

        #if(UART0_CONTROL_REG_REMOVED == 0u)
            UART0_CONTROL_REG = UART0_backup.cr;
        #endif /* End UART0_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: UART0_Sleep
********************************************************************************
*
* Summary:
*  Stops and saves the user configuration. Should be called
*  just prior to entering sleep.
*
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART0_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART0_Sleep(void)
{

    #if(UART0_RX_ENABLED || UART0_HD_ENABLED)
        if((UART0_RXSTATUS_ACTL_REG  & UART0_INT_ENABLE) != 0u)
        {
            UART0_backup.enableState = 1u;
        }
        else
        {
            UART0_backup.enableState = 0u;
        }
    #else
        if((UART0_TXSTATUS_ACTL_REG  & UART0_INT_ENABLE) !=0u)
        {
            UART0_backup.enableState = 1u;
        }
        else
        {
            UART0_backup.enableState = 0u;
        }
    #endif /* End UART0_RX_ENABLED || UART0_HD_ENABLED*/

    UART0_Stop();
    UART0_SaveConfig();
}


/*******************************************************************************
* Function Name: UART0_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called
*  just after awaking from sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART0_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART0_Wakeup(void)
{
    UART0_RestoreConfig();
    #if( (UART0_RX_ENABLED) || (UART0_HD_ENABLED) )
        UART0_ClearRxBuffer();
    #endif /* End (UART0_RX_ENABLED) || (UART0_HD_ENABLED) */
    #if(UART0_TX_ENABLED || UART0_HD_ENABLED)
        UART0_ClearTxBuffer();
    #endif /* End UART0_TX_ENABLED || UART0_HD_ENABLED */

    if(UART0_backup.enableState != 0u)
    {
        UART0_Enable();
    }
}


/* [] END OF FILE */
