#include "periph/i2c.h"
#include "periph_conf.h"
#include "debug.h"
#include <inttypes.h>
#include <avr/io.h>
#include <util/twi.h>

#define ENABLE_DEBUG 	(1U)


uint8_t _twbr_values(int speed)
{
	return ((16*speed-CLOCK_CORECLOCK)/(-2*speed));
}

uint32_t speeds[5] = {10000, 100000, 400000, 1000000, 3400000};

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
	DEBUG("I2C: Starting i2c master init");
	if(speed >4)
		return -2;
	if(dev >= I2C_NUMOF)
		return -1;
	/* Set up gpio pins*/
	//gpio_init(i2c_config[dev].scl_pin, )

	/* Setup a master mask for later */
	*(i2c_config[dev].mask) = (1<<TWEN)|(1<<TWINT);
	/* clear status register */
	TWSR = 0x00;
	/* set speed speed*/
	TWBR = _twbr_values(speeds[speed]);

	return 0;
}

int i2c_acquire(i2c_t dev)
{
	int result = mutex_trylock(i2c_config[dev].used);
	if(!result)
		/*mutex already locked */
		return -1;
	/*mutex now locked and i2c aquired */
	return 0;
}

int i2c_release(i2c_t dev)
{
	mutex_unlock(i2c_config[dev].used);
	return 0;
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
	if(!i2c_acquire(dev))
		DEBUG("MUTEX ERROR");
	if(dev >= I2C_NUMOF)
		return -1;

	TWCR = *(i2c_config[dev].mask)|(1<<TWSTA);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_START)
		DEBUG("I2C: error on sending Start");
	TWDR = (address<<1) + TW_WRITE;
	TWCR = *(i2c_config[dev].mask);
	while (!(TWCR & (1<<TWINT))); //ACK/NACK should be received now
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
		DEBUG("I2C: Slave didnt ACK Adress Error:%u\n", (TWSR & 0xF8));
	TWDR = data;
	TWCR = *(i2c_config[dev].mask);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
		DEBUG("I2C: Slave didnt ACK BYTE Error:%u \n",(TWSR & 0xF8));
	i2c_release(dev);
	return 0;
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
	if(!i2c_acquire(dev))
		DEBUG("MUTEX ERROR");
	if(dev >= I2C_NUMOF)
		return -1;

	uint8_t *my_data = (uint8_t*) data;
	TWCR = *(i2c_config[dev].mask)|(1<<TWSTA);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_START)
		DEBUG("I2C: error on sending Start");
	TWDR = (address<<1) + TW_WRITE;
	TWCR = *(i2c_config[dev].mask);
	while (!(TWCR & (1<<TWINT))); //ACK/NACK should be received now
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
		DEBUG("I2C: Slave didnt ACK Adress Error:%u\n", (TWSR & 0xF8));
	for(int i = 0; i <length; i++) {
		TWDR = my_data[i];
		TWCR = *(i2c_config[dev].mask);
		while (!(TWCR & (1<<TWINT)));
		if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
			DEBUG("I2C: Slave didnt ACK BYTE Error:%u  on Byte %u\n",(TWSR & 0xF8), i);
	}
	i2c_release(dev);
	return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address, void *data)
{
	if(!i2c_acquire(dev))
			DEBUG("MUTEX ERROR");
	if(dev >= I2C_NUMOF)
			return -1;
	TWCR = *(i2c_config[dev].mask)|(1<<TWSTA);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_START)
		DEBUG("I2C: error on sending START");
	TWDR = (address<<1)|TW_READ;
	TWCR = *(i2c_config[dev].mask);
	while (!(TWCR & (1<<TWINT))); //ACK/NACK should be received now
	if ((TWSR & 0xF8) != TW_MR_SLA_ACK) {
		DEBUG("I2C: Slave didnt ACK Adress-read %u\n",(TWSR & 0xF8));
		return -2;
	}
	TWCR = *(i2c_config[dev].mask)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	*((uint8_t*)data) = TWDR;
	i2c_release(dev);
	return 1;
}

int i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length)
{
	if(!i2c_acquire(dev))
			DEBUG("MUTEX ERROR");
	if(dev >= I2C_NUMOF)
			return -1;

	uint8_t *my_data = (uint8_t*) data;
	TWCR = *(i2c_config[dev].mask)|(1<<TWSTA);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_START)
		DEBUG("I2C: error on sending START");
	TWDR = (address<<1)|TW_READ;
	TWCR = *(i2c_config[dev].mask);
	while (!(TWCR & (1<<TWINT))); //ACK/NACK should be received now
	if ((TWSR & 0xF8) != TW_MR_SLA_ACK) {
		DEBUG("I2C: Slave didnt ACK Adress-read %u\n",(TWSR & 0xF8));
		return -2;
	}
	for(int i=0; i<length; i++){
		TWCR = *(i2c_config[dev].mask)|(1<<TWEA);
		while (!(TWCR & (1<<TWINT)));
		if ((TWSR & 0xF8) != TW_MR_DATA_ACK)
			DEBUG("I2C: Error receiving Data from Slave Errorcode: %u", (TWSR & 0xF8));
		my_data[i] = TWDR;
	}
	i2c_release(dev);
	return 1;
}

