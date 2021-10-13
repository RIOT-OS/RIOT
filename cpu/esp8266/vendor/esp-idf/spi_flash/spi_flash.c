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
#include "esp8266/spi_register.h"
#include "esp8266/pin_mux_register.h"
#include "priv/esp_spi_flash_raw.h"

#include "esp_attr.h"
#include "esp_wifi_osi.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "esp_image_format.h"

#define SPI_FLASH_ISSI_ENABLE_QIO_MODE          (BIT(6))

/*gd25q32c*/
#define SPI_FLASH_GD25Q32C_WRITE_STATUSE1_CMD   (0X01)
#define SPI_FLASH_GD25Q32C_WRITE_STATUSE2_CMD   (0X31)
#define SPI_FLASH_GD25Q32C_WRITE_STATUSE3_CMD   (0X11)

#define SPI_FLASH_GD25Q32C_READ_STATUSE1_CMD    (0X05)
#define SPI_FLASH_GD25Q32C_READ_STATUSE2_CMD    (0X35)
#define SPI_FLASH_GD25Q32C_READ_STATUSE3_CMD    (0X15)

#define SPI_FLASH_GD25Q32C_QIO_MODE             (BIT(1))

#define SPI_ISSI_FLASH_WRITE_PROTECT_STATUS     (BIT(2)|BIT(3)|BIT(4)|BIT(5))
#define SPI_EON_25Q16A_WRITE_PROTECT_STATUS     (BIT(2)|BIT(3)|BIT(4)|BIT(5))
#define SPI_EON_25Q16B_WRITE_PROTECT_STATUS     (BIT(2)|BIT(3)|BIT(4)|BIT(5))
#define SPI_GD25Q32_FLASH_WRITE_PROTECT_STATUS  (BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6))
#define SPI_FLASH_RDSR2      0x35
#define SPI_FLASH_PROTECT_STATUS                (BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(14))

#ifndef BOOTLOADER_BUILD
#define FLASH_INTR_DECLARE(t)
#define FLASH_INTR_LOCK(t)                      vPortEnterCritical()
#define FLASH_INTR_UNLOCK(t)                    vPortExitCritical()
#else
#define FLASH_INTR_DECLARE(t)
#define FLASH_INTR_LOCK(t)
#define FLASH_INTR_UNLOCK(t)
#endif

#define FLASH_ALIGN_BYTES                       4
#define FLASH_ALIGN(addr)                       ((((size_t)addr) + (FLASH_ALIGN_BYTES - 1)) & (~(FLASH_ALIGN_BYTES - 1)))
#define FLASH_ALIGN_BEFORE(addr)                (FLASH_ALIGN(addr) - 4)
#define NOT_ALIGN(addr)                         (((size_t)addr) & (FLASH_ALIGN_BYTES - 1))
#define IS_ALIGN(addr)                          (NOT_ALIGN(addr) == 0)

extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);

esp_spi_flash_chip_t flashchip = {
    0x1640ef,
    CONFIG_SPI_FLASH_SIZE,
    64 * 1024,
    4 * 1024,
    256,
    0xffff
};

uint8_t FlashIsOnGoing = 0;

bool spi_user_cmd(spi_cmd_dir_t mode, spi_cmd_t *p_cmd)
{
    bool ret;
    FLASH_INTR_DECLARE(c_tmp);

    if ((p_cmd->addr_len != 0 && p_cmd->addr == NULL)
            || (p_cmd->data_len != 0 && p_cmd->data == NULL)
            || (p_cmd == NULL)) {
        return false;
    }

    FLASH_INTR_LOCK(c_tmp);
    FlashIsOnGoing = 1;

    ret = spi_user_cmd_raw(&flashchip, mode, p_cmd);

    FlashIsOnGoing = 0;
    FLASH_INTR_UNLOCK(c_tmp);

    return ret;
}

bool special_flash_read_status(uint8_t command, uint32_t* status, int len)
{
    bool ret;
    spi_cmd_t cmd;

    if( len > 2 || len < 1) {
        return false;
    }

    cmd.cmd = command;
    cmd.cmd_len = 1;
    cmd.addr = NULL;
    cmd.addr_len = 0;
    cmd.dummy_bits = 0;
    cmd.data = status;
    cmd.data_len = len;

    ret = spi_user_cmd(SPI_RX, &cmd);

    return ret;
}

bool special_flash_write_status(uint8_t command, uint32_t status, int len, bool write_en)
{
    bool ret;
    spi_cmd_t cmd;

    if (len > 2 || len < 1) {
        return false;
    }

    cmd.cmd = command;
    cmd.cmd_len = 1;
    cmd.addr = NULL;
    cmd.addr_len = 0;
    cmd.dummy_bits = 0;
    cmd.data = &status;
    cmd.data_len = len > 1 ? 2 : 1;
    if (write_en) {
        ret = spi_user_cmd(SPI_WRSR, &cmd);
    } else {
        ret = spi_user_cmd(SPI_TX | SPI_RAW, &cmd);
    }

    return ret;
}

uint8_t en25q16x_read_sfdp(void)
{
    spi_cmd_t cmd;
    uint32_t addr = 0x00003000;
    uint32_t data = 0;

    cmd.cmd = 0x5a;
    cmd.cmd_len = 1;

    cmd.addr = &addr;
    cmd.addr_len = 3;
    cmd.dummy_bits = 8;

    cmd.data = &data;
    cmd.data_len = 1;

    spi_user_cmd(SPI_RX, &cmd);

    return ((uint8_t) data);
}

uint32_t spi_flash_get_id(void)
{
    uint32_t rdid = 0;
    FLASH_INTR_DECLARE(c_tmp);

    FLASH_INTR_LOCK(c_tmp);

    FlashIsOnGoing = 1;

    rdid = spi_flash_get_id_raw(&flashchip);

    FlashIsOnGoing = 0;

    FLASH_INTR_UNLOCK(c_tmp);

    return rdid;
}

esp_err_t spi_flash_read_status(uint32_t *status)
{
    esp_err_t ret;
    FLASH_INTR_DECLARE(c_tmp);

    FLASH_INTR_LOCK(c_tmp);

    FlashIsOnGoing = 1;

    ret = spi_flash_read_status_raw(&flashchip, status);

    FlashIsOnGoing = 0;

    FLASH_INTR_UNLOCK(c_tmp);

    return ret;
}

esp_err_t spi_flash_write_status(uint32_t status_value)
{
    FLASH_INTR_DECLARE(c_tmp);

    FLASH_INTR_LOCK(c_tmp);

    FlashIsOnGoing = 1;

    spi_flash_write_status_raw(&flashchip, status_value);

    FlashIsOnGoing = 0;

    FLASH_INTR_UNLOCK(c_tmp);

    return ESP_OK;
}

bool spi_flash_check_wr_protect(void)
{
    uint32_t flash_id=spi_flash_get_id();
    uint32_t status=0;
    //check for EN25Q16A/B flash chips
    if ((flash_id & 0xffffff) == 0x15701c) {
        uint8_t sfdp = en25q16x_read_sfdp();
        if (sfdp == 0xE5) {
            //This is EN25Q16A, set bit6 in the same way as issi flash chips.
            if(spi_flash_read_status(&status)==0) { //Read status Ok
                if(status&(SPI_EON_25Q16A_WRITE_PROTECT_STATUS)) { //Write_protect
                    special_flash_write_status(0x1, status&(~(SPI_EON_25Q16A_WRITE_PROTECT_STATUS)), 1, true);
                }
            }
        } else if (sfdp == 0xED) {
            //This is EN25Q16B
            if(spi_flash_read_status(&status)==0) { //Read status Ok
                if(status&(SPI_EON_25Q16B_WRITE_PROTECT_STATUS)) { //Write_protect
                    special_flash_write_status(0x1, status&(~(SPI_EON_25Q16B_WRITE_PROTECT_STATUS)), 1, true);
                }
            }
        }
    }
    //MXIC :0XC2
    //ISSI :0X9D
    // ets_printf("spi_flash_check_wr_protect\r\n");
    else if(((flash_id&0xFF)==0X9D)||((flash_id&0xFF)==0XC2)||((flash_id & 0xFF) == 0x1C)) {
        if(spi_flash_read_status(&status)==0) { //Read status Ok
            if(status&(SPI_ISSI_FLASH_WRITE_PROTECT_STATUS)) { //Write_protect
                special_flash_write_status(0x1, status&(~(SPI_ISSI_FLASH_WRITE_PROTECT_STATUS)), 1, true);
            }
        }
    }
    //GD25Q32C:0X16409D
    //GD25Q128
    else if(((flash_id&0xFFFFFFFF)==0X1640C8)||((flash_id&0xFFFFFFFF)==0X1840C8)) {
        if(spi_flash_read_status(&status)==0) { //Read status Ok
            if(status&SPI_GD25Q32_FLASH_WRITE_PROTECT_STATUS) {
                special_flash_write_status(0x01, status&(~(SPI_GD25Q32_FLASH_WRITE_PROTECT_STATUS)), 1, true);
            }
        }
    }
    //Others
    else {
        if(spi_flash_read_status(&status)==0) { //Read status Ok
            uint32_t status1 = 0; //flash_gd25q32c_read_status(GD25Q32C_STATUS2);
            special_flash_read_status(SPI_FLASH_RDSR2, &status1, 1);
            status=(status1 << 8)|(status & 0xff);
            if(status&SPI_FLASH_PROTECT_STATUS) {
                status=((status&(~SPI_FLASH_PROTECT_STATUS))&0xffff);
                spi_flash_write_status(status);
            }
        }
    }
    return true;
}

static void spi_flash_enable_qio_bit6(void)
{
    uint8_t wrsr_cmd = 0x1;
    uint32_t issi_qio = SPI_FLASH_ISSI_ENABLE_QIO_MODE;
    special_flash_write_status(wrsr_cmd, issi_qio, 1, true);
}

static bool spi_flash_issi_enable_QIO_mode(void)
{
    uint32_t status = 0;
    if(spi_flash_read_status(&status) == 0) {
        if((status&SPI_FLASH_ISSI_ENABLE_QIO_MODE)) {
            return true;
        }
    }
    else {
        return false;
    }

    spi_flash_enable_qio_bit6();

    if(spi_flash_read_status(&status) == 0) {
        if((status&SPI_FLASH_ISSI_ENABLE_QIO_MODE)) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

static uint8_t flash_gd25q32c_read_status(enum GD25Q32C_status status_index)
{
    uint8_t rdsr_cmd=0;
    if(GD25Q32C_STATUS1 == status_index) {
        rdsr_cmd = SPI_FLASH_GD25Q32C_READ_STATUSE1_CMD;
    }
    else if(GD25Q32C_STATUS2 == status_index) {
        rdsr_cmd = SPI_FLASH_GD25Q32C_READ_STATUSE2_CMD;
    }
    else if(GD25Q32C_STATUS3 == status_index) {
        rdsr_cmd = SPI_FLASH_GD25Q32C_READ_STATUSE3_CMD;
    }
    else {

    }
    uint32_t status;
    special_flash_read_status(rdsr_cmd, &status, 1);
    return ((uint8_t)status);
}

static void flash_gd25q32c_write_status(enum GD25Q32C_status status_index,uint8_t status)
{
    uint32_t wrsr_cmd=0;
    uint32_t new_status = status;
    if(GD25Q32C_STATUS1 == status_index) {
        wrsr_cmd = SPI_FLASH_GD25Q32C_WRITE_STATUSE1_CMD;
    }
    else if(GD25Q32C_STATUS2 == status_index) {
        wrsr_cmd = SPI_FLASH_GD25Q32C_WRITE_STATUSE2_CMD;
    }
    else if(GD25Q32C_STATUS3 == status_index) {
        wrsr_cmd = SPI_FLASH_GD25Q32C_WRITE_STATUSE3_CMD;
    }
    else {
        //ets_printf("[ERR]Not konw GD25Q32C status idx %d\n ",spi_wr_status_cmd);
    }
    special_flash_write_status(wrsr_cmd, new_status, 1, true);
}

static bool flash_gd25q32c_enable_QIO_mode(void)
{
    uint8_t data = 0;
    if((data=flash_gd25q32c_read_status(GD25Q32C_STATUS2))&SPI_FLASH_GD25Q32C_QIO_MODE) {
        return true;
    }
    else {
        flash_gd25q32c_write_status(GD25Q32C_STATUS2,SPI_FLASH_GD25Q32C_QIO_MODE);
        if(flash_gd25q32c_read_status(GD25Q32C_STATUS2)&SPI_FLASH_GD25Q32C_QIO_MODE) {
            return true;
        }
        else {
            return false;
        }
    }
}

void special_flash_set_mode(uint8_t command, bool disable_wait_idle)
{
    spi_cmd_t cmd;
    cmd.cmd = command;
    cmd.cmd_len = 1;
    cmd.addr = NULL;
    cmd.addr_len = 0;
    cmd.dummy_bits = 0;
    cmd.data = NULL;
    cmd.data_len = 0;
    if (disable_wait_idle) {
        spi_user_cmd(SPI_TX | SPI_RAW, &cmd);
    } else {
        spi_user_cmd(SPI_TX, &cmd);
    }
}

bool en25q16x_write_volatile_status(uint8_t vsr)
{
    //enter OTP mode
    special_flash_set_mode(0x3a, true);
    //volatile status register write enable
    special_flash_set_mode(0x50, true);
    //send 0x01 + 0x40 to set WHDIS bit
    special_flash_write_status(0x01, vsr, 1, false);
    //check
    uint32_t status = 0;
    special_flash_read_status(0x05, &status, 1);
    //Leave OTP mode
    special_flash_set_mode(0x04, false);

    if (status == 0x40) {
        return true;
    } else {
        return false;
    }
}

void user_spi_flash_dio_to_qio_pre_init(void)
{
    uint32_t flash_id = spi_flash_get_id();
    bool to_qio = false;
    //check for EN25Q16A/B flash chips
    if ((flash_id & 0xffffff) == 0x15701c) {
        uint8_t sfdp = en25q16x_read_sfdp();
        if (sfdp == 0xE5) {
            //This is EN25Q16A, set bit6 in the same way as issi flash chips.
            if (spi_flash_issi_enable_QIO_mode() == true) {
                to_qio = true;
            }
        } else if (sfdp == 0xED) {
            //This is EN25Q16B
            if (en25q16x_write_volatile_status(0x40) == true) {
                to_qio = true;
            }
        }
    }
    // ISSI : 0x9D
    // MXIC : 0xC2
    // GD25Q32C & GD25Q128C : 0x1640C8
    // EON : 0X1C
    // ENABLE FLASH QIO 0X01H+BIT6
    else if (((flash_id & 0xFF) == 0x9D) || ((flash_id & 0xFF) == 0xC2) || ((flash_id & 0xFF) == 0x1C) ) {
        if (spi_flash_issi_enable_QIO_mode() == true) {
            to_qio = true;
        }
        //ENABLE FLASH QIO 0X31H+BIT2
    } else if (((flash_id & 0xFFFFFF) == 0x1640C8) || ((flash_id & 0xFFFFFF) == 0x1840C8)) {
        if (flash_gd25q32c_enable_QIO_mode() == true) {
            to_qio = true;
        }
        //ENBALE FLASH QIO 0X01H+0X00+0X02
    } else {
        if (spi_flash_enable_qmode_raw(&flashchip) == ESP_OK) {
            to_qio = true;
        }
    }

    if (to_qio == true) {
        spi_flash_switch_to_qio_raw();
    }
}

esp_err_t spi_flash_erase_sector(size_t sec)
{
    FLASH_INTR_DECLARE(c_tmp);

    esp_err_t ret;

    if (sec >= (flashchip.chip_size / flashchip.sector_size)) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    if (spi_flash_check_wr_protect() == false) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    FLASH_INTR_LOCK(c_tmp);
    FlashIsOnGoing = 1;
    
    ret = spi_flash_erase_sector_raw(&flashchip, sec, flashchip.sector_size);

    FlashIsOnGoing = 0;
    FLASH_INTR_UNLOCK(c_tmp);

    return ret;
}

static esp_err_t spi_flash_program(uint32_t target, uint32_t *src_addr, size_t len)
{
    uint32_t page_size;
    uint32_t pgm_len, pgm_num;
    uint8_t i;

    page_size = flashchip.page_size;
    pgm_len = page_size - (target % page_size);

    if (len < pgm_len) {
        if (ESP_OK != spi_flash_write_raw(&flashchip,  target, src_addr, len)) {
            return ESP_ERR_FLASH_OP_FAIL;
        }
    } else {
        if (ESP_OK != spi_flash_write_raw(&flashchip,  target, src_addr, pgm_len)) {
            return ESP_ERR_FLASH_OP_FAIL;
        }

        //whole page program
        pgm_num = (len - pgm_len) / page_size;

        for (i = 0; i < pgm_num; i++) {
            if (ESP_OK != spi_flash_write_raw(&flashchip,  target + pgm_len, src_addr + (pgm_len >> 2), page_size)) {
                return ESP_ERR_FLASH_OP_FAIL;
            }

            pgm_len += page_size;
        }

        //remain parts to program
        if (ESP_OK != spi_flash_write_raw(&flashchip,  target + pgm_len, src_addr + (pgm_len >> 2), len - pgm_len)) {
            return ESP_ERR_FLASH_OP_FAIL;
        }
    }

    return  ESP_OK;
}

static esp_err_t __spi_flash_write(size_t dest_addr, const void *src, size_t size)
{
    esp_err_t ret;
    FLASH_INTR_DECLARE(c_tmp);

    FLASH_INTR_LOCK(c_tmp);
    FlashIsOnGoing = 1;

    ret = spi_flash_program(dest_addr, (uint32_t *)src, size);

    FlashIsOnGoing = 0;
    FLASH_INTR_UNLOCK(c_tmp);

    return ret;
}

esp_err_t spi_flash_write(size_t dest_addr, const void *src, size_t size)
{
#undef FLASH_WRITE
#define FLASH_WRITE(dest, src, size)                \
{                                                   \
    ret = __spi_flash_write(dest, src, size);       \
    esp_task_wdt_reset();                           \
    if (ret) {                                      \
        return ret;                                 \
    }                                               \
}

#undef FLASH_READ
#define FLASH_READ(dest, src, size)                 \
{                                                   \
    ret = spi_flash_read(dest, src, size);          \
    if (ret) {                                      \
        return ret;                                 \
    }                                               \
}

    esp_err_t ret = ESP_ERR_FLASH_OP_FAIL;
    uint8_t *tmp = (uint8_t *)src;

    if (!size)
        return ESP_OK;

    if (src == NULL) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    if ((dest_addr + size) > flashchip.chip_size) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    if (spi_flash_check_wr_protect() == false) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    if (NOT_ALIGN(dest_addr)
        || NOT_ALIGN(tmp)
        || NOT_ALIGN(size)
        || IS_FLASH(src)) {
        uint8_t buf[SPI_READ_BUF_MAX];

        if (NOT_ALIGN(dest_addr)) {
            size_t r_addr = FLASH_ALIGN_BEFORE(dest_addr);
            size_t c_off = dest_addr - r_addr;
            size_t wbytes = FLASH_ALIGN_BYTES - c_off;

            wbytes = wbytes > size ? size : wbytes;

            FLASH_READ(r_addr, buf, FLASH_ALIGN_BYTES);
            memcpy(&buf[c_off], tmp, wbytes);
            FLASH_WRITE(r_addr, buf, FLASH_ALIGN_BYTES);

            dest_addr += wbytes;
            tmp += wbytes;
            size -= wbytes;
        }

        while (size > 0) {
            size_t len = size >= SPI_READ_BUF_MAX ? SPI_READ_BUF_MAX : size;
            size_t wlen = FLASH_ALIGN(len);

            if (wlen != len) {
                size_t l_b = wlen - FLASH_ALIGN_BYTES;

                FLASH_READ(dest_addr + l_b, &buf[l_b], FLASH_ALIGN_BYTES);                
            }

            memcpy(buf, tmp, len);

            FLASH_WRITE(dest_addr, buf, wlen);

            dest_addr += len;
            tmp += len;
            size -= len;
        }
    } else {
        FLASH_WRITE(dest_addr, src, size);
    }

    return ret;
}

static esp_err_t __spi_flash_read(size_t src_addr, void *dest, size_t size)
{
    esp_err_t ret;
    FLASH_INTR_DECLARE(c_tmp);

    FLASH_INTR_LOCK(c_tmp);
    FlashIsOnGoing = 1;

    ret = spi_flash_read_raw(&flashchip, src_addr, dest, size);

    FlashIsOnGoing = 0;
    FLASH_INTR_UNLOCK(c_tmp);

    return ret == 0 ? ESP_OK : ESP_ERR_FLASH_OP_FAIL;
}

esp_err_t spi_flash_read(size_t src_addr, void *dest, size_t size)
{
#undef FLASH_READ
#define FLASH_READ(addr, dest, size)                        \
{                                                           \
    ret = __spi_flash_read(addr, dest, size);               \
    esp_task_wdt_reset();                                   \
    if (ret)                                                \
        return ret;                                         \
}

    esp_err_t ret;
    uint8_t *tmp = (uint8_t *)dest;

    if (!size)
        return ESP_OK;

    if (tmp == NULL) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    if (NOT_ALIGN(src_addr)
        || NOT_ALIGN(tmp)
        || NOT_ALIGN(size)) {
        uint8_t buf[SPI_READ_BUF_MAX];

        if (NOT_ALIGN(src_addr)) {
            size_t r_addr = FLASH_ALIGN_BEFORE(src_addr);
            size_t c_off = src_addr - r_addr;
            size_t wbytes = FLASH_ALIGN_BYTES - c_off;

            wbytes = wbytes > size ? size : wbytes;

            FLASH_READ(r_addr, buf, FLASH_ALIGN_BYTES);
            memcpy(tmp, &buf[c_off], wbytes);

            tmp += wbytes;
            src_addr += wbytes;
            size -= wbytes;
        }

        while (size) {
            size_t len = size >= SPI_READ_BUF_MAX ? SPI_READ_BUF_MAX : size;
            size_t wlen = FLASH_ALIGN(len);

            FLASH_READ(src_addr, buf, wlen);

            memcpy(tmp, buf, len);

            src_addr += len;
            tmp += len;
            size -= len;            
        }
    } else {
        FLASH_READ(src_addr, tmp, size);
    }

    return ESP_OK;
}

/**
 * @brief  Erase a range of flash sectors
 */
esp_err_t spi_flash_erase_range(size_t start_address, size_t size)
{
    esp_err_t ret;
    size_t sec, num;

    if (start_address % SPI_FLASH_SEC_SIZE
            || size % SPI_FLASH_SEC_SIZE) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    if (spi_flash_check_wr_protect() == false) {
        return ESP_ERR_FLASH_OP_FAIL;
    }

    sec = start_address / SPI_FLASH_SEC_SIZE;
    num = size / SPI_FLASH_SEC_SIZE;

    /*
     * call "spi_flash_erase_sector" continuely to make the function to be able
     * to enter/exit critical state so that system core can feed watch
     */
    do {
        ret = spi_flash_erase_sector(sec++);

        esp_task_wdt_reset();
    } while (ret == ESP_OK && --num);

    return ret;
}

void esp_spi_flash_init(uint32_t spi_speed, uint32_t spi_mode)
{
    uint32_t freqdiv, freqbits;

    SET_PERI_REG_MASK(PERIPHS_SPI_FLASH_USRREG, BIT5);

    if (spi_speed < 3)
        freqdiv = spi_speed + 2;
    else if (0x0F == spi_speed)
        freqdiv = 1;
    else
        freqdiv = 2;

    if (1 >= freqdiv) {
        freqbits = SPI_FLASH_CLK_EQU_SYSCLK;
        SET_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, SPI_FLASH_CLK_EQU_SYSCLK);
        SET_PERI_REG_MASK(PERIPHS_IO_MUX_CONF_U, SPI0_CLK_EQU_SYSCLK);
    } else {
        freqbits = ((freqdiv - 1) << 8) + ((freqdiv / 2 - 1) << 4) + (freqdiv - 1);
        CLEAR_PERI_REG_MASK(PERIPHS_SPI_FLASH_CTRL, SPI_FLASH_CLK_EQU_SYSCLK);
        CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX_CONF_U, SPI0_CLK_EQU_SYSCLK);
    }
    SET_PERI_REG_BITS(PERIPHS_SPI_FLASH_CTRL, 0xfff, freqbits, 0);      

    if (spi_mode == ESP_IMAGE_SPI_MODE_QIO || spi_mode == ESP_IMAGE_SPI_MODE_QOUT) {
        user_spi_flash_dio_to_qio_pre_init();

        ESP_EARLY_LOGI("qio_mode", "Enabling default flash chip QIO");
    }
}

uintptr_t spi_flash_cache2phys(const void *cached)
{
    uint32_t map_size;
    uintptr_t addr_offset;
    uintptr_t addr = (uintptr_t)cached;

    const uint32_t reg = REG_READ(CACHE_FLASH_CTRL_REG);
    const uint32_t segment = (reg >> CACHE_MAP_SEGMENT_S) & CACHE_MAP_SEGMENT_MASK;

    if (reg & CACHE_MAP_2M) {
        map_size = CACHE_2M_SIZE;
        addr_offset = 0;
    } else {
        map_size = CACHE_1M_SIZE;
        if (reg & CACHE_MAP_1M_HIGH)
            addr_offset = CACHE_1M_SIZE;
        else
            addr_offset = 0;
    }

    if (addr <= CACHE_BASE_ADDR || addr >= CACHE_BASE_ADDR + map_size)
        return SPI_FLASH_CACHE2PHYS_FAIL;
    
    return segment * CACHE_2M_SIZE + (addr + addr_offset - CACHE_BASE_ADDR);
}
