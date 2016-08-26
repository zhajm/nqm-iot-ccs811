#include <avr/io.h>
#include <util/delay.h>
#include "am2302.h"

#define PIN	0

#define HIGH()		PORTB |= _BV(PIN)
#define LOW()		PORTB &= ~_BV(PIN)
#define MODE_WR()	DDRB |= _BV(PIN)
#define MODE_RD()	DDRB &= ~_BV(PIN)
#define READ()		(PINB & _BV(PIN))

static uint8_t start()
{
	while (!READ());
	LOW();
	MODE_WR();
	_delay_ms(1);	// Tbe
	MODE_RD();
	HIGH();
	_delay_us(30);	// Tgo
	uint8_t i = 20;	// Wait for response for 200us
	while (i--) {
		if (!READ()) {
			while (!READ());	// Trel
			while (READ());		// Treh
			return 1;
		}
		_delay_us(10);
	}
	return 0;
}

static uint8_t read()
{
	uint8_t data = 0;
	uint8_t i = 8;
	while (i--) {
		while (!READ());
		// Delay conditions:
		// > TH0_MAX
		// < TH1_MIN
		// < TH0_MIN + TLOW_MIN
		_delay_us(50);
		data <<= 1;
		data |= !!READ();
		while (READ());
	}
	return data;
}

struct am2302_data am2302_read()
{
	struct am2302_data data = {0, 0, 0xff};
	if (!start())
		return data;
	data.rh = (uint16_t)read() << 8;
	data.rh |= read();
	data.temp = (uint16_t)read() << 8;
	data.temp |= read();
	data.parity = read();
	return data;
}

uint8_t am2302_check(struct am2302_data *ptr)
{
	uint8_t parity = 0;
	uint8_t *ptru8 = (uint8_t *)ptr;
	uint8_t i = 4;
	while (i--)
		parity += *ptru8++;
	return parity == ptr->parity;
}
