#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "twi.h"

#define TWI_BAUD	400000UL

//#define dbg(str)	puts(str)
#define dbg(str)

void twi_init()
{
	//PORTC |= _BV(0) | _BV(1);
	TWBR = (F_CPU / TWI_BAUD - 16) / 2 / 1;
	TWSR = 0;
}

static inline uint8_t twi_ready()
{
	return TWCR & _BV(TWINT);
}

void twi_start()
{
	dbg(__func__);
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while (!twi_ready());
}

void twi_stop()
{
	dbg(__func__);
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	_delay_us(5);
}

uint8_t twi_write(uint8_t data)
{
	dbg(__func__);
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while (!twi_ready());
	uint8_t s = TWSR & 0xf8;
	return s == 0x40 || s == 0x18 || s == 0x28;
}

uint8_t twi_read(uint8_t ack)
{
	dbg(__func__);
	TWCR = _BV(TWINT) | (ack ? _BV(TWEA) : 0) | _BV(TWEN);
	while (!twi_ready());
	return TWDR;
}

uint8_t twi_scan(uint8_t addr)
{
	dbg(__func__);
	twi_start();
	uint8_t ack = twi_write(addr);
	twi_stop();
	return ack;
}
