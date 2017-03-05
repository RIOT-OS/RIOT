
#include "shtc1.h"
#include "xtimer.h"
#include "periph_conf.h"
#include <stdio.h>
#include "periph/i2c.h"

int main (void)
{
	//DDRD |= (1<<PD0);
	//DDRE |= (1<<PE4);
	//PORTE &= ~(1<<PE4);

	/*int8_t error =*/ shtc1_init(I2C_0);

	//xtimer_sleep(2);

	uint16_t id = 0;
	shtc1_id(I2C_0, &id);

	printf("%u \n",id);


	shtc1_values_t my_values;

	shtc1_measure(I2C_0, &my_values);

	printf("Temp=%.2f \n Rel_humidity=%.2f \n", my_values.temp, my_values.rel_humidity);

/*	uint8_t data[] = { 0xEF, 0xC8 };
	uint8_t received[3];
	//i2c_init_master(I2C_0, I2C_SPEED_FAST);
	i2c_write_bytes(I2C_0, 0x70, data, 2);
	i2c_read_bytes(I2C_0, 0x70, received, 3);
	printf("Error: %i \n", error);
	printf("%u \n", received[0]);
	printf("%u \n", received[1]);
	printf("%u \n", received[2]); */
/*	xtimer_sleep(1);
	TWSR = 0x00;
	TWBR = 32;
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_START)
		printf("error");
	TWDR = (0x70<<1) + TW_WRITE;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT))); //ACK/NACK should be received now
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
		printf("Slave didnt ACK Adress %u\n", (TWSR & 0xF8));
	TWDR = 0xEF;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
		printf("Slave didnt ACK MSB %u \n",(TWSR & 0xF8));
	TWDR = 0xC8;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
		printf("Slave didnt ACK LSB  %u\n", (TWSR & 0xF8));
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

	xtimer_sleep(1);


	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_START)
		printf("error");
	TWDR = (0x70<<1)+TW_READ;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT))); //ACK/NACK should be received now
	if ((TWSR & 0xF8) != TW_MR_SLA_ACK)
		printf("Slave didnt ACK Adress-read %u\n",(TWSR & 0xF8));
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	printf("%u \n", TWDR);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	printf("%u \n", TWDR);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	printf("CRC: %u \n", TWDR);
*/
	//sht11_val_t sht11;
	//sht11_init();
	/*uint8_t value;
	uint8_t checksum;
	uint8_t result = sht11_read_status(&value, &checksum);
	printf("read status %u \n", result);
	printf("result: %u", result); */
	//uint8_t result = sht11_read_sensor(&sht11, HUMIDITY|TEMPERATURE);
	//printf("Result %u Temp: %f",result, sht11.temperature);
	//printf("%-6.2f °C %5.2f %% %5.2f %%\n", sht11.temperature, sht11.relhum, sht11.relhum_temp);


	return 0;
}
