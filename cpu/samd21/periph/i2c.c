/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_samr21
 * @{
 *
 * @file
 * @brief       I2C driver implementation
 *
 * @author      Rane Balslev (SAMR21) <ranebalslev@gmail.com> 
 *
 * @}
 */



#include <stdint.h>
#include "cpu.h"
#include "board.h"
#include "hwtimer.h" 
#include "sched.h"
#include "thread.h" 
#include "periph/i2c.h"
#include "periph_conf.h"
#include "port_util.h"
#define ENABLE_DEBUG    (1)
#include "debug.h"


/* guard */
#if I2C_0_EN

void Set_Pin_Mux_Config(int);
#define div_ceil(a, b)      (((a) + (b) - 1) / (b))

/* static function definitions */
static inline void  _start(SercomI2cm *dev, uint8_t address, uint8_t rw_flag);
static inline void _write(SercomI2cm *dev, char *data, int length);
static inline void _read(SercomI2cm *dev, char *data, int length);
static inline void _stop(SercomI2cm *dev);


/**
 * @brief Initialize an I2C device to run as bus master
 *
 * @param[in] dev           the device to initialize
 * @param[in] speed         the selected bus speed
 *
 * @return                  0 on successful initialization
 * @return                  -1 on undefined device given
 * @return                  -2 on unsupported speed value
 */

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
	SercomI2cm *I2CSercom = 0;
	PortGroup *port_group;
	uint8_t pin_scl;
	uint8_t pin_sda;
	uint32_t i2c_port;
	uint32_t clock_source_speed;
	uint8_t sercom = -1;

	uint32_t timeout_counter = 0;
	int32_t tmp_baud;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
			I2CSercom = &I2C_0_DEV;
			port_group = &I2C_0_PORT;
			pin_scl = I2C_SDA;
			pin_sda = I2C_SCL;
			i2c_port = I2C_0_PINS;
			clock_source_speed = I2C_0_REF_F;
			sercom = 3;
            break;
#endif
        default:
			DEBUG("I2C FALSE VALUE\n");
            return -1;
    }
	
	/*Reset I2C */
	I2CSercom->CTRLA.reg = SERCOM_I2CS_CTRLA_SWRST;
	while(I2CSercom->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);

    /* Turn on power manager for sercom */
	PM->APBCMASK.reg |= (PM_APBCMASK_SERCOM0 << sercom);

	//I2C using CLK GEN 0
	uint16_t GCLK_SERCOM_CORE = 20 + sercom;
	uint8_t SERCOM_GCLK_ID_SLOW = 19;

	GCLK->CLKCTRL.reg = (uint32_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 << GCLK_CLKCTRL_GEN_Pos | (GCLK_SERCOM_CORE<< GCLK_CLKCTRL_ID_Pos));
	while (GCLK->STATUS.bit.SYNCBUSY);

	GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 << GCLK_CLKCTRL_GEN_Pos  | (SERCOM_GCLK_ID_SLOW << GCLK_CLKCTRL_ID_Pos)));
	while (GCLK->STATUS.bit.SYNCBUSY);


	/* Check if module is enabled. */
	if (I2CSercom->CTRLA.reg & SERCOM_I2CM_CTRLA_ENABLE) {
		DEBUG("STATUS_ERR_DENIED\n");
		return -3;
	}
	/* Check if reset is in progress. */
	if (I2CSercom->CTRLA.reg & SERCOM_I2CM_CTRLA_SWRST) {
		DEBUG("STATUS_BUSY\n");
		return -3;
	}
	
	/************ SERCOM PAD0 - SDA and SERCOM PAD1 - SCL  ************/
	// DIR + INEN at one: in/out pin. DIRSET modifies DIR in order to set I2C SDA/SCL on output
	port_group->DIRSET.reg = (1 << pin_scl);
	port_group->DIRSET.reg = (1 << pin_sda);
	
	/* The Write Configuration register (WRCONFIG) requires the
	 * pins to to grouped into two 16-bit half-words (PIN are set up to 31)*/
	uint32_t lower_pin_mask = (i2c_port & 0xFFFF);
	uint32_t upper_pin_mask = (i2c_port >> 16);

	port_group->WRCONFIG.reg =
		  (lower_pin_mask << PORT_WRCONFIG_PINMASK_Pos) \
		| PORT_WRCONFIG_PMUXEN \
		| PORT_WRCONFIG_PMUX(0x3) \
		| PORT_WRCONFIG_INEN \
		| PORT_WRCONFIG_WRPMUX \
		| PORT_WRCONFIG_WRPINCFG;
	
	port_group->WRCONFIG.reg =
		  (upper_pin_mask << PORT_WRCONFIG_PINMASK_Pos) \
		| PORT_WRCONFIG_PMUXEN \
		| PORT_WRCONFIG_PMUX(0x3) \
		| PORT_WRCONFIG_INEN \
		| PORT_WRCONFIG_WRPMUX \
		| PORT_WRCONFIG_WRPINCFG \
		| PORT_WRCONFIG_HWSEL;
	
	
	/**** I2C CONFIGURATION ****/
	while(I2CSercom->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
	I2CSercom->CTRLA.bit.ENABLE = 0; //Disable I2C to write confs

	/* Set sercom module to operate in I2C master mode. */
	I2CSercom->CTRLA.reg = SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;

	/* Set transfer speed: SERCOM_I2CM_CTRLA_SPEED(0): Standard-mode (Sm) up to 100 kHz and Fast-mode (Fm) up to 400 kHz*/
	I2CSercom->CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(0);

	/*set HOLD time*/
	//I2CSercom->CTRLA.reg |= I2C_0_START_HOLD_TIME;

	/* Set whether module should run in standby.
	* Should be enable if system_is_debugger_present()==true*/
	//tmp_ctrla |= SERCOM_I2CM_CTRLA_RUNSTDBY;

	/* Enable Smart Mode (ACK is sent when DATA.DATA is read) */
	I2CSercom->CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;
	
	/* Find and set baudrate. Read speed configuration */
    switch (speed) {
    	case I2C_SPEED_NORMAL:
            tmp_baud = (int32_t)(((clock_source_speed + (2*(100000)) - 1) / (2*(100000))) - 5);
			if (tmp_baud < 255 && tmp_baud > 0) {
				I2CSercom->BAUD.reg = SERCOM_I2CM_BAUD_BAUD(tmp_baud);
				}
			break;
        case I2C_SPEED_FAST:
            tmp_baud = (int32_t)(((clock_source_speed + (2*(400000)) - 1) / (2*(400000))) - 5);
			if (tmp_baud < 255 && tmp_baud > 0) {
				I2CSercom->BAUD.reg = SERCOM_I2CM_BAUD_BAUD(tmp_baud);
				}
			break;
        case I2C_SPEED_FAST_PLUS:
			tmp_baud = (int32_t)(((clock_source_speed + (2*(3400000)) - 1) / (2*(3400000))) - 1);
			if (tmp_baud < 255 && tmp_baud > 0) {
				I2CSercom->BAUD.reg =SERCOM_I2CM_BAUD_HSBAUD(tmp_baud);
            }
			break;
        default:
			DEBUG("BAD BAUDRATE\n");
            return -2;
    }
	
	/* ENABLE I2C MASTER */
	i2c_poweron(dev);
	
	/* Start timeout if bus state is unknown. */
	while (!(I2CSercom->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(1))) {
		timeout_counter++;
		if(timeout_counter >= 65535) {
			I2CSercom->STATUS.reg = SERCOM_I2CM_STATUS_BUSSTATE(1);
			DEBUG("I2C Init has a timeout, force idle!\n"); /* Timeout, force bus state to idle. */
		}
	}
	return 0;
}


 // int i2c_init_master(i2c_t dev, i2c_speed_t speed)
 // {
 	
 // 	SercomI2cm *i2c = 0;
 	
	// switch (dev) 
	// {
	// #if I2C_0_EN
 //        case I2C_0:
	// 		i2c = &(I2C_0_DEV);
 //        break;
	// #endif
 //        default:
 //            return -1;
 //    }

	// /* Turn on module in PM */
	// PM->APBCMASK.reg |= PM_APBCMASK_SERCOM3;//(1 << pm_index);

	// /* Set up the GCLK for the module */
 //    GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (SERCOM3_GCLK_ID_CORE << GCLK_CLKCTRL_ID_Pos)));
 //    while (GCLK->STATUS.bit.SYNCBUSY);

 //    //i2c->CTRLA.bit.SWRST = true;
	
	// /* Check if module is enabled and disable. */
	// if (i2c->CTRLA.reg & SERCOM_I2CM_CTRLA_ENABLE) {
	// 	i2c_poweroff(dev);
	// }

	// while (i2c->SYNCBUSY.bit.SWRST);
	// while(i2c->CTRLA.bit.SWRST);

	// 	/* Check if reset is in progress. */
	// if (i2c->CTRLA.reg & SERCOM_I2CM_CTRLA_SWRST) 
	// {
	// 	DEBUG("\n\nSWRST in progress\n\n");
	// }

	// /* Set sercom module to operate in I2C master mode. */
	// i2c->CTRLA.reg = SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;
	// while (i2c->SYNCBUSY.reg);

	// /*set configuration*/
	// uint32_t tmp_ctrla;

	// //Set_Pin_Mux_Config(I2C_PINMUX_PAD0_MUXPOSITION);
	// //Set_Pin_Mux_Config(I2C_PINMUX_PAD1_MUXPOSITION);
 //    //pmux_set(I2C_PINMUX_PAD0_PIN, D);
 //    //pmux_set(I2C_PINMUX_PAD1_PIN, D);




	// // SCL = OUTPUT
	// I2C_PINMUX_DEV.DIRSET.reg = (1 << (I2C_PINMUX_PAD1_PIN)); // see UART NO % 32 ???
	// I2C_PINMUX_DEV.PINCFG[I2C_PINMUX_PAD1_PIN % 32].bit.INEN = false;
	// //I2C_PINMUX_DEV.OUTSET.reg = (1 << (I2C_PINMUX_PAD1_PIN % 32));
	//  // SDA INPUT/OUTPUT set    
 //    I2C_PINMUX_DEV.DIRSET.reg = (1 << (I2C_PINMUX_PAD0_PIN)); // see UART NO % 32 
	// I2C_PINMUX_DEV.PINCFG[I2C_PINMUX_PAD0_PIN % 32].bit.INEN = true;    
 //    //I2C_PINMUX_DEV.OUTSET.reg = (1 << (I2C_PINMUX_PAD0_PIN % 32));




 //   /* enable PMUX for pins and set to config D. See spec p. 12 */
 //        I2C_PINMUX_DEV.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG \
 //                                    | PORT_WRCONFIG_WRPMUX \
 //                                    | PORT_WRCONFIG_PMUX(0x3) \
 //                                    | PORT_WRCONFIG_PMUXEN \
 //                                    | I2C_0_PINS;




	// /* Set whether module should run in standby. */
	// /*DSU->STATUSB.reg & DSU_STATUSB_DBGPRES <<-- (0x1u << 1); for debugger check!*/


	// /* Check and set start data hold timeout. */
	// if (I2C_0_START_HOLD_TIME != SERCOM_I2CM_CTRLA_SDAHOLD(0)/*I2C_MASTER_START_HOLD_TIME_DISABLED*/) {
	// 	tmp_ctrla |= I2C_0_START_HOLD_TIME;
	// }

	// /* Check and set transfer speed */
	// tmp_ctrla |= I2C_0_TRANSFER_SPEED;

	// /* Write config to register CTRLA. */
	// i2c->CTRLA.reg |= tmp_ctrla;
	// //DEBUG("CTRLA set up DONE!!\n" );
	// while (i2c->SYNCBUSY.reg);

	// /* Baud rate acceptable. */
	// //(uint8_t) (((uint32_t) I2C_0_F_REF) / (2 * (I2C_0_BAUDRATE*1000)) - 1);
	// int32_t baud = (int32_t)(div_ceil(I2C_0_F_REF, (2000*(I2C_0_BAUDRATE))) - 5);
	// i2c->BAUD.reg = SERCOM_I2CM_BAUD_BAUD(baud);// | SERCOM_I2CM_BAUD_HSBAUD(tmp_baud_hs);

	// /* Enable I2C device*/
	// while (i2c->SYNCBUSY.reg);
	// /* Set SMART mode EN configurations in CTRLB. */
	// i2c->CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;
	// while (i2c->SYNCBUSY.reg);

	// i2c_poweron(dev);

 // 	return 0;
 // }

 /**
 * @brief Initialize an I2C device to run in slave mode
 *
 * @param[in] dev           the device to initialize
 * @param[in] address       the devices I2C address
 *
 * @return                  0 on success
 * @return                  -1 on undefined device given
 */
int i2c_init_slave(i2c_t dev, uint8_t address)
{
	printf("%s\n", "i2c_init_slave NOT IMPLEMENTED!\n");
	return 0;
}

/**
 * @brief Read one byte from an I2C device with the given address
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  address      bus address of the target device
 * @param[out] data         the result that was read
 *
 * @return                  the number of bytes that were read
 * @return                  -1 on undefined device given
 * @return                  -2 on invalid address
 */
int i2c_read_byte(i2c_t dev, uint8_t address, char *data)
{
	return i2c_read_bytes(dev, address, data, 1);
}


/**
 * @brief Read multiple bytes from an I2C device with the given address
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  address      bus address of the target device
 * @param[out] data         array holding the received bytes
 * @param[in]  length       the number of bytes to read into `data`
 *
 * @return                  the number of bytes that were read
 * @return                  -1 on undefined device given
 */
int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{ 
	SercomI2cm *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = &I2C_0_DEV;
            break;
#endif
        default:
            return -1;
    }

    /* start transmission and send slave address */
    _start(i2c, address, I2C_FLAG_READ);
    /* read data to register */
    _read(i2c, data, length);
    _stop(i2c);
    /* return number of bytes sent */
    return length;
} 
// int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
// {
// 	SercomI2cm *i2c = 0;

// 	switch (dev) {
// 	#if I2C_0_EN
//         case I2C_0:
//             i2c = &(I2C_0_DEV);
//             break;
// 	#endif
//         default:
//             return -1;
//     }
//     //DEBUG("Read start!\n" );
// 	/* Written buffer counter. */
// 	uint16_t counter = 0;
//     bool sclsm_flag = i2c->CTRLA.bit.SCLSM;

//     while (i2c->SYNCBUSY.reg);

   

//     /* Set action to ACK. */
// 	i2c->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;
// 	//DEBUG("Read set ACK 1! DONE\n" );
// 		/* Set address and direction bit. Will send start command on bus. */
// 		i2c->ADDR.reg = (address << 1) | I2C_FLAG_READ |
// 			(FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED << SERCOM_I2CM_ADDR_HS_Pos);
// 	//DEBUG("Read after Set addr and dir bits!\n" );
// 	uint16_t timeout_counter = 0;	
// 	while (!(i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) &&
// 			!(i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) {

// 		if (++timeout_counter >= I2C_BUFFER_TIMEOUT) {
// 			DEBUG("waiting for bus interrupt time out!\n");
// 			return -1;
// 		}
// 	};
// 	//DEBUG("Read after INTFLAG timeout check DONE!\n" );

// 	/* Check for error and ignore bus-error; workaround for BUSSTATE stuck in
// 	 * BUSY */
// 	if (i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB) {

// 		/* Clear write interrupt flag */
// 		i2c->INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;

// 		// /* Check arbitration. */
// 		// if (i2c->STATUS.reg & SERCOM_I2CM_STATUS_ARBLOST) {
// 		// 	 Return packet collision. 
// 		// 	DEBUG("1 - packet collision error!");
// 		// 	return -1;
// 		// }
// 	/* Check that slave responded with ack. */
// 	} else if (i2c->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
// 		/* Slave busy. Issue ack and stop command. */
// 		i2c->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);

// 		/* Return bad address value. */
// 			DEBUG("1 - bad address error!");
// 			return -1;
// 	}
// 	//DEBUG("Read after INTFLAG  error ignore check 1 DONE!\n" );

// 	/* Set action to ack. */
// 	i2c->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;
// 	//DEBUG("Read set ACK 2! DONE\n" );

// 	/* Check for error and ignore bus-error; workaround for BUSSTATE stuck in
// 	 * BUSY */
// 	if (i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB) 
// 	{

// 		/* Clear write interrupt flag */
// 		i2c->INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;

// 		// /* Check arbitration. */
// 		// if (i2c->STATUS.reg & SERCOM_I2CM_STATUS_ARBLOST) {
// 		// 	 Return packet collision. 
// 		// 	DEBUG("2 - packet collision error!");
// 		// 	return -1;
// 		// }
// 	/* Check that slave responded with ack. */
// 	} 
// 	else if (i2c->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) 
// 	{
// 		/* Slave busy. Issue ack and stop command. */
// 		i2c->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);

// 		/* Return bad address value. */
// 			DEBUG("2 - bad address error!");
// 			return -1;
// 	}
// 	//DEBUG("Read after INTFLAG  error ignore check 2 DONE!\n" );
// 	while (length--) {
// 	// /* Check that bus ownership is not lost. */
// 	// if (!(i2c->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(2))) 
// 	// {
// 	// 	DEBUG("3 - packet collision error!");
// 	// 	return -1;
// 	// }
// 		if (((!sclsm_flag) && (length == 0)) || ((sclsm_flag) && (length == 1))) 
// 		{
// 			/* Set action to NACK */
// 			i2c->CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
// 		} 
// 		else 
// 		{
// 			/* Save data to buffer. */
// 			while (i2c->SYNCBUSY.reg);
// 			data[counter++] = i2c->DATA.reg;
// 			/* Wait for response. */
// 			while (i2c->SYNCBUSY.reg);
// 		}
// 	}
// 	/*send stop command*/
// 	while (i2c->SYNCBUSY.reg);
// 	i2c->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
// 	while (i2c->SYNCBUSY.reg);
// 	data[counter] = i2c->DATA.reg;
// 	//DEBUG("Read Afer DATA.reg DONE -> last step\n" );
// 	return 0;
// }

/**
 * @brief Read one byte from a register at the I2C slave with the given address
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  address      bus address of the target device
 * @param[in]  reg          the register address on the targeted I2C device
 * @param[out] data         the result that was read
 *
 * @return                  the number of bytes that were read
 * @return                  -1 on undefined device given
 */
int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, char *data)
{
	return i2c_read_regs(dev, address, reg, data, 1);
}

/**
 * @brief Read multiple bytes from a register at the I2C slave with the given address
 *
 * @param[in]  dev          I2C peripheral device
 * @param[in]  address      bus address of the target device
 * @param[in]  reg          the register address on the targeted I2C device
 * @param[out] data         array holding the received bytes
 * @param[in]  length       the number of bytes to read into `data`
 *
 * @return                  the number of bytes that were read
 * @return                  -1 on undefined device given
 */
int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
	SercomI2cm *i2c;

    switch (dev) 
    {
#if I2C_0_EN
        case I2C_0:
            i2c = &I2C_0_DEV;
            break;
#endif
        default:
            return -1;
    }

    /* start transmission and send slave address */
    _start(i2c, address, I2C_FLAG_WRITE);
    /* send register address/command and wait for complete transfer to be finished*/
    _write(i2c, (char *)(&reg), 1);
    return i2c_read_bytes(dev, address, data, length);
}

// int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
// {
// 	printf("%s\n", "i2c_read_regs NOT IMPLEMENTED!");
// 	SercomI2cm *i2c = 0;

// 	switch (dev) {
// 	#if I2C_0_EN
//         case I2C_0:
//             i2c = &(I2C_0_DEV);
//             break;
// 	#endif
//         default:
//             return -1;
//     }
//     DEBUG("Write start!\n" );
// 	return 0;
// }

/**
 * @brief Write one byte to an I2C device with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] address       bus address of the target device
 * @param[in] data          byte to write to the device
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
	return i2c_write_bytes(dev, address, &data, 1);
}

/**
 * @brief Write multiple bytes to an I2C device with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] address       bus address of the target device
 * @param[in] data          array with bytes to write to the target device
 * @param[in] length        number of bytes to write to the target device
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
	SercomI2cm *I2CSercom;
	
	    switch (dev) {
#if I2C_0_EN
        case I2C_0:
			I2CSercom = &I2C_0_DEV;
            break;
#endif
        default:
            return -1;
    }
	
	_start(I2CSercom, address, I2C_FLAG_WRITE);
	_write(I2CSercom, data, length);
	//hwtimer_wait(HWTIMER_TICKS_TO_US(20));
	_stop(I2CSercom);
	return length;
} 
// int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
// {
// 	SercomI2cm *i2c = 0;

// 	switch (dev) {
// 	#if I2C_0_EN
//         case I2C_0:
//             i2c = &(I2C_0_DEV);
//             break;
// 	#endif
//         default:
//             return -1;
//     }
//     DEBUG("i2c_write_bytes 1!\n" );

// 	uint16_t tmp_data_length = length;

// 	while (i2c->SYNCBUSY.reg);

// 	// /* Switch to high speed mode */
// 	// if (packet->high_speed) {
// 	// 	_i2c_master_send_hs_master_code(module, packet->hs_master_code);
// 	// }
// 	DEBUG("i2c_write_bytes 2!\n" );
// 	/* Set action to ACK. */
// 	i2c->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;


// 	//i2c->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(1);
// 	/* Set address and direction bit. Will send start command on bus. */
// 	i2c->ADDR.reg = (address << 1) | I2C_FLAG_WRITE;// | (FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED << SERCOM_I2CM_ADDR_HS_Pos);
// 	DEBUG("i2c_write_bytes 3!\n" );
	

// 		/* Wait for response on bus. */
// 	uint16_t timeout_counter = 0;	
// 	while (!(i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) && !(i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) 
// 	{
// 		if (++timeout_counter >= I2C_BUFFER_TIMEOUT) {
// 			DEBUG("\nwrite bytes: waiting for bus interrupt time out 0!\n");
// 			return -1;
// 		}
// 	};




// 	/*TEST!*/
// 	/* Set action to ACK. */
// 	//i2c->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;





// 	/* Check for error and ignore bus-error; workaround for BUSSTATE stuck in
// 	 * BUSY */
// 	if (i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB) {

// 		/* Clear write interrupt flag */
// 		i2c->INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;

// 		/* Check arbitration. */
// 		if (i2c->STATUS.reg & SERCOM_I2CM_STATUS_ARBLOST) {
// 			/* Return packet collision. */
// 			DEBUG("\nwrite bytes: packet collision error 0!\n");
// 				return -1;
// 		}
// 	/* Check that slave responded with ack. */
// 	} else if (i2c->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
// 		/* Slave busy. Issue ack and stop command. */
// 		i2c->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);

// 		/* Return bad address value. */
// 		DEBUG("\nwrite bytes: BAD ADDRESS!!\n");
// 				return -1;
// 	}

// 	DEBUG("i2c_write_bytes 4!\n" );
// 		/* Buffer counter. */
// 		uint16_t buffer_counter = 0;

// 	/* Write data buffer. */
// 	while (tmp_data_length--) 
// 	{
// 		/* Check that bus ownership is not lost. */
// 		// if (!(i2c->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(2))) 
// 		// {
// 		// 	DEBUG("\nwrite bytes: packet collision error 1!\n");
// 		// 	return -1;
// 		// }

// DEBUG("i2c_write_bytes 5!\n" );

// 		/* Write byte to slave. */
// 		while (i2c->SYNCBUSY.reg);
// 		i2c->DATA.reg = data[buffer_counter++];

// DEBUG("i2c_write_bytes 6!\n" );

// 		/* Wait for response. */
// 			/* Wait for response on bus. */
// 		// uint16_t timeout_counter2 = 0;	
// 		// while (!(i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) && !(i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) 
// 		// {
// 		// 	if (++timeout_counter2 >= I2C_BUFFER_TIMEOUT) {
// 		// 		DEBUG("write bytes: waiting for bus interrupt time out 2!\n");
// 		// 		return -1;
// 		// 	}
// 		// };
// 		// if((i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) && (i2c->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB))
// 		// 	break; 
		
// DEBUG("i2c_write_bytes 7!\n" );

// 		/* Check for NACK from slave. */
// 		if (i2c->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
// 			/* Return bad data value. */
// 			DEBUG("write bytes: Check for NACK from slave -> bad data value!\n");
// 			return -1;
// 		}
// 	}

// 		/* Stop command */
// 	while (i2c->SYNCBUSY.reg);
// 	i2c->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);

	
// 	DEBUG("i2c_write_bytes FINAL!\n" );
// 	return 0;
// }

/**
 * @brief Write one byte to a register at the I2C slave with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] address       bus address of the target device
 * @param[in] reg           the register address on the targeted I2C device
 * @param[in] data          byte to write to the device
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, char data)
{
	return i2c_write_regs(dev, address, reg, &data, 1);
}

/**
 * @brief Write multiple bytes to a register at the I2C slave with the given address
 *
 * @param[in] dev           I2C peripheral device
 * @param[in] address       bus address of the target device
 * @param[in] reg           the register address on the targeted I2C device
 * @param[in] data          array with bytes to write to the target device
 * @param[in] length        number of bytes to write to the target device
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
 int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
	SercomI2cm *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = &I2C_0_DEV;
            break;
#endif
        default:
            return -1;
    }

    /* start transmission and send slave address */
    _start(i2c, address, I2C_FLAG_WRITE);
    /* send register address and wait for complete transfer to be finished*/
    _write(i2c, (char *)(&reg), 1);
    /* write data to register */
    _write(i2c, data, length);
    /* finish transfer */
    _stop(i2c);
    return length;
}
// int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
// {
// 	printf("%s\n", "i2c_write_regs NOT IMPLEMENTED!");
// 	SercomI2cm *i2c = 0;

// 	switch (dev) {
// 	#if I2C_0_EN
//         case I2C_0:
//             i2c = &(I2C_0_DEV);
//             break;
// 	#endif
//         default:
//             return -1;
//     }
//     DEBUG("Write regs start!\n" );
//     i2c_write_byte(dev, address, reg);

// 	return i2c_write_bytes(dev, address, data , length+1);
// }

/**
 * @brief Power on the given I2C peripheral
 *
 * @param[in] dev           the I2C device to power on
 */
void i2c_poweron(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
        	while(I2C_0_DEV.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
        	I2C_0_DEV.CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;			
            break;
#endif
		default:
            return;
    }
} 
// void i2c_poweron(i2c_t dev)
// {
// 	SercomI2cm *i2c = 0;
 	
// 	switch (dev) 
// 	{
// 	#if I2C_0_EN
//         case I2C_0:
// 			i2c = &(I2C_0_DEV);
//         break;
// 	#endif
//         default:
//             return;
//     }
//     while (i2c->SYNCBUSY.reg);
// 	i2c->CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;
// 	return;
// }

/**
 * @brief Power off the given I2C peripheral
 *
 * @param[in] dev           the I2C device to power off
 */
 void i2c_poweroff(i2c_t dev)
{
	switch (dev) {
#if I2C_0_EN
        case I2C_0:
        	while(I2C_0_DEV.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
        	I2C_0_DEV.CTRLA.bit.ENABLE = 0;
            break;
#endif
		default:
            return;
    }
}
// void i2c_poweroff(i2c_t dev)
// {
// 	SercomI2cm *i2c = 0;
 	
// 	switch (dev) 
// 	{
// 	#if I2C_0_EN
//         case I2C_0:
// 			i2c = &(I2C_0_DEV);
//         break;
// 	#endif
//         default:
//             return;
//     }
//     while (i2c->SYNCBUSY.reg);
// 	i2c->CTRLA.reg &= ~SERCOM_I2CM_CTRLA_ENABLE;
// 	// perhaps a sw reset should be here as well??
// 	return;
// }

static void _start(SercomI2cm *dev, uint8_t address, uint8_t rw_flag)
{
	uint32_t timeout_counter = 0;
	uint16_t buffer_timeout = 65535;

	//Wait for hardware module to sync
	DEBUG("start: Wait for device to be ready\n");
	while(dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
	
	/* Set action to ACK. */
	dev->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;
	
	/* Send Start | Address | Write/Read   */
	DEBUG("start: Generate start condition by sending address\n");
	dev->ADDR.reg = (address << 1) | rw_flag;// | (0 << SERCOM_I2CM_ADDR_HS_Pos);

	/* Wait for response on bus. */
	while (!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) &&	!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) 
	{
		if (++timeout_counter >= buffer_timeout) 
		{
			DEBUG("start: STATUS_ERR_TIMEOUT\n");
			return;
		}
	}

	/* Check for address response error unless previous error is detected. */
	/* Check for error and ignore bus-error; workaround for BUSSTATE stuck in BUSY */
	if (dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB) 
	{
		/* Clear write interrupt flag */
		dev->INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;
		/* Check arbitration. */
		if (dev->STATUS.reg & SERCOM_I2CM_STATUS_ARBLOST) 
		{
			DEBUG("start: STATUS_ERR_PACKET_COLLISION\n");
			return;
		}
	/* Check that slave responded with nack. */
	} 
	else if (dev->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) 
	{
		/* Slave busy. Issue ack and stop command. */
		dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		DEBUG("start: STATUS_ERR_BAD_ADDRESS\n");
		return;
	}

}

static inline void _write(SercomI2cm *dev, char *data, int length)
{
	uint16_t tmp_data_length = length;
	uint16_t buffer_timeout = 65535;
	uint32_t timeout_counter = 0;
	uint16_t buffer_counter = 0;
	
	/* Write data buffer until the end. */
	//DEBUG("Looping through bytes\n");
	while (tmp_data_length--) 
	{
		if (!(dev->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(2))) 
		{   /* Check that bus ownership is not lost. */
			DEBUG("write: STATUS_ERR_PACKET_COLLISION\n");
			return;
		}

		//Wait for hardware module to sync
		while(dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
		
		DEBUG("Written %i byte to data reg, now waiting for DR to be empty again\n", buffer_counter);
		dev->DATA.reg = data[buffer_counter++];

		//hwtimer_wait(HWTIMER_TICKS_TO_US(20));

		DEBUG("write: Wait for response.\n");
		while (!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) &&
				!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) 
		{
			if (++timeout_counter >= buffer_timeout) {
				DEBUG("write: STATUS_ERR_TIMEOUT\n");
				return;
			}
		}

		/* Check for NACK from slave. */
		if (dev->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) 
		{
			DEBUG("write: STATUS_ERR_OVERFLOW\n");
			return;
		}
	}

	//Perhaps STOP is needed???
		// if (module->send_stop) {
		// 	/* Stop command */
		// 	_i2c_master_wait_for_sync(module);
		// 	i2c_module->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		// }
}

static inline void _read(SercomI2cm *dev, char *data, int length)
{
	uint32_t timeout_counter = 0;
	uint16_t buffer_timeout = 65535;
	uint8_t count = 0;

	/* Set action to ack. */
	dev->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

	/* Read data buffer. */
	while (count != length) {
		if (!(dev->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(2))) {   /* Check that bus ownership is not lost. */
			DEBUG("STATUS_ERR_PACKET_COLLISION");
			return;
		}

		/*Wait for hardware module to sync*/
		while(dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
		/* Save data to buffer. */
		data[count] = dev->DATA.reg;
		
		/* Wait for response. */
		timeout_counter = 0;
		while (!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) &&	!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) {
			if (++timeout_counter >= buffer_timeout) {
				DEBUG("STATUS_ERR_TIMEOUT");
				return;
			}
		}
		count++;
	}
	/*Send NACK before STOP*/
	dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
}

static inline void _stop(SercomI2cm *dev)
{
	//Wait for hardware module to sync
	while(dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
	/* Stop command */
	dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
	DEBUG("Stop sent\n");
}

void Set_Pin_Mux_Config(int padx_muxpos)
{
	/* Pin pad0 configuration */
	uint32_t pin_mask = (1UL << ((padx_muxpos >> 16) % 32));

	uint32_t pin_cfg = 0;

	if (!I2C_PINMUX_POWERSAVE) 
	{
			/* Enable the pin peripheral MUX flag if non-GPIO selected (pinmux will
		 * be written later) and store the new MUX mask */
		if (padx_muxpos != I2C_PINMUX_MUXPOSITION) {
			pin_cfg |= PORT_WRCONFIG_PMUXEN;
			pin_cfg |= (padx_muxpos << PORT_WRCONFIG_PMUX_Pos);
		}

			/* Check if the user has requested that the input buffer be enabled */
		if ((I2C_PINMUX_DIRECTION == 0 /*SYSTEM_PINMUX_PIN_DIR_INPUT*/) ||
				(I2C_PINMUX_DIRECTION == 2 /*SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK*/)) {
			/* Enable input buffer flag */
			pin_cfg |= PORT_WRCONFIG_INEN;

			/* Enable pull-up/pull-down control flag if requested */
			if (I2C_PINMUX_INPUT_PULL != 0/*SYSTEM_PINMUX_PIN_PULL_NONE*/) {
				pin_cfg |= PORT_WRCONFIG_PULLEN;
			}

			/* Clear the port DIR bits to disable the output buffer */
			I2C_PINMUX_DEV.DIRCLR.reg = pin_mask;
		}

		/* Check if the user has requested that the output buffer be enabled */
		if ((I2C_PINMUX_DIRECTION == 1/*SYSTEM_PINMUX_PIN_DIR_OUTPUT*/) ||
				(I2C_PINMUX_DIRECTION == 2/*SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK*/)) {
			/* Cannot use a pullup if the output driver is enabled,
			 * if requested the input buffer can only sample the current
			 * output state */
			pin_cfg &= ~PORT_WRCONFIG_PULLEN;
		}
	}
	else 
	{
		I2C_PINMUX_DEV.DIRCLR.reg = pin_mask;
	}

		/* The Write Configuration register (WRCONFIG) requires the
	 * pins to to grouped into two 16-bit half-words - split them out here */
	uint32_t lower_pin_mask = (pin_mask & 0xFFFF);
	uint32_t upper_pin_mask = (pin_mask >> 16);


		/* Configure the lower 16-bits of the port to the desired configuration,
	 * including the pin peripheral multiplexer just in case it is enabled */
	I2C_PINMUX_DEV.WRCONFIG.reg
		= (lower_pin_mask << PORT_WRCONFIG_PINMASK_Pos) |
			pin_cfg | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_WRPINCFG;

	/* Configure the upper 16-bits of the port to the desired configuration,
	 * including the pin peripheral multiplexer just in case it is enabled */
	I2C_PINMUX_DEV.WRCONFIG.reg
		= (upper_pin_mask << PORT_WRCONFIG_PINMASK_Pos) |
			pin_cfg | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_WRPINCFG |
			PORT_WRCONFIG_HWSEL;

	if(!I2C_PINMUX_POWERSAVE) {
		/* Set the pull-up state once the port pins are configured if one was
		 * requested and it does not violate the valid set of port
		 * configurations */
		if (pin_cfg & PORT_WRCONFIG_PULLEN) {
			/* Set the OUT register bits to enable the pullup if requested,
			 * clear to enable pull-down */
			if (I2C_PINMUX_INPUT_PULL == 1 /*SYSTEM_PINMUX_PIN_PULL_UP*/) {
				I2C_PINMUX_DEV.OUTSET.reg = pin_mask;
			} else {
				I2C_PINMUX_DEV.OUTCLR.reg = pin_mask;
			}
		}

		/* Check if the user has requested that the output buffer be enabled */
		if ((I2C_PINMUX_DIRECTION == 1 /*SYSTEM_PINMUX_PIN_DIR_OUTPUT*/) ||
				(I2C_PINMUX_DIRECTION == 2/*SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK*/)) {
			/* Set the port DIR bits to enable the output buffer */
			I2C_PINMUX_DEV.DIRSET.reg = pin_mask;
		}
	}
}

#endif /* I2C_0_EN */