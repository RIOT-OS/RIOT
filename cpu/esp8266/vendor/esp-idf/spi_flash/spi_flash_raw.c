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

#include <string.h>

#include "spi_flash.h"
#include "priv/esp_spi_flash_raw.h"

#include "esp8266/eagle_soc.h"
#include "esp8266/pin_mux_register.h"
#include "esp8266/spi_register.h"

void Cache_Read_Disable_2(void)
{
    CLEAR_PERI_REG_MASK(CACHE_FLASH_CTRL_REG,CACHE_READ_EN_BIT);
    while(REG_READ(SPI_EXT2(0)) != 0) { }
    CLEAR_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL,SPI_ENABLE_AHB);
}

void Cache_Read_Enable_2(void)
{
    SET_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL,SPI_ENABLE_AHB);
    SET_PERI_REG_MASK(CACHE_FLASH_CTRL_REG,CACHE_READ_EN_BIT);
}
void Cache_Read_Enable_New(void) __attribute__((alias("Cache_Read_Enable_2")));

uint32_t spi_flash_get_id_raw(esp_spi_flash_chip_t *chip)
{
    uint32_t rdid = 0;

    Cache_Read_Disable();

    Wait_SPI_Idle(chip);

    WRITE_PERI_REG(PERIPHS_SPI_FLASH_C0, 0);    // clear regisrter
    WRITE_PERI_REG(PERIPHS_SPI_FLASH_CMD, SPI_FLASH_RDID);
    while(READ_PERI_REG(PERIPHS_SPI_FLASH_CMD)!=0);

    rdid = READ_PERI_REG(PERIPHS_SPI_FLASH_C0)&0xffffff;

    Cache_Read_Enable_New();

    return rdid;
}

esp_err_t spi_flash_read_status_raw(esp_spi_flash_chip_t *chip, uint32_t *status)
{
    esp_err_t ret;

    Cache_Read_Disable_2();

    ret =  SPI_read_status(chip, status);

    Cache_Read_Enable_2();

    return ret;
}

esp_err_t spi_flash_write_status_raw(esp_spi_flash_chip_t *chip, uint32_t status_value)
{
    Cache_Read_Disable_2();

    Wait_SPI_Idle(chip);

    if (ESP_OK != SPI_write_enable(chip))
        return ESP_ERR_FLASH_OP_FAIL;

    if(ESP_OK != SPI_write_status(chip, status_value))
        return ESP_ERR_FLASH_OP_FAIL;

    Wait_SPI_Idle(chip);

    Cache_Read_Enable_2();

    return ESP_OK;
}

esp_err_t spi_flash_erase_sector_raw(esp_spi_flash_chip_t *chip, size_t sec, size_t sec_size)
{
    esp_err_t ret = ESP_OK;

    Cache_Read_Disable_2();

    if (ESP_OK != SPI_write_enable(chip)) {
        ret  = ESP_ERR_FLASH_OP_FAIL;
    }

    if (ESP_OK != SPI_sector_erase(chip, sec * sec_size)) {
        ret = ESP_ERR_FLASH_OP_FAIL;
    }

    Cache_Read_Enable_2();

    return ret;
}

esp_err_t spi_flash_enable_qmode_raw(esp_spi_flash_chip_t *chip)
{
    esp_err_t ret;

    Cache_Read_Disable_2();

    ret = Enable_QMode(chip);

    Wait_SPI_Idle(chip);

    Cache_Read_Enable_2();

    return ret;
}

esp_err_t spi_flash_write_raw(esp_spi_flash_chip_t *chip, size_t dest_addr, const void *src, size_t size)
{
    esp_err_t ret;

    Cache_Read_Disable_2();

    ret = SPI_page_program(chip, dest_addr, (void *)src, size);

    Cache_Read_Enable_2();

    return ret;
}

esp_err_t spi_flash_read_raw(esp_spi_flash_chip_t *chip, size_t src_addr, void *dest, size_t size)
{
    esp_err_t ret;

    Cache_Read_Disable_2();

    ret = SPI_read_data(chip, src_addr, dest, size);

    Cache_Read_Enable_2();

    return ret;
}

bool spi_user_cmd_raw(esp_spi_flash_chip_t *chip, spi_cmd_dir_t mode, spi_cmd_t *p_cmd)
{
    int idx = 0;

    // Cache Disable
    Cache_Read_Disable_2();
    //wait spi idle
    if((mode & SPI_RAW) == 0) {
        Wait_SPI_Idle(chip);
    }
    //save reg
    uint32_t io_mux_reg = READ_PERI_REG(PERIPHS_IO_MUX_CONF_U);
    uint32_t spi_clk_reg = READ_PERI_REG(SPI_CLOCK(SPI));
    uint32_t spi_ctrl_reg = READ_PERI_REG(SPI_CTRL(SPI));
    uint32_t spi_user_reg = READ_PERI_REG(SPI_USER(SPI));

    if (mode & SPI_WRSR) {
        // enable write register
        SPI_write_enable(chip);
    }

    SET_PERI_REG_MASK(SPI_USER(SPI),SPI_USR_COMMAND);

    //Disable flash operation mode
    CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_FLASH_MODE);
    //SET SPI SEND BUFFER MODE
    CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_MISO_HIGHPART);
    //CLEAR EQU SYS CLK
    CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX_CONF_U,SPI0_CLK_EQU_SYS_CLK);

    SET_PERI_REG_MASK(SPI_USER(SPI), SPI_CS_SETUP|SPI_CS_HOLD|SPI_USR_COMMAND|SPI_USR_MOSI);
    CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_FLASH_MODE);
    //CLEAR DAUL OR QUAD LINES TRANSMISSION MODE
    CLEAR_PERI_REG_MASK(SPI_CTRL(SPI), SPI_QIO_MODE|SPI_DIO_MODE|SPI_DOUT_MODE|SPI_QOUT_MODE);
    WRITE_PERI_REG(SPI_CLOCK(SPI),
                   ((3&SPI_CLKCNT_N)<<SPI_CLKCNT_N_S)|
                   ((1&SPI_CLKCNT_H)<<SPI_CLKCNT_H_S)|
                   ((3&SPI_CLKCNT_L)<<SPI_CLKCNT_L_S)); //clear bit 31,set SPI clock div
    //Enable fast read mode
    SET_PERI_REG_MASK(SPI_CTRL(SPI), SPI_FASTRD_MODE);

    //WAIT COMMAND DONE
    while(READ_PERI_REG(SPI_CMD(SPI)) & SPI_USR);

    //SET USER CMD
    if (p_cmd->cmd_len != 0) {
        //Max CMD length is 16 bits
        SET_PERI_REG_BITS(SPI_USER2(SPI), SPI_USR_COMMAND_BITLEN, p_cmd->cmd_len * 8 - 1, SPI_USR_COMMAND_BITLEN_S);
        //Enable CMD
        SET_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_COMMAND);
        //LOAD CMD
        SET_PERI_REG_BITS(SPI_USER2(SPI), SPI_USR_COMMAND_VALUE, p_cmd->cmd, SPI_USR_COMMAND_VALUE_S);
    } else {
        //CLEAR CMD
        CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_COMMAND);
        SET_PERI_REG_BITS(SPI_USER2(SPI), SPI_USR_COMMAND_BITLEN, 0, SPI_USR_COMMAND_BITLEN_S);
    }
    if (p_cmd->dummy_bits != 0) {
        //SET dummy bits
        SET_PERI_REG_BITS(SPI_USER1(SPI), SPI_USR_DUMMY_CYCLELEN, p_cmd->dummy_bits - 1, SPI_USR_DUMMY_CYCLELEN_S);
        //Enable dummy
        SET_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_DUMMY);
    } else {
        //CLEAR DUMMY
        SET_PERI_REG_BITS(SPI_USER1(SPI), SPI_USR_DUMMY_CYCLELEN, 0, SPI_USR_DUMMY_CYCLELEN_S);
        CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_DUMMY);
    }

    //SET USER ADDRESS
    if (p_cmd->addr_len != 0) {
        //Set addr lenght
        SET_PERI_REG_BITS(SPI_USER1(SPI), SPI_USR_ADDR_BITLEN, p_cmd->addr_len * 8 - 1, SPI_USR_ADDR_BITLEN_S);
        //Enable user address
        SET_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_ADDR);
        WRITE_PERI_REG(SPI_ADDR(SPI), *p_cmd->addr);
    } else {
        //CLEAR ADDR
        CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_ADDR);
        SET_PERI_REG_BITS(SPI_USER1(SPI), SPI_USR_ADDR_BITLEN, 0, SPI_USR_ADDR_BITLEN_S);
    }

    uint32_t *value = p_cmd->data;
    if (((mode & SPI_TX) || (mode & SPI_WRSR)) && p_cmd->data_len != 0) {
        //Enable MOSI, disable MISO
        SET_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_MOSI);
        CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_MISO);
        do {
            WRITE_PERI_REG((SPI_W0(SPI) + (idx << 2)), *value++);
        } while ( ++idx < (p_cmd->data_len / 4));
        SET_PERI_REG_BITS(SPI_USER1(SPI), SPI_USR_MOSI_BITLEN, ((p_cmd->data_len) * 8 - 1), SPI_USR_MOSI_BITLEN_S);

    } else if ((mode & SPI_RX) && p_cmd->data_len != 0) {
        //Enable MISO, disable MOSI
        CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_MOSI);
        SET_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_MISO);
        SET_PERI_REG_BITS(SPI_USER1(SPI),SPI_USR_MISO_BITLEN, p_cmd->data_len * 8 - 1, SPI_USR_MISO_BITLEN_S);
        int fifo_idx = 0;
        do {
            WRITE_PERI_REG((SPI_W0(SPI) + (fifo_idx << 2)), 0);
        } while ( ++fifo_idx < (p_cmd->data_len / 4));
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_MOSI);
        CLEAR_PERI_REG_MASK(SPI_USER(SPI), SPI_USR_MISO);
        SET_PERI_REG_BITS(SPI_USER1(SPI), SPI_USR_MISO_BITLEN, 0, SPI_USR_MISO_BITLEN_S);
        SET_PERI_REG_BITS(SPI_USER1(SPI), SPI_USR_MOSI_BITLEN, 0, SPI_USR_MOSI_BITLEN_S);
    }

    //Start command
    SET_PERI_REG_MASK(SPI_CMD(SPI), SPI_USR);
    while (READ_PERI_REG(SPI_CMD(SPI)) & SPI_USR);

    if (mode & SPI_RX) {
        do {
            *p_cmd->data ++ = READ_PERI_REG(SPI_W0(SPI) + (idx << 2));
        } while (++idx < (p_cmd->data_len / 4));
    }

    //recover
    WRITE_PERI_REG(PERIPHS_IO_MUX_CONF_U,io_mux_reg);
    WRITE_PERI_REG(SPI_CTRL(SPI),spi_ctrl_reg);
    WRITE_PERI_REG(SPI_CLOCK(SPI),spi_clk_reg);
    WRITE_PERI_REG(SPI_USER(SPI),spi_user_reg);

    if((mode & SPI_RAW) == 0) {
        Wait_SPI_Idle(chip);
    }
    //enable icache
    Cache_Read_Enable_2();

    return true;
}

void spi_flash_switch_to_qio_raw(void)
{
    CLEAR_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, SPI_QIO_MODE
                        |SPI_QOUT_MODE
                        |SPI_DIO_MODE
                        |SPI_DOUT_MODE
                        |SPI_FASTRD_MODE);

    SET_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, SPI_QIO_MODE | SPI_FASTRD_MODE);
}
