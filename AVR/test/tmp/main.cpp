#include <avr/io.h>
#include <util/delay.h>
#include "ccs811.h"
#include "uart0.h"
#include "twi.h"

void init()
{
	uart0_init();
	stdout = uart0_fd();

	twi_init();
	ccs811_init();
}

int main()
{
	init();

	_delay_ms(2000);
	puts("Hello, world!");

#if 1
loop:
	if (ccs811_poll()) {
		static struct ccs811_data_t data;
		ccs811_read_data(&data);

		uint8_t err = ccs811_error();
		if (!err) {
			printf("eCO2: %u\n", data.eco2);
			printf("TVOC: %u\n", data.tvoc);
			printf("status: 0x%02x\n", data.status);

			uint8_t *ptr = (uint8_t *)&data;
			uint8_t i;
			for (i = sizeof(data); i != 0; i--)
				printf("0x%02x, ", *ptr++);
			putchar('\n');
		}
	}

	_delay_ms(100);
	goto loop;

#else
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
#endif
}
