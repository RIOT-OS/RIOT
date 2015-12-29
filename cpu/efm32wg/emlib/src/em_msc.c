/***************************************************************************//**
 * @file em_msc.c
 * @brief Flash controller (MSC) Peripheral API
 * @version 4.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
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

#include "em_msc.h"
#if defined( MSC_COUNT ) && ( MSC_COUNT > 0 )

#include "em_system.h"
#if defined( _MSC_TIMEBASE_MASK )
#include "em_cmu.h"
#include "em_int.h"
#endif
#include "em_assert.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#if defined( MSC_WRITECTRL_WDOUBLE )
#define WORDS_PER_DATA_PHASE (FLASH_SIZE < (512 * 1024) ? 1 : 2)
#else
#define WORDS_PER_DATA_PHASE (1)
#endif

typedef enum {
  mscWriteIntSafe,
  mscWriteFast,
} MSC_WriteStrategy_Typedef;

#ifdef __ICCARM__ /* IAR compiler */
__ramfunc static MSC_Status_TypeDef MSC_WriteWordI(uint32_t *address,
                                         void const *data,
                                         uint32_t numBytes,
                                         MSC_WriteStrategy_Typedef writeStrategy);
__ramfunc __STATIC_INLINE MSC_Status_TypeDef MSC_LoadWriteData(uint32_t* data,
                                                     uint32_t numWords,
                                                     MSC_WriteStrategy_Typedef writeStrategy);
__ramfunc __STATIC_INLINE MSC_Status_TypeDef MSC_LoadVerifyAddress(uint32_t* address);
#endif
#ifdef __GNUC__  /* GCC based compilers */
#ifdef __CROSSWORKS_ARM  /* Rowley Crossworks (GCC based) */
static MSC_Status_TypeDef MSC_WriteWordI(uint32_t *address,
                                         void const *data,
                                         uint32_t numBytes,
                                         MSC_WriteStrategy_Typedef writeStrategy) __attribute__ ((section(".fast")));
__STATIC_INLINE MSC_Status_TypeDef MSC_LoadWriteData(uint32_t* data,
                                                     uint32_t numWords,
                                                     MSC_WriteStrategy_Typedef writeStrategy) __attribute__ ((section(".fast")));
__STATIC_INLINE MSC_Status_TypeDef MSC_LoadVerifyAddress(uint32_t* address) __attribute__ ((section(".fast")));
#else /* GCC */
static MSC_Status_TypeDef MSC_WriteWordI(uint32_t *address,
                                         void const *data,
                                         uint32_t numBytes,
                                         MSC_WriteStrategy_Typedef writeStrategy) __attribute__ ((section(".ram")));
__STATIC_INLINE MSC_Status_TypeDef MSC_LoadWriteData(uint32_t* data,
                                                     uint32_t numWords,
                                                     MSC_WriteStrategy_Typedef writeStrategy) __attribute__ ((section(".ram")));
__STATIC_INLINE MSC_Status_TypeDef MSC_LoadVerifyAddress(uint32_t* address) __attribute__ ((section(".ram")));
#endif /* __CROSSWORKS_ARM */
#endif /* __GNUC__ */

/** @endcond */

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup MSC
 * @brief Flash controller (MSC) Peripheral API
 * @{
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Enables the flash controller for writing.
 * @note
 *   IMPORTANT: This function must be called before flash operations when
 *   AUXHFRCO clock has been changed from default 14MHz band.
 * @note
 *   This function calls SystemCoreClockGet in order to set the global variable
 *   SystemCoreClock which is used in subseqent calls of MSC_WriteWord to make
 *   sure the frequency is sufficiently high for flash operations. If the clock
 *   frequency is changed then software is responsible for calling MSC_Init or
 *   SystemCoreClockGet in order to set the SystemCoreClock variable to the
 *   correct value.
 ******************************************************************************/
void MSC_Init(void)
{
#if defined( _MSC_TIMEBASE_MASK )
  uint32_t freq, cycles;
#endif
  /* Unlock the MSC */
  MSC->LOCK = MSC_UNLOCK_CODE;
  /* Disable writing to the flash */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;

  /* Call SystemCoreClockGet in order to set the global variable SystemCoreClock
     which is used in MSC_LoadWriteData to make sure the frequency is
     sufficiently high. If the clock frequency is changed then software is
     responsible for calling MSC_Init or SystemCoreClockGet in order to set the
     SystemCoreClock variable to the correct value. */
  SystemCoreClockGet();

#if defined( _MSC_TIMEBASE_MASK )
  /* Configure MSC->TIMEBASE according to selected frequency */
  freq = CMU_ClockFreqGet(cmuClock_AUX);

  if (freq > 7000000)
  {
    /* Calculate number of clock cycles for 1us as base period */
    freq   = (freq * 11) / 10;
    cycles = (freq / 1000000) + 1;

    /* Configure clock cycles for flash timing */
    MSC->TIMEBASE = (MSC->TIMEBASE & ~(_MSC_TIMEBASE_BASE_MASK |
                                       _MSC_TIMEBASE_PERIOD_MASK)) |
                    MSC_TIMEBASE_PERIOD_1US |
                    (cycles << _MSC_TIMEBASE_BASE_SHIFT);
  }
  else
  {
    /* Calculate number of clock cycles for 5us as base period */
    freq   = (freq * 5 * 11) / 10;
    cycles = (freq / 1000000) + 1;

    /* Configure clock cycles for flash timing */
    MSC->TIMEBASE = (MSC->TIMEBASE & ~(_MSC_TIMEBASE_BASE_MASK |
                                       _MSC_TIMEBASE_PERIOD_MASK)) |
                    MSC_TIMEBASE_PERIOD_5US |
                    (cycles << _MSC_TIMEBASE_BASE_SHIFT);
  }
#endif
}

/***************************************************************************//**
 * @brief
 *   Disables the flash controller for writing.
 ******************************************************************************/
void MSC_Deinit(void)
{
  /* Disable writing to the flash */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
  /* Lock the MSC */
  MSC->LOCK = 0;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Perform address phase of Flash write cycle.
 * @details
 *   This function performs the address phase of a Flash write operation by
 *   writing the given flash address to the ADDRB register and issuing the
 *   LADDRIM command to load the address.
 * @param[in] address
 *   Address in flash memory. Must be aligned at a 4 byte boundary.
 * @return
 *   Returns the status of the address load operation, #MSC_Status_TypeDef
 * @verbatim
 *   mscReturnOk - Operation completed successfully.
 *   mscReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   mscReturnLocked - Operation tried to erase a locked area of the flash.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#ifdef __ICCARM__
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
__ramfunc
#endif
__STATIC_INLINE MSC_Status_TypeDef MSC_LoadVerifyAddress(uint32_t* address)
{
  uint32_t status;
  uint32_t timeOut;

  /* Wait for the MSC to become ready. */
  timeOut = MSC_PROGRAM_TIMEOUT;
  while ((MSC->STATUS & MSC_STATUS_BUSY) && (timeOut != 0))
  {
    timeOut--;
  }

  /* Check for timeout */
  if (timeOut == 0)
  {
    return mscReturnTimeOut;
  }
  /* Load address */
  MSC->ADDRB    = (uint32_t)address;
  MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

  status = MSC->STATUS;
  if (status & (MSC_STATUS_INVADDR | MSC_STATUS_LOCKED))
  {
    /* Check for invalid address */
    if (status & MSC_STATUS_INVADDR)
      return mscReturnInvalidAddr;
    /* Check for write protected page */
    if (status & MSC_STATUS_LOCKED)
      return mscReturnLocked;
  }
  return mscReturnOk;
}
#ifdef __ICCARM__
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */


/***************************************************************************//**
 * @brief
 *   Perform a Flash data write phase.
 * @details
 *   This function performs the data phase of a Flash write operation by loading
 *   the given number of 32-bit words to the WDATA register.
 * @param[in] data
 *   Pointer to the first data word to load.
 * @param[in] numWords
 *   Number of data words (32-bit) to load.
 * @param[in] writeStrategy
 *   Write strategy to apply.
 * @return
 *   Returns the status of the data load operation
 * @verbatim
 *   mscReturnOk - Operation completed successfully.
 *   mscReturnTimeOut - Operation timed out waiting for flash operation
 *                      to complete.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#ifdef __ICCARM__
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
__ramfunc
#endif
__STATIC_INLINE MSC_Status_TypeDef MSC_LoadWriteData(uint32_t* data,
                                                     uint32_t numWords,
                                                     MSC_WriteStrategy_Typedef writeStrategy)
{
  uint32_t timeOut;
  uint32_t wordIndex;
  uint32_t wordsPerDataPhase;
  MSC_Status_TypeDef retval = mscReturnOk;

#if defined( _MSC_WRITECTRL_LPWRITE_MASK ) && defined( _MSC_WRITECTRL_WDOUBLE_MASK )
  /* If LPWRITE (Low Power Write) is NOT enabled, set WDOUBLE (Write Double word) */
  if (!(MSC->WRITECTRL & MSC_WRITECTRL_LPWRITE))
  {
    /* If the number of words to be written are odd, we need to align by writing
       a single word first, before setting the WDOUBLE bit. */
    if (numWords & 0x1)
    {
      /* Wait for the MSC to become ready for the next word. */
      timeOut = MSC_PROGRAM_TIMEOUT;
      while ((!(MSC->STATUS & MSC_STATUS_WDATAREADY)) && (timeOut != 0))
      {
        timeOut--;
      }
      /* Check for timeout */
      if (timeOut == 0)
      {
        return mscReturnTimeOut;
      }
      /* Clear double word option, in order to write the initial single word. */
      MSC->WRITECTRL &= ~MSC_WRITECTRL_WDOUBLE;
      /* Write first data word. */
      MSC->WDATA = *data++;
      MSC->WRITECMD = MSC_WRITECMD_WRITEONCE;

      /* Wait for the operation to finish. It may be required to change the WDOUBLE
         config after the initial write. It should not be changed while BUSY. */
      timeOut = MSC_PROGRAM_TIMEOUT;
      while((MSC->STATUS & MSC_STATUS_BUSY) && (timeOut != 0))
      {
        timeOut--;
      }
      /* Check for timeout */
      if (timeOut == 0)
      {
        return mscReturnTimeOut;
      }
      /* Subtract this initial odd word for the write loop below */
      numWords -= 1;
      retval = mscReturnOk;
    }
    /* Now we can set the double word option in order to write two words per
       data phase. */
    MSC->WRITECTRL |= MSC_WRITECTRL_WDOUBLE;
    wordsPerDataPhase = 2;
  }
  else
#endif /* defined( _MSC_WRITECTRL_LPWRITE_MASK ) && defined( _MSC_WRITECTRL_WDOUBLE_MASK ) */
  {
    wordsPerDataPhase = 1;
  }

  /* Write the rest as double word write if wordsPerDataPhase == 2 */
  if (numWords > 0)
  {
    /**** Write strategy: mscWriteIntSafe ****/
    if (writeStrategy == mscWriteIntSafe)
    {
      /* Requires a system core clock at 1MHz or higher */
      EFM_ASSERT(SystemCoreClock >= 1000000);
      wordIndex = 0;
      while(wordIndex < numWords)
      {
        MSC->WDATA = *data++;
        wordIndex++;
        if (wordsPerDataPhase == 2)
        {
          while (!(MSC->STATUS & MSC_STATUS_WDATAREADY));
          MSC->WDATA = *data++;
          wordIndex++;
        }
        MSC->WRITECMD = MSC_WRITECMD_WRITEONCE;

        /* Wait for the transaction to finish. */
        timeOut = MSC_PROGRAM_TIMEOUT;
        while ((MSC->STATUS & MSC_STATUS_BUSY) && (timeOut != 0))
        {
          timeOut--;
        }
        /* Check for timeout */
        if (timeOut == 0)
        {
          retval = mscReturnTimeOut;
          break;
        }
#if defined( _EFM32_GECKO_FAMILY )
        MSC->ADDRB += 4;
        MSC->WRITECMD = MSC_WRITECMD_LADDRIM;
#endif
      }
    }

    /**** Write strategy: mscWriteFast ****/
    else
    {
#if defined( _EFM32_GECKO_FAMILY )
      /* Gecko does not have auto-increment of ADDR. */
      EFM_ASSERT(0);
#else
      /* Requires a system core clock at 14MHz or higher */
      EFM_ASSERT(SystemCoreClock >= 14000000);

      wordIndex = 0;
      INT_Disable();
      while(wordIndex < numWords)
      {
        /* Wait for the MSC to be ready for the next word. */
        while (!(MSC->STATUS & MSC_STATUS_WDATAREADY))
        {
          /* If the write to MSC->WDATA below missed the 30us timeout and the
             following MSC_WRITECMD_WRITETRIG command arrived while
             MSC_STATUS_BUSY is 1, then the MSC_WRITECMD_WRITETRIG could be ignored by
             the MSC. In this case, MSC_STATUS_WORDTIMEOUT is set to 1
             and MSC_STATUS_BUSY is 0. A new trigger is therefore needed here to
             complete write of data in MSC->WDATA.
             If WDATAREADY became high since entry into this loop, exit and continue
             to the next WDATA write.
          */
          if ((MSC->STATUS & (MSC_STATUS_WORDTIMEOUT | MSC_STATUS_BUSY | MSC_STATUS_WDATAREADY)) == MSC_STATUS_WORDTIMEOUT)
          {
            MSC->WRITECMD = MSC_WRITECMD_WRITETRIG;
          }
        }
        MSC->WDATA = *data;
        if ((wordsPerDataPhase == 1) || ((wordsPerDataPhase == 2) && (wordIndex & 0x1)))
        {
          MSC->WRITECMD = MSC_WRITECMD_WRITETRIG;
        }
        data++;
        wordIndex++;
      }
      INT_Enable();

      /* Wait for the transaction to finish. */
      timeOut = MSC_PROGRAM_TIMEOUT;
      while ((MSC->STATUS & MSC_STATUS_BUSY) && (timeOut != 0))
      {
        timeOut--;
      }
      /* Check for timeout */
      if (timeOut == 0)
      {
        retval = mscReturnTimeOut;
      }
#endif
    } /* writeStrategy */
  }

#if defined( _MSC_WRITECTRL_WDOUBLE_MASK )
  /* Clear double word option, which should not be left on when returning. */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WDOUBLE;
#endif

  return retval;
}
#ifdef __ICCARM__
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */


/***************************************************************************//**
 * @brief
 *   Internal flash write function with select write strategy parameter
 * @param[in] address
 *   Write address
 * @param[in] data
 *   Pointer to the first data word to load.
 * @param[in] numWords
 *   Number of data words (32-bit) to load.
 * @param[in] writeStrategy
 *   Write strategy to apply.
 * @return
 *   Returns the status of the data load operation
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#ifdef __ICCARM__
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
#endif
static MSC_Status_TypeDef MSC_WriteWordI(uint32_t *address,
                                         void const *data,
                                         uint32_t numBytes,
                                         MSC_WriteStrategy_Typedef writeStrategy)
{
  uint32_t wordCount;
  uint32_t numWords;
  uint32_t pageWords;
  uint32_t* pData;
  MSC_Status_TypeDef retval = mscReturnOk;

  /* Check alignment (Must be aligned to words) */
  EFM_ASSERT(((uint32_t) address & 0x3) == 0);

  /* Check number of bytes. Must be divisable by four */
  EFM_ASSERT((numBytes & 0x3) == 0);

  /* Enable writing to the MSC */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  /* Convert bytes to words */
  numWords = numBytes >> 2;
  EFM_ASSERT(numWords > 0);

  /* The following loop splits the data into chunks corresponding to flash pages.
     The address is loaded only once per page, because the hardware automatically
     increments the address internally for each data load inside a page. */
  for (wordCount = 0, pData = (uint32_t *)data; wordCount < numWords; )
  {
    /* First we load address. The address is auto-incremented within a page.
       Therefore the address phase is only needed once for each page. */
    retval = MSC_LoadVerifyAddress(address + wordCount);
    if (mscReturnOk != retval)
    {
      return retval;
    }
    /* Compute the number of words to write to the current page. */
    pageWords =
      (FLASH_PAGE_SIZE -
       (((uint32_t) (address + wordCount)) & (FLASH_PAGE_SIZE - 1)))
      / sizeof(uint32_t);
    if (pageWords > numWords - wordCount)
    {
      pageWords = numWords - wordCount;
    }
    /* Now write the data in the current page. */
    retval = MSC_LoadWriteData(pData, pageWords, writeStrategy);
    if (mscReturnOk != retval)
    {
      break;
    }
    wordCount += pageWords;
    pData += pageWords;
  }

  /* Disable writing to the MSC */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;

#if defined( _MSC_WRITECTRL_WDOUBLE_MASK )
#if ( WORDS_PER_DATA_PHASE == 2 )
  /* Turn off double word write cycle support. */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WDOUBLE;
#endif
#endif

  return retval;
}
#ifdef __ICCARM__
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */

/** @endcond */


/***************************************************************************//**
 * @brief
 *   Erases a page in flash memory.
 * @note
 *   It is recommended to run this code from RAM. On the Gecko family, it is required
 *   to run this function from RAM.
 *
 *   For IAR, Rowley, SimplicityStudio, Atollic and armgcc this will be achieved
 *   automatically by using attributes in the function proctype. For Keil uVision you
 *   must define a section called "ram_code" and place this manually in your
 *   project's scatter file.
 *
 * @param[in] startAddress
 *   Pointer to the flash page to erase. Must be aligned to beginning of page
 *   boundary.
 * @return
 *   Returns the status of erase operation, #MSC_Status_TypeDef
 * @verbatim
 *   mscReturnOk - Operation completed successfully.
 *   mscReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   mscReturnLocked - Operation tried to erase a locked area of the flash.
 *   mscReturnTimeOut - Operation timed out waiting for flash operation
 *       to complete.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#ifdef __ICCARM__
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
#endif
MSC_Status_TypeDef MSC_ErasePage(uint32_t *startAddress)
{
  uint32_t timeOut = MSC_PROGRAM_TIMEOUT;

  /* Address must be aligned to pages */
  EFM_ASSERT((((uint32_t) startAddress) & (FLASH_PAGE_SIZE - 1)) == 0);

  /* Enable writing to the MSC */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  /* Load address */
  MSC->ADDRB    = (uint32_t)startAddress;
  MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

  /* Check for invalid address */
  if (MSC->STATUS & MSC_STATUS_INVADDR)
  {
    /* Disable writing to the MSC */
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return mscReturnInvalidAddr;
  }
  /* Check for write protected page */
  if (MSC->STATUS & MSC_STATUS_LOCKED)
  {
    /* Disable writing to the MSC */
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return mscReturnLocked;
  }
  /* Send erase page command */
  MSC->WRITECMD = MSC_WRITECMD_ERASEPAGE;

  /* Wait for the erase to complete */
  while ((MSC->STATUS & MSC_STATUS_BUSY) && (timeOut != 0))
  {
    timeOut--;
  }
  if (timeOut == 0)
  {
    /* Disable writing to the MSC */
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return mscReturnTimeOut;
  }
  /* Disable writing to the MSC */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
  return mscReturnOk;
}
#ifdef __ICCARM__
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */


/***************************************************************************//**
 * @brief
 *   Writes data to flash memory. This function is interrupt safe, but slower than
 *   MSC_WriteWordFast(), which writes to flash with interrupts disabled.
 *   Write data must be aligned to words and contain a number of bytes that is
 *   divisable by four.
 * @note
 *   It is recommended to erase the flash page before performing a write.
 *
 *   It is recommended to run this code from RAM. On the Gecko family, it is required
 *   to run this function from RAM.
 *
 *   For IAR, Rowley, SimplicityStudio, Atollic and armgcc this will be achieved
 *   automatically by using attributes in the function proctype. For Keil uVision you
 *   must define a section called "ram_code" and place this manually in your
 *   project's scatter file.
 *
 *   This function requires a ystem core clock at 1MHz or higher.
 *
 * @param[in] address
 *   Pointer to the flash word to write to. Must be aligned to words.
 * @param[in] data
 *   Data to write to flash.
 * @param[in] numBytes
 *   Number of bytes to write from flash. NB: Must be divisable by four.
 * @return
 *   Returns the status of the write operation
 * @verbatim
 *   flashReturnOk - Operation completed successfully.
 *   flashReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   flashReturnLocked - Operation tried to erase a locked area of the flash.
 *   flashReturnTimeOut - Operation timed out waiting for flash operation
 *       to complete. Or the MSC timed out waiting for the software to write
 *       the next word into the DWORD register.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#ifdef __ICCARM__
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
#endif
MSC_Status_TypeDef MSC_WriteWord(uint32_t *address,
                                  void const *data,
                                  uint32_t numBytes)
{
  return MSC_WriteWordI(address, data, numBytes, mscWriteIntSafe);
}
#ifdef __ICCARM__
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */


#if !defined( _EFM32_GECKO_FAMILY )
/***************************************************************************//**
 * @brief
 *   Writes data to flash memory. This function is faster than MSC_WriteWord(),
 *   but it disables interrupts. Write data must be aligned to words and contain
 *   a number of bytes that is divisable by four.
 * @note
 *   It is recommended to erase the flash page before performing a write.

 *   It is recommended to run this code from RAM. On the Gecko family, it is required
 *   to run this function from RAM.
 *
 *   For IAR, Rowley, SimplicityStudio, Atollic and armgcc this will be achieved
 *   automatically by using attributes in the function proctype. For Keil uVision you
 *   must define a section called "ram_code" and place this manually in your
 *   project's scatter file.
 *
 * @param[in] address
 *   Pointer to the flash word to write to. Must be aligned to words.
 * @param[in] data
 *   Data to write to flash.
 * @param[in] numBytes
 *   Number of bytes to write from flash. NB: Must be divisable by four.
 * @return
 *   Returns the status of the write operation
 * @verbatim
 *   flashReturnOk - Operation completed successfully.
 *   flashReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   flashReturnLocked - Operation tried to erase a locked area of the flash.
 *   flashReturnTimeOut - Operation timed out waiting for flash operation
 *       to complete. Or the MSC timed out waiting for the software to write
 *       the next word into the DWORD register.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
#ifdef __ICCARM__
/* Suppress warnings originating from use of EFM_ASSERT():              */
/* "Call to a non __ramfunc function from within a __ramfunc function"  */
/* "Possible rom access from within a __ramfunc function"               */
#pragma diag_suppress=Ta022
#pragma diag_suppress=Ta023
#endif
MSC_Status_TypeDef MSC_WriteWordFast(uint32_t *address,
                                  void const *data,
                                  uint32_t numBytes)
{
  return MSC_WriteWordI(address, data, numBytes, mscWriteFast);
}
#ifdef __ICCARM__
#pragma diag_default=Ta022
#pragma diag_default=Ta023
#endif
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */
#endif


#if defined( _MSC_MASSLOCK_MASK )
/***************************************************************************//**
 * @brief
 *   Erase entire flash in one operation
 * @note
 *   This command will erase the entire contents of the device.
 *   Use with care, both a debug session and all contents of the flash will be
 *   lost. The lock bit, MLW will prevent this operation from executing and
 *   might prevent successful mass erase.
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
MSC_Status_TypeDef MSC_MassErase(void)
{
  /* Enable writing to the MSC */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  /* Unlock device mass erase */
  MSC->MASSLOCK = MSC_MASSLOCK_LOCKKEY_UNLOCK;

  /* Erase first 512K block */
  MSC->WRITECMD = MSC_WRITECMD_ERASEMAIN0;

  /* Waiting for erase to complete */
  while ((MSC->STATUS & MSC_STATUS_BUSY));

#if (FLASH_SIZE >= (512 * 1024))
  /* Erase second 512K block */
  MSC->WRITECMD = MSC_WRITECMD_ERASEMAIN1;

  /* Waiting for erase to complete */
  while ((MSC->STATUS & MSC_STATUS_BUSY));
#endif

  /* Restore mass erase lock */
  MSC->MASSLOCK = MSC_MASSLOCK_LOCKKEY_LOCK;

  /* This will only successfully return if calling function is also in SRAM */
  return mscReturnOk;
}
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */
#endif

/** @} (end addtogroup MSC) */
/** @} (end addtogroup EM_Library) */
#endif /* defined(MSC_COUNT) && (MSC_COUNT > 0) */
