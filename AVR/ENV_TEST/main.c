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
	if (!ccs811_init())
		for (;;);
}

int main()
{
	init();

	_delay_ms(2000);
	puts("Hello, world!");

#if 0
	si7021_write_heater(0x80 | 0x0f);
#else
	si7021_write_heater(0);
#endif

loop:
#if 0	// AM2302
	printf(ESC_RED "AM2302\t");
	struct am2302_data amdata = am2302_read();
	printf("| RH:\t%u.%u\t%u\t", amdata.rh / 10, amdata.rh % 10, amdata.rh);
	printf("| TEMP:\t%u.%u\t%u\n", amdata.temp / 10, amdata.temp % 10, amdata.temp);
	//printf("Parity: 0x%02x, %s\n", amdata.parity, am2302_check(&amdata) ? "passed" : "failed");
#endif	// AM2302

#if 1	// Si7021
	printf(ESC_GREEN "Si7021\t");
	//printf("USER1:\t0x%02x\n", si7021_read_user1());
	static struct si7021_data_t sidata;
	sidata = si7021_read();
	struct {
		float rh, temp;
	} sidata_f = {
		(float)sidata.rh * 125. / 65536. - 6.,
		(float)sidata.temp * 175.72 / 65536. - 46.85,
	};
	printf("| RH:\t%g\t%u\t", sidata_f.rh, sidata.rh);
	printf("| TEMP:\t%g\t%u\n", sidata_f.temp, sidata.temp);

#if 0
	if (!ccs811_write_envdata(sidata_f.rh, sidata_f.temp))
		puts("ccs811_write_envdata failed");
	printf("Error ID: %02x\n", ccs811_read_error());
	ccs811_test();
#endif
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

#if 1	// CCS811
	if (ccs811_poll()) {
		printf(ESC_BLUE "CCS811\t");
		static struct ccs811_data_t ccsdata;
		ccs811_read_data(&ccsdata);
		ccs811_write_envdata(sidata_f.rh, sidata_f.temp);

		//uint8_t err = ccs811_error();
		//if (!err) {
			printf("| eCO2:\t%u\t\t", ccsdata.eco2);
			printf("| TVOC:\t%u\t\t", ccsdata.tvoc);
			//printf("| STAT:\t0x%02x\t\t", ccsdata.status);
			//ccs811_test();
			uint8_t error = ccs811_read_error();
			printf("| error status:\t0x%02x\t\n", error);
#if 0
			uint8_t *ptr = (uint8_t *)&ccsdata;
			uint8_t i;
			for (i = sizeof(data); i != 0; i--)
				printf("0x%02x, ", *ptr++);
			putchar('\n');
#endif
		//}
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
	_delay_ms(500);
	goto loop;
}
