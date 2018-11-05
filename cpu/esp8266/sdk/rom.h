/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266_sdk
 * @{
 *
 * @file
 * @brief       ESP8266 other ROM function prototypes
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef ROM_H
#define ROM_H

#ifndef DOXYGEN

#include <stdint.h>
#include <stdarg.h>

#include "c_types.h"
#include "spi_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MODULE_ESP_SDK
/*
 * The following functions are mappings or dummies for source code
 * compatibility of SDK and NON-SDK version
 */

extern uint8_t rom_i2c_readReg(uint32_t block, uint32_t host_id, uint32_t reg_add);
extern uint8_t rom_i2c_readReg_Mask(uint32_t block, uint32_t host_id,
                                    uint32_t reg_add,uint32_t Msb, uint32_t Lsb);
extern void rom_i2c_writeReg (uint32_t block, uint32_t host_id,
                              uint32_t reg_add, uint32_t data);
extern void rom_i2c_writeReg_Mask(uint32_t block, uint32_t host_id,
                                  uint32_t reg_add, uint32_t Msb, uint32_t Lsb,
                                  uint32_t indata);

extern uint32_t rtc_get_reset_reason(void);

#endif /* MODULE_ESP_SDK */

/* pointer to flash chip data structure */
extern SpiFlashChip* flashchip;

extern SpiFlashOpResult spi_flash_attach(void);

void Cache_Read_Disable(void);
void Cache_Read_Enable (uint32_t odd_even, uint32_t mb_count, uint32_t no_idea);

SpiFlashOpResult Wait_SPI_Idle(SpiFlashChip *chip);
SpiFlashOpResult SPI_write_enable(SpiFlashChip *chip);

SpiFlashOpResult SPIEraseArea  (uint32_t off, size_t len);
SpiFlashOpResult SPIEraseBlock (uint32_t num);
SpiFlashOpResult SPIEraseSector(uint32_t num);
SpiFlashOpResult SPIEraseChip  (void);

SpiFlashOpResult SPILock  (void);
SpiFlashOpResult SPIUnlock(void);

SpiFlashOpResult SPIRead  (uint32_t off, uint32_t *dst, size_t size);
SpiFlashOpResult SPIWrite (uint32_t off, const uint32_t *src, size_t size);

int SPIReadModeCnfig (uint32_t);

/* set elements of flashchip, see struct SpiFlashChip; */
SpiFlashOpResult SPIParamCfg (uint32_t deviceId,
                              uint32_t chip_size,
                              uint32_t block_size,
                              uint32_t sector_size,
                              uint32_t page_size,
                              uint32_t status_mask);

extern void uartAttach (void);
extern void    uart_div_modify(uint8 uart_no, uint32_t DivLatchValue);
extern void Uart_Init (uint8 uart_no);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* ROM_H */
