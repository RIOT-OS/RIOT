//*****************************************************************************
// cc_pal.c
//
// simplelink abstraction file for CC3200
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//Simplelink includes
#include <simplelink.h>
#include <cc_pal.h>

//Driverlib includes
#include <hw_ints.h>
#include <hw_udma.h>
#include <hw_types.h>
#include <pin.h>
#include <hw_memmap.h>
#include <hw_mcspi.h>
#include <hw_common_reg.h>
#include <rom.h>
#include <rom_map.h>
#include <spi.h>
#include <prcm.h>
#include <rom.h>
#include <rom_map.h>
#include <hw_ints.h>
#include <interrupt.h>
#include <udma.h>
#include <utils.h>

//OSLib includes
#if defined(SL_PLATFORM_MULTI_THREADED)
#include <osi.h>
#endif


#define REG_INT_MASK_SET                0x400F7088
#define REG_INT_MASK_CLR                0x400F708C
#define APPS_SOFT_RESET_REG             0x4402D000
#define OCP_SHARED_MAC_RESET_REG        0x4402E168
#define ROM_VERSION_ADDR                0x00000400

#define DMA_BUFF_SIZE_MIN               100
#define MAX_NUM_CH	                    64	//32*2 entries
#define MAX_DMA_RECV_TRANSACTION_SIZE   4000

//SPI rate determined by the ROM version
#define ROM_VER_PG1_21                  1
#define ROM_VER_PG1_32                  2
#define ROM_VER_PG1_33                  3
#define SPI_RATE_13M 		            13000000
#define SPI_RATE_20M                    20000000

#define UNUSED(x) 						x=x

//Structure definition to determine the ROM version
typedef struct
{
    unsigned short ucMajorVerNum;
    unsigned short ucMinorVerNum;
    unsigned short ucSubMinorVerNum;
    unsigned short ucDay;
    unsigned short ucMonth;
    unsigned short ucYear;
}tROMVersion;

//
// GLOBAL VARIABLES -- Start
//
volatile Fd_t g_SpiFd =0;
P_EVENT_HANDLER g_pHostIntHdl  = NULL;
unsigned long g_ucDinDout[20];
unsigned char g_ucDMAEnabled = 0;

#if defined(SL_PLATFORM_MULTI_THREADED)
OsiMsgQ_t DMAMsgQ;
char g_cDummy[4];
#else //SL_PLATFORM_MULTI_THREADED
volatile char g_cDummy;
#endif //SL_PLATFORM_MULTI_THREADED

//
// GLOBAL VARIABLES -- End
//


//****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//****************************************************************************
static int spi_Read_CPU(unsigned char *pBuff, int len);
static int spi_Write_CPU(unsigned char *pBuff, int len);
void DmaSpiSwIntHandler();
static void SetupDMAReceive(unsigned char *ucBuff,int len);
static void SetupDMASend(unsigned char *ucBuff,int len);
static void cc_UDMAChannelSelect(unsigned int uiChannel);
static void cc_SetupTransfer(unsigned long ulChannel,
					  unsigned long ulMode,
					  unsigned long ulItemCount,
					  unsigned long ulItemSize,
					  unsigned long ulArbSize,
					  void *pvSrcBuf,
					  unsigned long ulSrcInc,
					  void *pvDstBuf,
					  unsigned long ulDstInc);

//****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//****************************************************************************

/*!
    \brief attempts to read up to len bytes from SPI channel into a buffer starting at pBuff.

	\param			pBuff		- 	points to first location to start writing the data

	\param			len			-	number of bytes to read from the SPI channel

	\return			upon successful completion, the function shall return Read Size.
					Otherwise, -1 shall be returned

    \sa             spi_Read_CPU , spi_Write_CPU
	\note
    \warning
*/
int spi_Read_CPU(unsigned char *pBuff, int len)
{
    unsigned long ulCnt;
    unsigned long ulStatusReg;
    unsigned long *ulDataIn;
    unsigned long ulTxReg;
    unsigned long ulRxReg;

    MAP_SPICSEnable(LSPI_BASE);

    //
    // Initialize local variable.
    //
    ulDataIn = (unsigned long *)pBuff;
    ulCnt = (len + 3) >> 2;
    ulStatusReg = LSPI_BASE+MCSPI_O_CH0STAT;
    ulTxReg = LSPI_BASE + MCSPI_O_TX0;
    ulRxReg = LSPI_BASE + MCSPI_O_RX0;

    //
    // Reading loop
    //
    while(ulCnt--)
    {
          while(!( HWREG(ulStatusReg)& MCSPI_CH0STAT_TXS ));
          HWREG(ulTxReg) = 0xFFFFFFFF;
          while(!( HWREG(ulStatusReg)& MCSPI_CH0STAT_RXS ));
          *ulDataIn = HWREG(ulRxReg);
          ulDataIn++;
    }

    MAP_SPICSDisable(LSPI_BASE);

    return len;
}

/*!
    \brief attempts to write up to len bytes to the SPI channel

	\param			pBuff		- 	points to first location to start getting the data from

	\param			len			-	number of bytes to write to the SPI channel

	\return			upon successful completion, the function shall return write size.
					Otherwise, -1 shall be returned

    \sa             spi_Read_CPU , spi_Write_CPU
	\note			This function could be implemented as zero copy and return only upon successful completion
					of writing the whole buffer, but in cases that memory allocation is not too tight, the
					function could copy the data to internal buffer, return back and complete the write in
					parallel to other activities as long as the other SPI activities would be blocked untill
					the entire buffer write would be completed
    \warning
*/
int spi_Write_CPU(unsigned char *pBuff, int len)
{


    unsigned long ulCnt;
    unsigned long ulStatusReg;
    unsigned long *ulDataOut;
    unsigned long ulDataIn;
    unsigned long ulTxReg;
    unsigned long ulRxReg;


    MAP_SPICSEnable(LSPI_BASE);

    //
    // Initialize local variable.
    //
    ulDataOut = (unsigned long *)pBuff;
    ulCnt = (len +3 ) >> 2;
    ulStatusReg = LSPI_BASE+MCSPI_O_CH0STAT;
    ulTxReg = LSPI_BASE + MCSPI_O_TX0;
    ulRxReg = LSPI_BASE + MCSPI_O_RX0;

    //
    // Writing Loop
    //
    while(ulCnt--)
    {
          while(!( HWREG(ulStatusReg)& MCSPI_CH0STAT_TXS ));
          HWREG(ulTxReg) = *ulDataOut;
          while(!( HWREG(ulStatusReg)& MCSPI_CH0STAT_RXS ));
          ulDataIn = HWREG(ulRxReg);
          ulDataOut++;
    }

    MAP_SPICSDisable(LSPI_BASE);

    UNUSED(ulDataIn);
    return len;

}



/*!
    \brief  DMA SPI interrupt handler

	\param			None

	\return			None

	\note			This function
       	   	   	   	   1. Invoked when SPI Transaction Completes
    \warning
*/
void DmaSpiSwIntHandler()
{
	MAP_SPIIntClear(LSPI_BASE,SPI_INT_EOW);
	MAP_SPICSDisable(LSPI_BASE);

#if defined(SL_PLATFORM_MULTI_THREADED)
    osi_MsgQWrite(&DMAMsgQ,g_cDummy,OSI_NO_WAIT);
#else
    g_cDummy = 0x1;
#endif
}


/*!
    \brief  This function is used to set DMA receive on th SPI

	\param			ucBuff	- 	points to the first location of the receive buffer

	\param			len		- Length of the data to be received

	\sa             SetupDMASend

	\return			None

    \warning
*/

void SetupDMAReceive(unsigned char *ucBuff,int len)
{

	cc_SetupTransfer(UDMA_CH13_LSPI_TX,
                  	  	  UDMA_MODE_BASIC,
                  	  	  len/4,
                  	  	  UDMA_SIZE_32,
                  	  	  UDMA_ARB_1,
                  	  	  &g_ucDinDout[5],
                  	  	  UDMA_SRC_INC_NONE,
                  	  	  (void *)(LSPI_BASE+MCSPI_O_TX0),
                  	  	  UDMA_DST_INC_NONE);

	cc_SetupTransfer(UDMA_CH12_LSPI_RX,
                  	  	  UDMA_MODE_BASIC,
                  	  	  len/4,
                  	  	  UDMA_SIZE_32,
                  	  	  UDMA_ARB_1,
                  	  	  (void *)(LSPI_BASE+MCSPI_O_RX0),
                  	  	  UDMA_SRC_INC_NONE,
                  	  	  (void *)ucBuff,
                  	  	  UDMA_DST_INC_32);

	MAP_SPIWordCountSet(LSPI_BASE,len/4);

}


/*!
    \brief  This function is used to set DMA send on th SPI

	\param			ucBuff	- 	points to the first location of the send buffer

	\param			len		- Length of the data to be sent

	\sa             SetupDMAReceive

	\return			None

    \warning
*/

void SetupDMASend(unsigned char *ucBuff,int len)
{

	cc_SetupTransfer(UDMA_CH13_LSPI_TX,
                  	  	  UDMA_MODE_BASIC,
                  	  	  len/4,
                  	  	  UDMA_SIZE_32,
                  	  	  UDMA_ARB_1,
                  	  	  (void *)ucBuff,
                  	  	  UDMA_SRC_INC_32,
                  	  	  (void *)(LSPI_BASE+MCSPI_O_TX0),
                  	  	  UDMA_DST_INC_NONE);


	cc_SetupTransfer(UDMA_CH12_LSPI_RX,
                  	  	  UDMA_MODE_BASIC,
                  	  	  len/4,
                  	  	  UDMA_SIZE_32,
                  	  	  UDMA_ARB_1,
                  	  	  (void *)(LSPI_BASE+MCSPI_O_RX0),
                  	  	  UDMA_SRC_INC_NONE,
                  	  	  &g_ucDinDout[15],
                  	  	  UDMA_DST_INC_NONE);

	MAP_SPIWordCountSet(LSPI_BASE,len/4);


}

/*!
    \brief open spi communication port to be used for communicating with a SimpleLink device

	Given an interface name and option flags, this function opens the spi communication port
	and creates a file descriptor. This file descriptor can be used afterwards to read and
	write data from and to this specific spi channel.
	The SPI speed, clock polarity, clock phase, chip select and all other attributes are all
	set to hardcoded values in this function.

	\param	 		ifName		-	points to the interface name/path. The interface name is an
									optional attributes that the simple link driver receives
									on opening the device. in systems that the spi channel is
									not implemented as part of the os device drivers, this
									parameter could be NULL.
	\param			flags		-	option flags

	\return			upon successful completion, the function shall open the spi channel and return
					a non-negative integer representing the file descriptor.
					Otherwise, -1 shall be returned

    \sa             spi_Close , spi_Read , spi_Write
	\note
    \warning
*/

Fd_t spi_Open(char *ifName, unsigned long flags)
{
    unsigned long ulBase;
    unsigned long ulSpiBitRate = 0;
    tROMVersion* pRomVersion = (tROMVersion *)(ROM_VERSION_ADDR);


    //NWP master interface
    ulBase = LSPI_BASE;

    //Enable MCSPIA2
    MAP_PRCMPeripheralClkEnable(PRCM_LSPI,PRCM_RUN_MODE_CLK|PRCM_SLP_MODE_CLK);

    //Disable Chip Select
    MAP_SPICSDisable(ulBase);

    //Disable SPI Channel
    MAP_SPIDisable(ulBase);

    // Reset SPI
    MAP_SPIReset(ulBase);

    //
    // Configure SPI interface
	//

    if(pRomVersion->ucMinorVerNum == ROM_VER_PG1_21 )
    {
    	ulSpiBitRate = SPI_RATE_13M;
    }
    else if(pRomVersion->ucMinorVerNum == ROM_VER_PG1_32)
    {
    	ulSpiBitRate = SPI_RATE_13M;
    }
    else if(pRomVersion->ucMinorVerNum >= ROM_VER_PG1_33)
    {
    	ulSpiBitRate = SPI_RATE_20M;
    }

    MAP_SPIConfigSetExpClk(ulBase,MAP_PRCMPeripheralClockGet(PRCM_LSPI),
		  	  	  	 ulSpiBitRate,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                     (SPI_SW_CTRL_CS |
                     SPI_4PIN_MODE |
                     SPI_TURBO_OFF |
                     SPI_CS_ACTIVEHIGH |
                     SPI_WL_32));

	if(MAP_PRCMPeripheralStatusGet(PRCM_UDMA))
	{
	  g_ucDMAEnabled = (HWREG(UDMA_BASE + UDMA_O_CTLBASE) != 0x0) ? 1 : 0;
	}
	else
	{
		g_ucDMAEnabled = 0;
	}
	#ifdef SL_CPU_MODE
	g_ucDMAEnabled = 0;
	#endif
	if(g_ucDMAEnabled)
	{
		memset(g_ucDinDout,0xFF,sizeof(g_ucDinDout));

		// Set DMA channel
		cc_UDMAChannelSelect(UDMA_CH12_LSPI_RX);
		cc_UDMAChannelSelect(UDMA_CH13_LSPI_TX);


		MAP_SPIFIFOEnable(ulBase,SPI_RX_FIFO);
		MAP_SPIFIFOEnable(ulBase,SPI_TX_FIFO);
		MAP_SPIDmaEnable(ulBase,SPI_RX_DMA);
		MAP_SPIDmaEnable(ulBase,SPI_TX_DMA);

		MAP_SPIFIFOLevelSet(ulBase,1,1);
	#if defined(SL_PLATFORM_MULTI_THREADED)
		osi_InterruptRegister(INT_LSPI, (P_OSI_INTR_ENTRY)DmaSpiSwIntHandler,INT_PRIORITY_LVL_1);
		MAP_SPIIntEnable(ulBase,SPI_INT_EOW);


		osi_MsgQCreate(&DMAMsgQ,"DMAQueue",sizeof(int),1);

	#else

		MAP_IntRegister(INT_LSPI,(void(*)(void))DmaSpiSwIntHandler);
		MAP_IntPrioritySet(INT_LSPI, INT_PRIORITY_LVL_1);
		MAP_IntEnable(INT_LSPI);

		MAP_SPIIntEnable(ulBase,SPI_INT_EOW);


		g_cDummy = 0x0;
	#endif

	}
	MAP_SPIEnable(ulBase);

    g_SpiFd = 1;
    return g_SpiFd;

}
/*!
    \brief closes an opened spi communication port

	\param	 		fd			-	file descriptor of an opened SPI channel

	\return			upon successful completion, the function shall return 0.
					Otherwise, -1 shall be returned

    \sa             spi_Open
	\note
    \warning
*/
int spi_Close(Fd_t fd)
{
    unsigned long ulBase = LSPI_BASE;

    g_SpiFd = 0;
	if(g_ucDMAEnabled)
	{

		#ifdef SL_PLATFORM_MULTI_THREADED
		  osi_InterruptDeRegister(INT_LSPI);
		  osi_MsgQDelete(&DMAMsgQ);

		#else
		  MAP_SPIIntUnregister(ulBase);
		  g_cDummy = 0;
		#endif
		  MAP_SPIFIFODisable(ulBase,SPI_RX_FIFO);
		  MAP_SPIFIFODisable(ulBase,SPI_TX_FIFO);
		  MAP_SPIDmaDisable(ulBase,SPI_RX_DMA);
		  MAP_SPIDmaDisable(ulBase,SPI_TX_DMA);
	}
		//Disable Chip Select
	MAP_SPICSDisable(LSPI_BASE);


		//Disable SPI Channel
	MAP_SPIDisable(ulBase);

		// Reset SPI
	MAP_SPIReset(ulBase);

		// Enable SPI Peripheral
	MAP_PRCMPeripheralClkDisable(PRCM_LSPI,PRCM_RUN_MODE_CLK|PRCM_SLP_MODE_CLK);

    return 0;
}

/*!
    \brief closes an opened spi communication port

	\param	 		fd			-	file descriptor of an opened SPI channel

	\return			upon successful completion, the function shall return 0.
					Otherwise, -1 shall be returned

    \sa             spi_Open
	\note
    \warning
*/

int spi_Read(Fd_t fd, unsigned char *pBuff, int len)
{
    int read_size = 0;
    unsigned int pBuffAddr = (unsigned int)pBuff;
    if(fd!=1 || g_SpiFd!=1)
    return -1;

	if(len>DMA_BUFF_SIZE_MIN && g_ucDMAEnabled && ((pBuffAddr % 4) == 0))
	{

#if defined(SL_PLATFORM_MULTI_THREADED)
			char temp[4];
#endif
			while (len>0)
			{
				if( len < MAX_DMA_RECV_TRANSACTION_SIZE)
				{
				   SetupDMAReceive(&pBuff[read_size],len);
				   MAP_SPICSEnable(LSPI_BASE);
#if defined(SL_PLATFORM_MULTI_THREADED)
				   osi_MsgQRead(&DMAMsgQ,temp,OSI_WAIT_FOREVER);
#else
				   while(g_cDummy != 0x1);
				   g_cDummy = 0x0;
#endif
				   read_size += len;
				   len = 0;
				}
				else
				{
					SetupDMAReceive(&pBuff[read_size],MAX_DMA_RECV_TRANSACTION_SIZE);
					MAP_SPICSEnable(LSPI_BASE);
#if defined(SL_PLATFORM_MULTI_THREADED)
				   osi_MsgQRead(&DMAMsgQ,temp,OSI_WAIT_FOREVER);
#else
				   while(g_cDummy != 0x1);
				   g_cDummy = 0x0;
#endif
					read_size += MAX_DMA_RECV_TRANSACTION_SIZE;

					len -= MAX_DMA_RECV_TRANSACTION_SIZE;
				}
			}
	}
	else
	{
		MAP_SPIWordCountSet(LSPI_BASE,0);
		read_size += spi_Read_CPU(pBuff,len);
	}
    return read_size;
}

/*!
    \brief attempts to write up to len bytes to the SPI channel

	\param	 		fd			-	file descriptor of an opened SPI channel

	\param			pBuff		- 	points to first location to start getting the data from

	\param			len			-	number of bytes to write to the SPI channel

	\return			upon successful completion, the function shall return 0.
					Otherwise, -1 shall be returned

    \sa             spi_Open , spi_Read
	\note			This function could be implemented as zero copy and return only upon successful completion
					of writing the whole buffer, but in cases that memory allocation is not too tight, the
					function could copy the data to internal buffer, return back and complete the write in
					parallel to other activities as long as the other SPI activities would be blocked untill
					the entire buffer write would be completed
    \warning
*/
int spi_Write(Fd_t fd, unsigned char *pBuff, int len)
{
    int write_size = 0;
    unsigned int pBuffAddr = (unsigned int)pBuff;

    if(fd!=1 || g_SpiFd!=1)
        return -1;

	if(len>DMA_BUFF_SIZE_MIN && g_ucDMAEnabled && ((pBuffAddr % 4) == 0))
	{
#if defined(SL_PLATFORM_MULTI_THREADED)
        char temp[4];
#endif
	    SetupDMASend(pBuff,len);
	    MAP_SPICSEnable(LSPI_BASE);

#if defined(SL_PLATFORM_MULTI_THREADED)
				   osi_MsgQRead(&DMAMsgQ,temp,OSI_WAIT_FOREVER);
#else
				   while(g_cDummy != 0x1);
				   g_cDummy = 0x0;
#endif
            write_size += len;
	}

	else
	{
		MAP_SPIWordCountSet(LSPI_BASE,0);
		write_size += spi_Write_CPU(pBuff,len);
	}
    return write_size;
}

/*!
    \brief register an interrupt handler for the host IRQ

	\param	 		InterruptHdl	-	pointer to interrupt handler function

	\param 			pValue			-	pointer to a memory strcuture that is passed to the interrupt handler.

	\return			upon successful registration, the function shall return 0.
					Otherwise, -1 shall be returned

    \sa
	\note			If there is already registered interrupt handler, the function should overwrite the old handler
					with the new one
    \warning
*/

int NwpRegisterInterruptHandler(P_EVENT_HANDLER InterruptHdl , void* pValue)
{

    if(InterruptHdl == NULL)
    {
		//De-register Interprocessor communication interrupt between App and NWP
		#ifdef SL_PLATFORM_MULTI_THREADED
		  osi_InterruptDeRegister(INT_NWPIC);
		#else
		  MAP_IntDisable(INT_NWPIC);
		  MAP_IntUnregister(INT_NWPIC);
		  MAP_IntPendClear(INT_NWPIC);
		#endif
    }
    else
    {
		  #ifdef SL_PLATFORM_MULTI_THREADED
			 MAP_IntPendClear(INT_NWPIC);
			 osi_InterruptRegister(INT_NWPIC, (P_OSI_INTR_ENTRY)InterruptHdl,INT_PRIORITY_LVL_1);
		  #else
			 MAP_IntRegister(INT_NWPIC, InterruptHdl);
			 MAP_IntPrioritySet(INT_NWPIC, INT_PRIORITY_LVL_1);
			 MAP_IntPendClear(INT_NWPIC);
			 MAP_IntEnable(INT_NWPIC);
		  #endif
    }

  return 0;
}


/*!
    \brief 				Masks host IRQ


    \sa             		NwpUnMaskInterrupt

    \warning
*/
void NwpMaskInterrupt()
{
	(*(unsigned long *)REG_INT_MASK_SET) = 0x1;
}


/*!
    \brief 				Unmasks host IRQ


    \sa             		NwpMaskInterrupt

    \warning
*/
void NwpUnMaskInterrupt()
{
	(*(unsigned long *)REG_INT_MASK_CLR) = 0x1;
}

#ifndef DISABLE_DEBUGGER_RECONNECT
/*!
    \brief		Preamble to the enabling the Network Processor.
                        Placeholder to implement any pre-process operations
                        before enabling networking operations.

    \sa			sl_DeviceEnable

    \note       belongs to \ref ported_sec

*/
void NwpPowerOnPreamble(void)
{
#ifndef CC3200_ES_1_2_1

#define MAX_RETRY_COUNT         1000
    unsigned int sl_stop_ind, apps_int_sts_raw, nwp_lpds_wake_cfg;
    unsigned int retry_count;
    /* Perform the sl_stop equivalent to ensure network services
       are turned off if active */
    HWREG(0x400F70B8) = 1;   /* APPs to NWP interrupt */
    UtilsDelay(800000/5);

    retry_count = 0;
    nwp_lpds_wake_cfg = HWREG(0x4402D404);
    sl_stop_ind = HWREG(0x4402E16C);

    if((nwp_lpds_wake_cfg != 0x20) && /* Check for NWP POR condition */
            !(sl_stop_ind & 0x2))     /* Check if sl_stop was executed */
    {
        /* Loop until APPs->NWP interrupt is cleared or timeout */
        while(retry_count < MAX_RETRY_COUNT)
        {
            apps_int_sts_raw = HWREG(0x400F70C0);
            if(apps_int_sts_raw & 0x1)
            {
                UtilsDelay(800000/5);
                retry_count++;
            }
            else
            {
                break;
            }
        }
    }
    HWREG(0x400F70B0) = 1;   /* Clear APPs to NWP interrupt */
    UtilsDelay(800000/5);

    /* Stop the networking services */
    NwpPowerOff();
#endif
}
#endif

/*!
    \brief		Enable the Network Processor

    \sa			sl_DeviceDisable

    \note       belongs to \ref ported_sec

*/
void NwpPowerOn(void)
{

#ifdef CC3200_ES_1_2_1
    //SPI CLK GATING
    HWREG(0x440250C8) = 0;

    //WLAN PD ON
    HWREG(OCP_SHARED_MAC_RESET_REG) &= ~(0xC00);

    //Remove NWP Reset
    HWREG(APPS_SOFT_RESET_REG) &= ~4;
#else
    //bring the 1.32 eco out of reset
    HWREG(0x4402E16C) &= 0xFFFFFFFD;
#endif

    //NWP Wakeup
    HWREG(0x44025118) = 1;
#ifndef DISABLE_DEBUGGER_RECONNECT
    UtilsDelay(8000000);
#endif

    //UnMask Host Interrupt
    NwpUnMaskInterrupt();
}


/*!
    \brief		Disable the Network Processor

    \sa			sl_DeviceEnable

    \note       belongs to \ref ported_sec
*/
void NwpPowerOff(void)
{
	//Must delay 300 usec to enable the NWP to finish all sl_stop activities
	UtilsDelay(300*80/3);

    //Mask Host Interrupt
    NwpMaskInterrupt();
    
    //Switch to PFM Mode
    HWREG(0x4402F024) &= 0xF7FFFFFF;
#ifdef CC3200_ES_1_2_1
    //Reset NWP
    HWREG(APPS_SOFT_RESET_REG) |= 4; 
    //WLAN PD OFF
    HWREG(OCP_SHARED_MAC_RESET_REG) |= 0xC00;
#else
    //sl_stop eco for PG1.32 devices
    HWREG(0x4402E16C) |= 0x2;

    UtilsDelay(800000);
#endif
}

/*!
    \brief      Disable the Network Processor. This API additionally ensures that 
                the Network processor has entered Low power mode before shutting
                it down

    \sa         sl_DeviceEnable

    \note       belongs to \ref ported_sec
*/
void NwpPowerOff_WithNwpLpdsPoll(void)
{
    //Must delay 300 usec to enable the NWP to finish all sl_stop activities
	UtilsDelay(300*80/3);
    
    //Mask Host Interrupt
    NwpMaskInterrupt();
    
#ifdef CC3200_ES_1_2_1
    //Reset NWP
    HWREG(APPS_SOFT_RESET_REG) |= 4; 
    //WLAN PD OFF
    HWREG(OCP_SHARED_MAC_RESET_REG) |= 0xC00;
#else
    
    // Dynamic delay loop to make sure the NWP has entered low power mode
    {
        // Wait for NWP to enter LPDS with 100 mSec timeout
        unsigned int NwpActive;
        long TimeoutUsec;

        TimeoutUsec = 100000;

        do
        {
            NwpActive = ((HWREG(0x4402DC48) & 0xF00) == 0x300);
            TimeoutUsec -= 10;
            UtilsDelay(800/6);
        }while (NwpActive && (TimeoutUsec > 0));        
    }  
    
    // Switch to PFM Mode
    // Note: make sure the switch to PFM Mode is done just before killing the
    // network processor
    HWREG(0x4402F024) &= 0xF7FFFFFF;
    
    //Killing the NWP
    HWREG(0x4402E16C) |= 0x2;

    UtilsDelay(800000);
#endif
}


/*!
 	 \brief		Configures the uDMA channel

 	 \param uiChannel - the DMA channel to be selected

	 \param pfpAppCb - the application callback to be invoked on transfer

     \return None.
*/
void cc_UDMAChannelSelect(unsigned int uiChannel)
{
    if((uiChannel & 0xFF) > MAX_NUM_CH)
    {
        return;
    }
    MAP_uDMAChannelAssign(uiChannel);
    MAP_uDMAChannelAttributeDisable(uiChannel,UDMA_ATTR_ALTSELECT);

}


/*!
 	 \brief		Does the actual Memory transfer

 	 \param ulChannel - the DMA channel to be selected

	 \param ulMode

	 \param ulItemCount

	 \param ulItemSize

	 \param ulArbSize

	 \param pvSrcBuf

	 \param ulSrcInc

	 \param pvDstBuf

	 \param ulDstInc

     \return None.
*/
void cc_SetupTransfer(
                  unsigned long ulChannel,
                  unsigned long ulMode,
                  unsigned long ulItemCount,
                  unsigned long ulItemSize,
                  unsigned long ulArbSize,
                  void *pvSrcBuf,
                  unsigned long ulSrcInc,
                  void *pvDstBuf,
                  unsigned long ulDstInc)
{

    MAP_uDMAChannelControlSet(ulChannel,
                              ulItemSize | ulSrcInc | ulDstInc | ulArbSize);

    MAP_uDMAChannelAttributeEnable(ulChannel,UDMA_ATTR_USEBURST);

    MAP_uDMAChannelTransferSet(ulChannel, ulMode,
                               pvSrcBuf, pvDstBuf, ulItemCount);

    MAP_uDMAChannelEnable(ulChannel);

}
