/*
 * Copyright (C) 2024 Ryan Innes @ University of Southampton
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @note        WIP
 *
 * @author      Ryan Innes <rinnes361@gmail.com>
 * @}
 */

#include <errno.h>

#include "periph_conf.h"
#include "periph/i2c.h"
#include "periph/pio/i2c.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"


#define I2C_NUMOF_MAX_MASTER 1024
#define I2C_NUMOF_MAX_SLAVE 128
#define _I2C 3 //gpio pin function
#define _GPIO 0 //gpio pin function
#define SOC_I2C_FIFO_LEN 8
#define MAX_T_POLL_COUNT 1000

/* select i2c base */
long I2C_BASE = 0x40044000;
const long IC_DATA_CMD = 0x10;
const long IC_ENABLE = 0x6c;
const long IC_CON = 0x00;
const long IC_TAR = 0x04;


typedef struct {
    i2c_speed_t clk_freq;   /* clock freuency in Hz according to bus speed */
    uint8_t cmd;            /* command index of a transfer*/
    uint8_t cmd_op;         /* main command op code of a transfer */
    uint8_t len;            /* data length of a transfer */
    uint32_t status;        /* results of a transfer */
    mutex_t cmd_lock;       /* command execution locking */
    mutex_t dev_lock;       /* device locking */
} _i2c_bus_t;

static _i2c_bus_t _i2c_bus[32] = { //no idea if its supposed to be 32 or what I2C_NUMOF is
    {
        .cmd_lock = MUTEX_INIT_LOCKED,
        .dev_lock = MUTEX_INIT,
    }
};

static inline int delayTimer(){
    //create timer for timeout, 10* highest transfer speed
    uint32_t timeoutTime = 10*MHZ(1);
    uint32_t POLL_COUNT = 0;
    while(POLL_COUNT < timeoutTime){
        POLL_COUNT++;
        //call checking fn ?
    }

}

int i2c_transmit(uint8_t byte, i2c_t dev){
    //if write
    if(_i2c_bus[dev].cmd_op == 1){
        //lock device
        //mutex_lock(&_i2c_bus[dev].dev_lock);

        return 1;
    }
    else{ //if read
        return 1;
    }
    return 0;
}

void i2c_init(i2c_t dev)
{
    // does pico have max i2c dev conns?
    assert(I2C_NUMOF < I2C_NUMOF_MAX_MASTER);
    
    //start in master mode (slave mode disabled on reset)

    //speed supports fast mode plus at max
    assert(i2c_config[dev].speed <= I2C_SPEED_FAST_PLUS);

    /* GPIOs for SCL and SDA signals must not already be used for peripherals /
    if (((gpio_get_pin_usage(i2c_config[dev].scl) != _I2C) &&
         (gpio_get_pin_usage(i2c_config[dev].scl) != _GPIO)) ||
        ((gpio_get_pin_usage(i2c_config[dev].sda) != _I2C) &&
         (gpio_get_pin_usage(i2c_config[dev].sda) != _GPIO))) {
        LOG_TAG_ERROR("i2c", "GPIO%u and/or GPIO%u are used for %s/%s and "
                      "cannot be used as I2C interface\n",
                       i2c_config[dev].scl, i2c_config[dev].sda,
                       gpio_get_pin_usage_str(i2c_config[dev].scl),
                       gpio_get_pin_usage_str(i2c_config[dev].sda));
        assert(0);
    } */

    //set pin functions (?)

    gpio_set_function_select(i2c_config[dev].scl, _I2C);
    gpio_set_function_select(i2c_config[dev].sda, _I2C);

}

void i2c_acquire(i2c_t dev)
{
    mutex_lock(&_i2c_bus[dev].dev_lock);
}

void i2c_release(i2c_t dev)
{
    mutex_unlock(&_i2c_bus[dev].dev_lock);
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data,
                   size_t len, uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    assert(len > 0);
    assert(data != NULL);

    //reset TX/RX FIFO Q ?


    //set START cond

    //read bytes off FIFO

    //

    //copy paste guff test
    //int res;

    _i2c_bus[dev].cmd_op = 0; //I2C_LL_CMD_READ;
    _i2c_bus[dev].cmd = 0;

    /* reset TX/RX FIFO queue */
    //i2c_hal_txfifo_rst(&_i2c_hw[dev]);
    //i2c_hal_rxfifo_rst(&_i2c_hw[dev]);

    /*  if I2C_NOSTART is not set, START condition and ADDR is used */
    if (!(flags & I2C_NOSTART)) {
        /* send START condition */
        //_i2c_start_cmd(dev);

        /* address handling */
        if (flags & I2C_ADDR10) {
            //write IC_ENABLE[0] 0 to disable DW_apb_i2c
            long* memaddr = (long *) I2C_BASE;
            *(memaddr + IC_ENABLE) = *(memaddr + IC_ENABLE) & 0xFFFE;

            /* write max speed mode supported to IC_CON (bits 2:1)*/
            uint8_t mode = 0x2; //0x1 = standard, 0x2 = FM/FM+, 0x3 = HSM 
            uint8_t addrmode = 0x1; //0 for 7-bit, 1 for 10-bit
            uint8_t addrdata = 0x41 | (mode << 1) | (addrmode << 4);
            *(memaddr + IC_CON) = *(memaddr + IC_CON) | (0x57 & (addrdata)); // <-- TODO:  TEST THIS

            /* write target addr for dev */
            *(memaddr + IC_TAR) = (*(memaddr + IC_TAR) & 0x3FF) | dev; //consider bits 8,9,11,13 for gen call / start cmd gen

            /* enable dw_a2b_i2c */
            *(memaddr + IC_ENABLE) = *(memaddr + IC_ENABLE) | 0x1;

            /* write transfer dir and data to IC_DATA_CMD 
               in this case 1 to bit 8 to indicate read */
            *(memaddr + IC_DATA_CMD) = *(memaddr + IC_DATA_CMD) & 0x1FFFFF; //consider bit 9,10,11

            

            /*not mine below lol
            / prepare 10 bit address bytes /
            uint8_t addr10[2];
            addr10[0] = 0xf0 | (addr & 0x0300) >> 7 | I2C_MASTER_READ;
            addr10[1] = addr & 0xff;
            / send ADDR with read flag /
            _i2c_write_cmd(dev, addr10, 2);
            */
        }
        else {
            /* send ADDR with read flag */
            uint8_t addr7 = (addr << 1 | I2C_READ);
            addr7++;
            //_i2c_write_cmd(dev, &addr7, 1);
        }
    }

    /* read data bytes in blocks of SOC_I2C_FIFO_LEN bytes */
    //uint32_t off = 0;

    /* VVV to be sorted VVV

    / if len > SOC_I2C_FIFO_LEN read SOC_I2C_FIFO_LEN bytes at a time 
    while (len > SOC_I2C_FIFO_LEN) {
        /read one block of data bytes command 
        _i2c_bus[dev].len = SOC_I2C_FIFO_LEN;
        _i2c_read_cmd(dev, SOC_I2C_FIFO_LEN, false);
        _i2c_end_cmd(dev);
        _i2c_transfer(dev);

        res = _i2c_status_to_errno(dev);
        if (res) {
            return res;
        }

        / if transfer was successful, fetch the data from I2C RAM
        i2c_hal_read_rxfifo(&_i2c_hw[dev], data + off, len);

        / reset RX FIFO queue 
        i2c_hal_rxfifo_rst(&_i2c_hw[dev]);

        len -= SOC_I2C_FIFO_LEN;
        off += SOC_I2C_FIFO_LEN;
    }

    

    / read remaining data bytes command with a final NAK 
    _i2c_bus[dev].len = len;
    _i2c_read_cmd(dev, len, true);

    / if I2C_NOSTOP flag is not set, send STOP condition is used 
    if (!(flags & I2C_NOSTOP)) {
        / send STOP condition 
        _i2c_stop_cmd(dev);
    }
    else {
        / otherwise place end command in pipeline 
        _i2c_end_cmd(dev);
    }

    / finish operation by executing the command pipeline 
    _i2c_transfer(dev);

    if ((res = _i2c_status_to_errno(dev))) {
        return res;
    }

    / fetch the data from RX FIFO 
    i2c_hal_read_rxfifo(&_i2c_hw[dev], data + off, len);

    / return 0 on success */
    return 0;
}

int i2c_read_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  void *data, size_t len, uint8_t flags)
{
    //check validity of args
    if(len < 1 || data == NULL){
        return EINVAL;
    }
    //read regs, len bytes
    long* baseaddr = (long *) I2C_BASE;
    for(uint16_t i = 0; i < len; i++){
        _i2c_bus[dev].len = 8; //?
        _i2c_bus[dev].cmd = 1; // send read cmd ?
        //or this sends read cmd ? sets IC_DATA_CMD-CMD to 1 which is read
        *(baseaddr + IC_DATA_CMD) = _i2c_bus[dev].cmd;
        uint8_t* dataAddr = (uint8_t *) data;
        /* set specified target dataAddr to that which occupies the data bits of 
           IC_DATA_CMD */
        dataAddr[i] = *(baseaddr + IC_DATA_CMD) & 0xFF;

        //send stop bit
        if(i == len-1){
            *(baseaddr + IC_DATA_CMD) = 0xFF & IC_DATA_CMD_STOP;
        }
    }
    uint8_t* dataAddr2 = (uint8_t *) data;
    dataAddr2[0] = 0xFF;


    //
    //throwaway code
    uint16_t address = addr;
    address += 1;
    uint16_t regist = reg;
    regist++;
    size_t length = len;
    length++;
    uint8_t flagss = flags;
    flagss++;
    return 0;

}

int i2c_read_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                 void *data, uint8_t flags)
{   
    //read reg
    //check validity of args
    if(data == NULL){
        return EINVAL;
    }
    


    //read regs, len bytes
    long* baseaddr = (long *) I2C_BASE;
    _i2c_bus[dev].len = 8; //?
    _i2c_bus[dev].cmd = 1; // send read cmd ?
    //or this sends read cmd ? sets IC_DATA_CMD-CMD to 1 which is read
    *(baseaddr + IC_DATA_CMD) = _i2c_bus[dev].cmd;

    /* actually do the read - take addr of where data is to be stored
       and plonk in TX FIFO data there*/
    uint8_t* dataAddr = (uint8_t *) data;
    *dataAddr = *(baseaddr + IC_DATA_CMD) & 0xF;

    //send stop bit
    *(baseaddr + IC_DATA_CMD) = 0xFF & IC_DATA_CMD_STOP;
    uint8_t* dataAddr2 = (uint8_t *) data;
    dataAddr2[0] = 0xFF;

    //throwaway code
    i2c_t device = dev;
    device += 1;
    uint16_t address = addr;
    address += 1;
    uint16_t regist = reg;
    regist++;
    assert((uint32_t) data != 0);
    uint8_t flagss = flags;
    flagss++;
    return 0;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t len, uint8_t flags)
{

    assert((dev < I2C_NUMOF) && (data != 0) && (len > 0) && (len < 256));

    if (flags & I2C_NOSTART) { //or I2C_ADDR10 ?
        return -EOPNOTSUPP;
    }

    DEBUG("i2c write %d bytes", len);

    //prepare i2c bus
    long* baseaddr = (long *) I2C_BASE;
    _i2c_bus[dev].len = 8; //?
    _i2c_bus[dev].cmd = 0; // send write cmd ?
    *(baseaddr + IC_DATA_CMD) = _i2c_bus[dev].cmd;

    //hold return value from transmit
    int status = 0;

    //transmit each byte
    for(int i = 0; i < len; i++){
        //send byte ? receive ack ?
        int status = i2c_transmit(data[i], dev);
    }

    i2c_t device = dev;
    device += 1;
    uint16_t address = addr;
    address += 1;
    assert((uint32_t) data != 0);
    size_t length = len;
    length++;
    uint8_t flagss = flags;
    flagss++;
    return 0;
}

int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                   const void *data, size_t len, uint8_t flags)
{
    //write reg lol idk what im doing
    long* baseaddr = (long *) I2C_BASE;
    _i2c_bus[dev].len = 8; //?
    _i2c_bus[dev].cmd = 0; // send write cmd ?
    *(baseaddr + IC_DATA_CMD) = _i2c_bus[dev].cmd;

    uint8_t tx_buffer[256] = {};
    int reg_len = 0; //know register length in bytes
    //if in 16-bit register addressing mode, split across tx bytes
    if(flags & I2C_REG16){
        tx_buffer[0] = reg >> 8;
        tx_buffer[1] = reg & 0xFF;
        reg_len = 2;
    }
    else {
        tx_buffer[0] = reg;
        reg_len = 1;
    }

    size_t len = reg_len;

    //copy data into txbuffer after the reg
    memcpy(&tx_buffer[reg_len], data, len);
    int retval = i2c_write_bytes(dev, addr, tx_buffer, 3, flags);

    i2c_t device = dev;
    device += 1;
    uint16_t address = addr;
    address += 1;
    uint16_t regist = reg;
    regist++;
    assert((uint32_t) data != 0);
    size_t length = len;
    length++;
    uint8_t flagss = flags;
    flagss++;
    return retval;
}

int i2c_write_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                  uint8_t data, uint8_t flags)
{
    //write reg lol idk what im doing
    long* baseaddr = (long *) I2C_BASE;
    _i2c_bus[dev].len = 8; //?
    _i2c_bus[dev].cmd = 0; // send write cmd ?
    *(baseaddr + IC_DATA_CMD) = _i2c_bus[dev].cmd;

    uint8_t tx_buffer[256] = {};
    int reg_len = 0; //know register length in bytes
    //if in 16-bit register addressing mode, split across tx bytes
    if(flags & I2C_REG16){
        tx_buffer[0] = reg >> 8;
        tx_buffer[1] = reg & 0xFF;
        reg_len = 2;
    }
    else {
        tx_buffer[0] = reg;
        reg_len = 1;
    }

    size_t len = reg_len;
    tx_buffer[reg_len] = data;
    int retval = i2c_write_bytes(dev, addr, tx_buffer, 3, flags);



    i2c_t device = dev;
    device += 1;
    uint16_t address = addr;
    address += 1;
    uint16_t regist = reg;
    regist++;
    assert(data != 1);
    uint8_t flagss = flags;
    flagss++;
    return retval;
}
