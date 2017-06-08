/***************************************************************************//**
 * @file em_ldma.c
 * @brief Direct memory access (LDMA) module peripheral API
 * @version 4.2.1
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include "em_ldma.h"

#if defined( LDMA_PRESENT ) && ( LDMA_COUNT == 1 )

#include <stddef.h>
#include "em_assert.h"
#include "em_bus.h"
#include "em_cmu.h"
#include "em_int.h"

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
  @addtogroup LDMA
  @brief Direct Memory Access (LDMA) Peripheral API
  @details
  The LDMA API functions provide full support for the LDMA peripheral.

  The LDMA supports these DMA transfer types:

  @li Memory to memory.
  @li Memory to peripheral.
  @li Peripheral to memory.
  @li Peripheral to peripheral.
  @li Constant value to memory.

  The LDMA supports linked lists of DMA descriptors allowing:

  @li Circular and ping-pong buffer transfers.
  @li Scatter-gather transfers.
  @li Looped transfers.

  The LDMA has some advanced features:

  @li Intra-channel synchronization (SYNC), allowing hardware events to
      pause and restart a DMA sequence.
  @li Immediate-write (WRI), allowing the DMA to write a constant anywhere
      in the memory map.
  @li Complex flow control allowing if-else constructs.

  A basic understanding of the LDMA controller is assumed. Please refer to
  the reference manual for further details. The LDMA examples described
  in the reference manual are particularly helpful in understanding LDMA
  operations.

  In order to use the DMA controller, the initialization function @ref
  LDMA_Init() must have been executed once (normally during system init).

  DMA transfers are initiated by a call to @ref LDMA_StartTransfer(), the
  transfer properties are controlled by the contents of @ref LDMA_TransferCfg_t
  and @ref LDMA_Descriptor_t structure parameters.
  The @htmlonly LDMA_Descriptor_t @endhtmlonly structure parameter may be a
  pointer to an array of descriptors, the descriptors in the array should
  be linked together as needed.

  Transfer and descriptor initialization macros are provided for the most common
  transfer types. Due to the flexibility of the LDMA peripheral only a small
  subset of all possible initializer macros are provided, the user should create
  new one's when needed.

  @note The LDMA module does not implement the LDMA interrupt handler. A
  template for a handler is include in the code.

  <b> Examples of LDMA usage: </b>

  A simple memory to memory transfer:
  @verbatim
  // A single transfer of 4 half words.

  const LDMA_TransferCfg_t memTransfer = LDMA_TRANSFER_CFG_MEMORY();
  const LDMA_Descriptor_t xfer         = LDMA_DESCRIPTOR_SINGLE_M2M_HALF( src, dest, 4 );
  LDMA_Init_t init                     = LDMA_INIT_DEFAULT;

  LDMA_Init( &init );

  LDMA_StartTransfer( 0, (void*)&memTransfer, (void*)&xfer );
  @endverbatim

  @n A list of two memory to memory transfers:
  @verbatim
  // A transfer of 4 half words which links to another transfer of 4 half words.

  const LDMA_TransferCfg_t memTransfer = LDMA_TRANSFER_CFG_MEMORY();

  const LDMA_Descriptor_t xfer[] =
  {
    LDMA_DESCRIPTOR_LINKREL_M2M_HALF( src, dest     , 4, 1 ),
    LDMA_DESCRIPTOR_SINGLE_M2M_HALF ( src, dest + 10, 4 )
  };

  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  LDMA_Init( &init );

  LDMA_StartTransfer( 0, (void*)&memTransfer, (void*)&xfer );
  @endverbatim

  @n A list of three memory to memory transfers:
  @verbatim
  // A transfer of 4 half words which links to another transfer of 4 half words,
  // which again links to a third transfer of 4 half words.

  const LDMA_TransferCfg_t memTransfer = LDMA_TRANSFER_CFG_MEMORY();

  const LDMA_Descriptor_t xfer[] =
  {
    LDMA_DESCRIPTOR_LINKREL_M2M_HALF( src    , dest     , 4, 1 ),
    LDMA_DESCRIPTOR_LINKREL_M2M_HALF( src + 2, dest + 5 , 4, 1 ),
    LDMA_DESCRIPTOR_SINGLE_M2M_HALF ( src + 4, dest + 10, 4 )
  };

  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  LDMA_Init( &init );

  LDMA_StartTransfer( 0, (void*)&memTransfer, (void*)&xfer );
  @endverbatim

  @n DMA from serial port peripheral to memory:
  @verbatim
  // Transfer 4 chars from USART1.

  const LDMA_TransferCfg_t periTransferRx =
        LDMA_TRANSFER_CFG_PERIPHERAL( ldmaPeripheralSignal_USART1_RXDATAV );

  const LDMA_Descriptor_t xfer =
    LDMA_DESCRIPTOR_SINGLE_P2M_BYTE( &USART1->RXDATA, // Peripheral address
                                     dest,            // Destination (SRAM)
                                     4 );             // Number of bytes

  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  LDMA_Init( &init );

  LDMA_StartTransfer( 0, (void*)&periTransferRx, (void*)&xfer );
  @endverbatim

  @n Ping pong DMA from serial port peripheral to memory:
  @verbatim
  // Ping Pong transfer from USART1.

  static char buff1[5];
  static char buff2[5];

  const LDMA_TransferCfg_t periTransferRx =
        LDMA_TRANSFER_CFG_PERIPHERAL( ldmaPeripheralSignal_USART1_RXDATAV );

  const LDMA_Descriptor_t xfer[] =
  {
    // Note the 1 and -1 link jump increments. This will cause each DMA transfer
    // to link to the other one in an endless loop.
    LDMA_DESCRIPTOR_LINKREL_P2M_BYTE( &USART1->RXDATA,  // Peripheral address
                                      buff1,            // Destination (SRAM)
                                      5,                // Number of bytes
                                      1 ),              // Next descriptor
    LDMA_DESCRIPTOR_LINKREL_P2M_BYTE( &USART1->RXDATA,  // Peripheral address
                                      buff2,            // Destination (SRAM)
                                      5,                // Number of bytes
                                      -1 )              // Next descriptor
  };

  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  LDMA_Init( &init );

  LDMA_StartTransfer( 0, (void*)&periTransferRx, (void*)&xfer );
  @endverbatim
 * @{ *************************************************************************/

#if defined( LDMA_IRQ_HANDLER_TEMPLATE )
/***************************************************************************//**
 * @brief
 *   Template for an LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler( void )
{
  uint32_t pending, chnum, chmask;

  /* Get all pending and enabled interrupts */
  pending  = LDMA->IF;
  pending &= LDMA->IEN;

  /* Check for LDMA error */
  if ( pending & LDMA_IF_ERROR )
  {
    /* Loop here to enable the debugger to see what has happened */
    while (1)
      ;
  }

  /* Iterate over all LDMA channels. */
  for ( chnum = 0,                chmask = 1;
        chnum < DMA_CHAN_COUNT;
        chnum++,                  chmask <<= 1 )
  {
    if ( pending & chmask )
    {
      /* Clear interrupt flag. */
      LDMA->IFC = chmask;

      /* Do more stuff here, execute callbacks etc. */
    }
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   De-initialize the LDMA controller.
 *
 *   LDMA interrupts are disabled and the LDMA clock is stopped.
 ******************************************************************************/
void LDMA_DeInit( void )
{
  NVIC_DisableIRQ( LDMA_IRQn );
  LDMA->IEN  = 0;
  LDMA->CHEN = 0;
  CMU_ClockEnable( cmuClock_LDMA, false );
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller.
 *
 * @param[in] init
 *   Pointer to initialization structure used to configure the setup.
 ******************************************************************************/
void LDMA_Init( LDMA_Init_t *init )
{
  EFM_ASSERT( init != NULL );
  EFM_ASSERT( !( ( init->ldmaInitCtrlNumFixed << _LDMA_CTRL_NUMFIXED_SHIFT )
                 & ~_LDMA_CTRL_NUMFIXED_MASK ) );
  EFM_ASSERT( !( ( init->ldmaInitCtrlSyncPrsClrEn << _LDMA_CTRL_SYNCPRSCLREN_SHIFT )
                 & ~_LDMA_CTRL_SYNCPRSCLREN_MASK ) );
  EFM_ASSERT( !( ( init->ldmaInitCtrlSyncPrsSetEn << _LDMA_CTRL_SYNCPRSSETEN_SHIFT )
                 & ~_LDMA_CTRL_SYNCPRSSETEN_MASK ) );
  EFM_ASSERT( init->ldmaInitIrqPriority < ( 1 << __NVIC_PRIO_BITS ) );

  CMU_ClockEnable( cmuClock_LDMA, true );

  LDMA->CTRL = (   init->ldmaInitCtrlNumFixed     << _LDMA_CTRL_NUMFIXED_SHIFT     )
               | ( init->ldmaInitCtrlSyncPrsClrEn << _LDMA_CTRL_SYNCPRSCLREN_SHIFT )
               | ( init->ldmaInitCtrlSyncPrsSetEn << _LDMA_CTRL_SYNCPRSSETEN_SHIFT );

  LDMA->CHEN    = 0;
  LDMA->DBGHALT = 0;
  LDMA->REQDIS  = 0;

  /* Enable LDMA error interrupt. */
  LDMA->IEN = LDMA_IEN_ERROR;
  LDMA->IFC = 0xFFFFFFFF;

  NVIC_ClearPendingIRQ( LDMA_IRQn );

  /* Range is 0..7, 0 is highest priority. */
  NVIC_SetPriority( LDMA_IRQn, init->ldmaInitIrqPriority );

  NVIC_EnableIRQ( LDMA_IRQn );
}

/***************************************************************************//**
 * @brief
 *   Start a DMA transfer.
 *
 * @param[in] ch
 *   DMA channel.
 *
 * @param[in] transfer
 *   Initialization structure used to configure the transfer.
 *
 * @param[in] descriptor
 *   Transfer descriptor, can be an array of descriptors linked together.
 ******************************************************************************/
void LDMA_StartTransfer(  int ch,
                          LDMA_TransferCfg_t *transfer,
                          LDMA_Descriptor_t  *descriptor )
{
  uint32_t tmp;
  uint32_t chMask = 1 << ch;

  EFM_ASSERT( ch < DMA_CHAN_COUNT );
  EFM_ASSERT( transfer != NULL );
  EFM_ASSERT( !( transfer->ldmaReqSel & ~_LDMA_CH_REQSEL_MASK ) );

  EFM_ASSERT( !( ( transfer->ldmaCtrlSyncPrsClrOff << _LDMA_CTRL_SYNCPRSCLREN_SHIFT )
                 & ~_LDMA_CTRL_SYNCPRSCLREN_MASK ) );
  EFM_ASSERT( !( ( transfer->ldmaCtrlSyncPrsClrOn << _LDMA_CTRL_SYNCPRSCLREN_SHIFT )
                 & ~_LDMA_CTRL_SYNCPRSCLREN_MASK ) );
  EFM_ASSERT( !( ( transfer->ldmaCtrlSyncPrsSetOff << _LDMA_CTRL_SYNCPRSSETEN_SHIFT )
                 & ~_LDMA_CTRL_SYNCPRSSETEN_MASK ) );
  EFM_ASSERT( !( ( transfer->ldmaCtrlSyncPrsSetOn << _LDMA_CTRL_SYNCPRSSETEN_SHIFT  )
                 & ~_LDMA_CTRL_SYNCPRSSETEN_MASK ) );

  EFM_ASSERT( !( ( transfer->ldmaCfgArbSlots << _LDMA_CH_CFG_ARBSLOTS_SHIFT )
                 & ~_LDMA_CH_CFG_ARBSLOTS_MASK ) );
  EFM_ASSERT( !( ( transfer->ldmaCfgSrcIncSign << _LDMA_CH_CFG_SRCINCSIGN_SHIFT )
                 & ~_LDMA_CH_CFG_SRCINCSIGN_MASK ) );
  EFM_ASSERT( !( ( transfer->ldmaCfgDstIncSign << _LDMA_CH_CFG_DSTINCSIGN_SHIFT )
                 & ~_LDMA_CH_CFG_DSTINCSIGN_MASK ) );
  EFM_ASSERT( !( ( transfer->ldmaLoopCnt << _LDMA_CH_LOOP_LOOPCNT_SHIFT )
                 & ~_LDMA_CH_LOOP_LOOPCNT_MASK ) );

  LDMA->CH[ ch ].REQSEL = transfer->ldmaReqSel;

  LDMA->CH[ ch ].LOOP =
    (   transfer->ldmaLoopCnt       << _LDMA_CH_LOOP_LOOPCNT_SHIFT     );

  LDMA->CH[ ch ].CFG =
    ( transfer->ldmaCfgArbSlots     << _LDMA_CH_CFG_ARBSLOTS_SHIFT     )
    | ( transfer->ldmaCfgSrcIncSign << _LDMA_CH_CFG_SRCINCSIGN_SHIFT   )
    | ( transfer->ldmaCfgDstIncSign << _LDMA_CH_CFG_DSTINCSIGN_SHIFT   );

  /* Set descriptor address. */
  LDMA->CH[ ch ].LINK = (uint32_t)descriptor & _LDMA_CH_LINK_LINKADDR_MASK;

  /* Clear pending channel interrupt. */
  LDMA->IFC = chMask;

  /* Critical region. */
  INT_Disable();

  /* Enable channel interrupt. */
  LDMA->IEN |= chMask;

  if ( transfer->ldmaReqDis )
  {
    LDMA->REQDIS |= chMask;
  }

  if ( transfer->ldmaDbgHalt )
  {
    LDMA->DBGHALT |= chMask;
  }

  tmp = LDMA->CTRL;

  if ( transfer->ldmaCtrlSyncPrsClrOff )
  {
    tmp &= ~_LDMA_CTRL_SYNCPRSCLREN_MASK
           | (~transfer->ldmaCtrlSyncPrsClrOff << _LDMA_CTRL_SYNCPRSCLREN_SHIFT);
  }

  if ( transfer->ldmaCtrlSyncPrsClrOn )
  {
    tmp |= transfer->ldmaCtrlSyncPrsClrOn << _LDMA_CTRL_SYNCPRSCLREN_SHIFT;
  }

  if ( transfer->ldmaCtrlSyncPrsSetOff )
  {
    tmp &= ~_LDMA_CTRL_SYNCPRSSETEN_MASK
           | (~transfer->ldmaCtrlSyncPrsSetOff << _LDMA_CTRL_SYNCPRSSETEN_SHIFT);
  }

  if ( transfer->ldmaCtrlSyncPrsSetOn )
  {
    tmp |= transfer->ldmaCtrlSyncPrsSetOn << _LDMA_CTRL_SYNCPRSSETEN_SHIFT;
  }

  LDMA->CTRL = tmp;

  BUS_RegMaskedClear(&LDMA->CHDONE, chMask);  /* Clear the done flag.     */
  LDMA->LINKLOAD = chMask;                    /* Enable descriptor load.  */
  BUS_RegMaskedSet(&LDMA->CHEN, chMask);      /* Enable channel.          */

  /* Critical region end. */
  INT_Enable();
}

/***************************************************************************//**
 * @brief
 *   Stop a DMA transfer.
 *
 * @note
 *   The DMA will complete the current AHB burst transfer before stopping.
 *
 * @param[in] ch
 *   DMA channel to stop.
 ******************************************************************************/
void LDMA_StopTransfer( int ch )
{
  uint32_t chMask = 1 << ch;

  EFM_ASSERT( ch < DMA_CHAN_COUNT );

  INT_Disable();

  LDMA->IEN &= ~chMask;
  BUS_RegMaskedClear(&LDMA->CHEN, chMask);

  INT_Enable();
}

/***************************************************************************//**
 * @brief
 *   Check if a DMA transfer has completed.
 *
 * @param[in] ch
 *   DMA channel to check.
 *
 * @return
 *   True if transfer has completed, false if not.
 ******************************************************************************/
bool LDMA_TransferDone( int ch )
{
  bool     retVal = false;
  uint32_t chMask = 1 << ch;

  EFM_ASSERT( ch < DMA_CHAN_COUNT );

  INT_Disable();
  if (    ( ( LDMA->CHEN   & chMask ) == 0      )
       && ( ( LDMA->CHDONE & chMask ) == chMask ) )
  {
    retVal = true;
  }
  INT_Enable();
  return retVal;
}

/***************************************************************************//**
 * @brief
 *  Get number of items remaining in a transfer.
 *
 * @note
 *  This function is does not take into account that a DMA transfers with
 *  a chain of linked transfers might be ongoing. It will only check the
 *  count for the current transfer.
 *
 * @param[in] ch
 *  The channel number of the transfer to check.
 *
 * @return
 *  Number of items remaining in the transfer.
 ******************************************************************************/
uint32_t LDMA_TransferRemainingCount( int ch )
{
  uint32_t remaining, done, iflag;
  uint32_t chMask = 1 << ch;

  EFM_ASSERT( ch < DMA_CHAN_COUNT );

  INT_Disable();
  iflag  = LDMA->IF;
  done   = LDMA->CHDONE;
  remaining = LDMA->CH[ ch ].CTRL;
  INT_Enable();

  iflag    &= chMask;
  done     &= chMask;
  remaining = ( remaining
                & _LDMA_CH_CTRL_XFERCNT_MASK )
              >> _LDMA_CH_CTRL_XFERCNT_SHIFT;

  if ( done || ( ( remaining == 0 ) && iflag ) )
  {
    return 0;
  }

  return remaining + 1;
}

/** @} (end addtogroup LDMA) */
/** @} (end addtogroup EM_Library) */
#endif /* defined( LDMA_PRESENT ) && ( LDMA_COUNT == 1 ) */
