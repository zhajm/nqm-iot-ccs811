#include <avr/io.h>
#include <util/delay.h>
#include "uart0.h"
#include "twi.h"

void init()
{
	uart0_init();
	stdout = uart0_fd();

	twi_init();
}

int main()
{
	init();

	_delay_ms(3000);
	puts("Hello, world!");

	uint8_t x, y;
loop:
	printf("\n*** I2C ADDRESS SCAN ***\n  ");
	for (x = 0; x != 0x10; x++)
		printf("%01x ", x);

	putchar('\n');
	for (y = 0; y != 0x08; y++) {
		printf("%01x ", y);
		for (x = 0; x != 0x10; x++) {
			uint8_t ack = twi_scan(((y << 4) | x) << 1);
			putchar(ack ? 'x' : '-');
			putchar(' ');
		}
		putchar('\n');
	}

	_delay_ms(1000);
	goto loop;
}
