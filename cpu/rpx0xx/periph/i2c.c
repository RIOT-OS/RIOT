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
#include <string.h> //for memcpy

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
#define I2C_SDA_PIN i2c_config[dev].sda
#define I2C_SCL_PIN i2c_config[dev].scl
#define I2C_DELAY 10 //potentially adjust

/* select i2c base */
long I2C_BASE = 0x40044000;
const long IC_DATA_CMD = 0x10;
const long IC_ENABLE = 0x6c;
const long IC_CON = 0x00;
const long IC_TAR = 0x04;

//used in transfers to store incoming/outgoing bytes
uint8_t txbuffer[256];



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

/*static inline int i2c_delay_timer(){
    //create timer for timeout, 10* highest transfer speed
    uint32_t timeoutTime = 10*MHZ(1);
    uint32_t POLL_COUNT = 0;
    while(POLL_COUNT < timeoutTime){
        POLL_COUNT++;
        //call checking fn ?
    }
    return 0;

}*/

/**
 * @brief delay for ms microseconds for line stability
 * @param ms microseconds to delay by
*/
static inline void i2c_delay(uint16_t us){
    for(uint16_t i = 0; i < us; i++){
        __asm__ volatile("nop"); //busy wait
    }
}

static inline bool check_received_ack(i2c_t dev){
    gpio_init(I2C_SDA_PIN, GPIO_IN);
    i2c_delay(10);
    gpio_write(I2C_SCL_PIN, 0);
    bool received_ack = !gpio_read(I2C_SDA_PIN);
    gpio_write(I2C_SCL_PIN, 0);
    //now reinitialise SDA back to output
    gpio_init(I2C_SDA_PIN, GPIO_OUT);
    return received_ack;
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

    //reset TX/RX FIFO Q
    for(int i = 0; i < 256; i++){
        txbuffer[i] = 0;
    }

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

        }
        else { //if 7bit addressing

            //init pins
            gpio_init(I2C_SDA_PIN, GPIO_OUT);
            gpio_init(I2C_SCL_PIN, GPIO_OUT);            

            //start condition
            gpio_write(I2C_SDA_PIN, 1);
            i2c_delay(I2C_DELAY);
            gpio_write(I2C_SCL_PIN, 0);
            i2c_delay(I2C_DELAY);

            //transmit slave address
            for(int i = 6; i >= 0; i--){
                gpio_write(I2C_SDA_PIN, (addr >> i) & 1);
                i2c_delay(I2C_DELAY);
                gpio_write(I2C_SCL_PIN, 1);
                i2c_delay(I2C_DELAY);
                gpio_write(I2C_SCL_PIN, 0);
            }

            //put read bit
            gpio_write(I2C_SDA_PIN, 1);
            i2c_delay(I2C_DELAY);
            gpio_write(I2C_SCL_PIN, 1);
            i2c_delay(I2C_DELAY);

            //read in each byte
            for(size_t i = 0; i < len; i++){

                //initialise byte
                txbuffer[i] = 0;

                //reinitialise SDA as input for reads
                gpio_init(I2C_SDA_PIN, GPIO_IN);
                i2c_delay(I2C_DELAY);

                //read in each bit
                for(int j = 7; j >= 0; j--){                
                    gpio_write(I2C_SCL_PIN, 1);
                    i2c_delay(I2C_DELAY);
                    txbuffer[i] |= gpio_read(I2C_SDA_PIN) << j;
                    gpio_write(I2C_SCL_PIN, 0);
                    i2c_delay(I2C_DELAY);
                }

                //reinitalise SDA as output
                gpio_init(I2C_SDA_PIN, GPIO_OUT);
                i2c_delay(I2C_DELAY);

                //send ACK except on last byte
                if(i == len - 1){
                    gpio_write(I2C_SDA_PIN, 1); //sending NACK
                    i2c_delay(I2C_DELAY);
                    gpio_write(I2C_SCL_PIN, 1);
                    i2c_delay(I2C_DELAY);
                    gpio_write(I2C_SCL_PIN, 0);
                }

                //stop condition - set IC_DATA_CMD[9] to 1
                uint32_t* memaddr = (uint32_t *) I2C_BASE;
                *(memaddr+IC_DATA_CMD) |= 1 << IC_DATA_CMD_STOP;

                if (!(flags & I2C_NOSTOP)) {
                    //gpio stop condition ... not needed?
                    gpio_write(I2C_SDA_PIN, 1);
                    i2c_delay(I2C_DELAY);
                    gpio_write(I2C_SCL_PIN, 1);
                    i2c_delay(I2C_DELAY);
                    gpio_write(I2C_SDA_PIN, 0);
                    i2c_delay(I2C_DELAY); //not necessary but improves signal integrity
                    gpio_write(I2C_SCL_PIN, 0);
                }

                //copy txbuffer back into data
                memcpy(data, txbuffer, len);

                return 0;
            }
            
            return 0;
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
    
    //write reg address first
    _i2c_bus[dev].cmd = 0;
    i2c_write_bytes(dev, addr, &reg, sizeof(reg), 0);

    //repeated start to indicate shift to read
    gpio_write(I2C_SDA_PIN, 1);
    i2c_delay(I2C_DELAY);
    gpio_write(I2C_SCL_PIN, 1);
    i2c_delay(I2C_DELAY);
    gpio_write(I2C_SCL_PIN, 0);
    gpio_write(I2C_SDA_PIN, 0);
    i2c_delay(I2C_DELAY);

    //read reg data
    _i2c_bus[dev].cmd = 1;
    int status = i2c_read_bytes(dev, addr, data, sizeof(reg), flags);

    return status;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t len, uint8_t flags)
{

    assert((dev < I2C_NUMOF) && (data != 0) && (len > 0) && (len < 256));

    if (flags & I2C_NOSTART) { //or I2C_ADDR10 ?
        return -EOPNOTSUPP;
    }

    //reset TX/RX FIFO Q
    for(int i = 0; i < 256; i++){
        txbuffer[i] = 0;
    }

    DEBUG("i2c write %d bytes", len);

    //prepare i2c bus
    long* baseaddr = (long *) I2C_BASE;
    _i2c_bus[dev].len = 8; //?
    _i2c_bus[dev].cmd = 0; // send write cmd ?
    *(baseaddr + IC_DATA_CMD) = _i2c_bus[dev].cmd;


    //transmit byte sequence
    //clear txbuffer and set to data
    for(int i = 0; i < 256; i++){
        txbuffer[i] = 0;
    }
    memcpy(txbuffer, data, len);

    //init pins
    gpio_init(I2C_SDA_PIN, GPIO_OUT);
    gpio_init(I2C_SCL_PIN, GPIO_OUT);     

    //start condition
    gpio_write(I2C_SDA_PIN, 1);
    i2c_delay(I2C_DELAY);
    gpio_write(I2C_SCL_PIN, 0);
    i2c_delay(I2C_DELAY);

    //transmit slave address
    for(int i = 6; i >= 0; i--){
        gpio_write(I2C_SDA_PIN, (addr >> i) & 1);
        i2c_delay(I2C_DELAY);
        gpio_write(I2C_SCL_PIN, 1);
        i2c_delay(I2C_DELAY);
        gpio_write(I2C_SCL_PIN, 0);
    }

    //set write bit
    gpio_write(I2C_SDA_PIN, 0);
    i2c_delay(I2C_DELAY);
    gpio_write(I2C_SCL_PIN, 1);
    i2c_delay(I2C_DELAY);

    //check that ACK is received from slave
    //first reinitialise SDA as input
    bool received_ack = check_received_ack(dev);

    if(!received_ack){
        return -EIO;
    }

    //transmit each byte
    for(size_t i = 0; i < len; i++){
        for(int j = 7; j >= 0; j--){
            gpio_write(I2C_SDA_PIN, (txbuffer[i] >> j) & 1);
            i2c_delay(I2C_DELAY);
            gpio_write(I2C_SCL_PIN, 1);
            i2c_delay(I2C_DELAY);
            gpio_write(I2C_SCL_PIN, 0);
        }

        //send LSB with set write bit
        gpio_write(I2C_SDA_PIN, 0);
        i2c_delay(I2C_DELAY);
        gpio_write(I2C_SCL_PIN, 1);
        i2c_delay(I2C_DELAY);

        //check to ensure ack received for each byte
        received_ack = check_received_ack(dev);
        if(!received_ack){
            return -EIO;
        }
    }

    //stop condition
    if (!(flags & I2C_NOSTOP)) {
        gpio_write(I2C_SDA_PIN, 1);
        i2c_delay(I2C_DELAY);
        gpio_write(I2C_SCL_PIN, 1);
        i2c_delay(I2C_DELAY);
        gpio_write(I2C_SDA_PIN, 0);
        i2c_delay(I2C_DELAY); //not necessary but improves signal integrity
        gpio_write(I2C_SCL_PIN, 0);
    }

    //copy txbuffer into data


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

    //copy data into txbuffer after the reg
    memcpy(&tx_buffer[reg_len], data, len);
    int retstatus = i2c_write_bytes(dev, addr, tx_buffer, 3, flags);


    return retstatus;
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

    tx_buffer[reg_len] = data;
    int retstatus = i2c_write_bytes(dev, addr, tx_buffer, 3, flags);


    return retstatus;
}
