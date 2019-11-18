// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sdkconfig.h"

#ifndef CONFIG_BOOTLOADER_INIT_SPI_FLASH
#include "spi_flash.h"

/*
 * @brief initialize the chip
 */
void chip_boot(void)
{
    extern void esp_spi_flash_init(uint32_t spi_speed, uint32_t spi_mode);

    esp_spi_flash_init(CONFIG_SPI_FLASH_FREQ, CONFIG_SPI_FLASH_MODE);
}
#endif /* CONFIG_BOOTLOADER_INIT_SPI_FLASH */
