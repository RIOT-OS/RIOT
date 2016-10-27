/*
 * Copyright (C) 2016 Unwired Devices <info@unwds.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdint.h>
#include <stddef.h>

#include "stm32l1xx.h"

#include "nvram.h"
#include "assert.h"

#include "l1-nvram-eeprom.h"

/**
 * @ingroup     nvram
 * @{
 *
 * @file	l1-nvram-eeprom.c
 *
 * @brief       STM32L1 Data EEPROM driver
 *
 * Tested on:
 * - STM32L151CCU6
 *
 * @author      EP <ep@unwds.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  FLASH_BUSY = 1,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
} l1_flash_status_t;

/**
  * @brief  Unlocks the data memory and FLASH_PECR register access.
  * @param  None
  * @return None
  */
static void eeprom_unlock(void)
{
  if((FLASH->PECR & FLASH_PECR_PELOCK) != RESET)
  {
    /* Unlocking the Data memory and FLASH_PECR register access*/
    FLASH->PEKEYR = FLASH_PEKEY1;
    FLASH->PEKEYR = FLASH_PEKEY2;
  }
}

/**
  * @brief  Locks the Data memory and FLASH_PECR register access.
  * @param  None
  * @return None
  */
static void eeprom_lock(void)
{
  /* Set the PELOCK Bit to lock the data memory and FLASH_PECR register access */
  FLASH->PECR |= FLASH_PECR_PELOCK;
}

/**
 * @brief Returns the current FLASH device status
 */
static l1_flash_status_t get_status(void)
{
  l1_flash_status_t status = FLASH_COMPLETE;

  if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY)
  {
    status = FLASH_BUSY;
  }
  else
  {
    if((FLASH->SR & (uint32_t)FLASH_FLAG_WRPERR)!= (uint32_t)0x00)
    {
      status = FLASH_ERROR_WRP;
    }
    else
    {
      if((FLASH->SR & (uint32_t)0x1E00) != (uint32_t)0x00)
      {
        status = FLASH_ERROR_PROGRAM;
      }
      else
      {
        status = FLASH_COMPLETE;
      }
    }
  }

  /* Return the FLASH Status */
  return status;
}

/**
 * @brief Busy-waits within a timeout if the FLASH peripheral is performing an operation
 *
 * @param[in] timeout timeout value in cycles
 * @return current status of FLASH device
 */
static l1_flash_status_t flash_wait_for_last_operation(uint32_t timeout) {
	l1_flash_status_t status = FLASH_COMPLETE;

	/* Check for the FLASH Status */
	status = get_status();

	/* Wait for a FLASH operation to complete or a TIMEOUT to occur */
	while((status == FLASH_BUSY) && (timeout != 0x00))
	{
		status = get_status();
		timeout--;
	}

	if(timeout == 0x00)
	{
		status = FLASH_TIMEOUT;
	}

	/* Return the operation status */
	return status;
}

/**
  * @brief  Write a Byte at a specified address in data EEPROM.
  * @param  address: specifies the address to be written.
  * @param  data: specifies the data to be written.
  * @retval FLASH Status: The returned value can be:
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
static l1_flash_status_t program_byte(uint32_t address, uint8_t data)
{
	l1_flash_status_t status = FLASH_COMPLETE;
#if !defined (STM32L1XX_HD) && !defined (STM32L1XX_MDP) && !defined (STM32L1XX_XL)
  uint32_t tmp = 0, tmpaddr = 0;
#endif

  /* Check the parameters */
  assert(IS_FLASH_DATA_ADDRESS(address));

  /* Wait for last operation to be completed */
  status = flash_wait_for_last_operation(FLASH_ER_PRG_TIMEOUT);

  if(status == FLASH_COMPLETE)
  {
#if !defined (STM32L1XX_HD) && !defined (STM32L1XX_MDP) && !defined (STM32L1XX_XL)
    if(data != (uint8_t) 0x00)
    {
      *(__IO uint8_t *)address = data;

      /* Wait for last operation to be completed */
      status = flash_wait_for_last_operation(FLASH_ER_PRG_TIMEOUT);
    }
    else
    {
      tmpaddr = address & 0xFFFFFFFC;
      tmp = * (__IO uint32_t *) tmpaddr;
      tmpaddr = 0xFF << ((uint32_t) (0x8 * (Address & 0x3)));
      tmp &= ~tmpaddr;
      status = DATA_EEPROM_EraseWord(address & 0xFFFFFFFC);
      status = DATA_EEPROM_FastProgramWord((address & 0xFFFFFFFC), tmp);
    }
#elif defined (STM32L1XX_HD) || defined (STM32L1XX_MDP) || defined (STM32L1XX_XL)
    *(__IO uint8_t *)address = data;

    /* Wait for last operation to be completed */
    status = flash_wait_for_last_operation(FLASH_ER_PRG_TIMEOUT);
#endif
  }

  /* Return the Write Status */
  return status;
}

/**
 * @brief Copy data from system memory to NVRAM.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[in]  src   Pointer to the first byte in the system memory address space
 * @param[in]  dst   Starting address in the NVRAM device address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return           Number of bytes written on success
 * @return           <0 on errors
 */
static int nvram_write(nvram_t *dev, const uint8_t *src, uint32_t dst, size_t len);

/**
 * @brief Copy data from NVRAM to system memory.
 *
 * @param[in]  dev   Pointer to NVRAM device descriptor
 * @param[out] dst   Pointer to the first byte in the system memory address space
 * @param[in]  src   Starting address in the NVRAM device address space
 * @param[in]  len   Number of bytes to copy
 *
 * @return           Number of bytes read on success
 * @return           <0 on errors
 */
static int nvram_read(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len);

int nvram_l1_eeprom_init(nvram_t *dev) {
	dev->write = nvram_write;
	dev->read = nvram_read;

	return 0;
}

static int nvram_read(nvram_t *dev, uint8_t *dst, uint32_t src, size_t len) {
	uint32_t eeprom_addr = EEPROM_BASE + src;
	uint32_t i = 0;
	for (i = 0; i < len; i++) {
		/* Read byte from EEPROM memory */
		dst[i] = *((uint8_t *) (eeprom_addr + i));
	}

	return i;
}

static int nvram_write(nvram_t *dev, const uint8_t *src, uint32_t dst, size_t len) {
	eeprom_unlock();

	uint32_t eeprom_addr = EEPROM_BASE + dst;
	uint32_t i = 0;
	for (i = 0; i < len; i++) {
		/* Program byte */
		program_byte(eeprom_addr + i, src[i]);
	}

	eeprom_lock();

	return i;
}

#ifdef __cplusplus
}
#endif
