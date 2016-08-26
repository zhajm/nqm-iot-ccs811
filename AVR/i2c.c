#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void i2c_init()
{
	TWSR = 0x00;
	TWBR = 0x20;				//16M/(16 + 2*32*1) = 200k
}

void i2c_start()
{
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);	//clear flag,start,enable
	while (!(TWCR & _BV(TWINT)));			//wait start transmitted
}

uint8_t i2c_write(unsigned char data)
{
	TWDR = data;					//put data into TWDR
	TWCR = _BV(TWINT) | _BV(TWEN);			//transmission of data
	while (!(TWCR & _BV(TWINT)));			//check TWINT flag, ACK
	uint8_t s = TWSR & 0xf8;
	return s == 0x40 || s == 0x18 || s == 0x28;
}

uint8_t i2c_read(uint8_t ack)
{
	if (ack)
		TWCR = _BV(TWINT) | _BV(TWEA) | _BV (TWEN);	//ack
	else
		TWCR = _BV(TWINT) | _BV(TWEN);
	
	while (!(TWCR & _BV(TWINT)));			//wait transmission
	return TWDR;					//return data
}

void i2c_stop()
{
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);	//clear flag, stop, enable
	while (TWCR & _BV(TWSTO));
	_delay_us(20);
}

uint8_t i2c_write_data(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *ptr)
{
	uint8_t err = 0;
	i2c_start();
	err += !i2c_write(addr << 1);
	err += !i2c_write(reg);
	while (len--)
		err += !i2c_write(*ptr++);
	i2c_stop();
	return !err;
}

uint8_t i2c_read_data(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *ptr)
{
	uint8_t err = 0;
	i2c_start();
	err += !i2c_write(addr << 1);
	err += !i2c_write(reg);

	i2c_start();
	err += !i2c_write((addr << 1) | 1U);
	if (err) {
		i2c_stop();
		return err;
	}

	while (len--)
		*ptr++ = i2c_read(len);
	i2c_stop();
	return !err;
}	
