#ifndef _ROM_FUNCTIONS_H
#define _ROM_FUNCTIONS_H

#include <stdint.h>
#include <stdarg.h>

#define ROM_FLASH_BUF_DECLARE(__name, __size) uint8_t __name[__size] __attribute__((aligned(4)))

typedef struct esp_spi_flash_chip {
    uint32_t  deviceId;
    uint32_t  chip_size;    // chip size in byte
    uint32_t  block_size;
    uint32_t  sector_size;
    uint32_t  page_size;
    uint32_t  status_mask;
} esp_spi_flash_chip_t;

extern esp_spi_flash_chip_t flashchip;

uint32_t Wait_SPI_Idle(esp_spi_flash_chip_t *chip);

void uart_div_modify(uint32_t uart_no, uint32_t baud_div);

int ets_io_vprintf(int (*putc)(int), const char* fmt, va_list ap);

void system_soft_wdt_feed(void);

void Cache_Read_Enable_New(void);

int SPI_page_program(esp_spi_flash_chip_t *chip,  uint32_t dst_addr, void *pbuf, uint32_t len);
int SPI_read_data(esp_spi_flash_chip_t *chip,  uint32_t dst_addr, void *pbuf, uint32_t len);
int SPI_write_enable(esp_spi_flash_chip_t *chip);
int SPI_sector_erase(esp_spi_flash_chip_t *chip, uint32_t sect_addr);
int SPI_write_status(esp_spi_flash_chip_t *chip, uint32_t status);
int SPI_read_status(esp_spi_flash_chip_t *chip, uint32_t *status);
int Enable_QMode(esp_spi_flash_chip_t *chip);

int SPIWrite(uint32_t addr, const uint8_t *src, uint32_t size);
int SPIRead(uint32_t addr, void *dst, uint32_t size);
int SPIEraseSector(uint32_t sector_num);

void Cache_Read_Disable(void);
void Cache_Read_Enable(uint8_t map, uint8_t p, uint8_t v);

#endif
