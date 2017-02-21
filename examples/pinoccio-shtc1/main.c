
#include "sht11.h"
#include <util/twi.h>
#include <avr/io.h>
#include "xtimer.h"
#include <stdio.h>

int main (void)
{
	DDRD |= (1<<PD0);
	DDRE |= (1<<PE4);
	PORTE &= ~(1<<PE4);
	xtimer_sleep(1);
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
