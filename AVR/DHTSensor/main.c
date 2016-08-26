#include <avr/io.h>
#include <util/delay.h>
#include "ccs811.h"
#include "si7021.h"
#include "am2302.h"
#include "uart0.h"
#include "twi.h"
#include "escape.h"

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

	_delay_ms(3000);
	puts("Hello, world!");

#if 0
	si7021_write_heater(0x80 | 0x0f);
#else
	si7021_write_heater(0);
#endif

loop:
#if 1	// AM2302
	puts(ESC_RED "\n*** AM2302 ***");
	struct am2302_data data = am2302_read();
	printf("RH:\t%u\t| %u.%u\n", data.rh, data.rh / 10, data.rh % 10);
	printf("TEMP:\t%u\t| %u.%u\n", data.temp, data.temp / 10, data.temp % 10);
	printf("Parity: 0x%02x, %s\n", data.parity, am2302_check(&data) ? "passed" : "failed");
#endif	// AM2302
#if 1	// Si7021
	puts(ESC_GREEN "\n*** Si7021 ***");
	printf("USER1:\t0x%02x\n", si7021_read_user1());
	struct si7021_data_t res = si7021_read();
	printf("RH:\t%u\t| %g\n", res.rh, (float)res.rh * 125. / 65536. - 6.);
	printf("TEMP:\t%u\t| %g\n", res.temp, (float)res.temp * 175.72 / 65536. - 46.85);

#if 0	// Si7021 SN
	printf("SN:\t");
	uint64_t sn = si7021_read_sn();
	uint8_t *ptr = (uint8_t *)&sn + 7;
	uint8_t i = 8;
	while (i--)
		printf("0x%02x, ", *ptr--);
	putchar('\n');
#endif	// Si7021 SN
#endif	// Si7021
#if 0	// CCS811
	puts(ESC_BLUE "\n*** CCS811 ***");
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
#endif	// CCS811
#if 0	// I2C scan
	uint8_t x, y;
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
#endif	// I2C scan
	_delay_ms(2000);
	goto loop;
}
