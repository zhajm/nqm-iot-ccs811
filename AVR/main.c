#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart0.h"
#include "ccs811.h"
#include "si7021.h"
#include "i2c.h"
#include "escape.h"

int main()
{
	//puts("Power on");

	//Initialise
	uart0_init();
	puts("Power on");

	i2c_init();

	while (!(ccs811_init())) {
		puts("Info: Sensor Initialise failed");
		_delay_ms(200);
	}

	puts("Info: Sensor Initialise success");
	
	uint16_t fw; 
	i2c_read_data(0x5a, 0x24, 2, (void *)&fw);
	printf("Info: FW: %04x\r\n", swap_u16(fw));

	//Loop
	for (;;)
	{
		//SI7021
		//printf(ESC_GREEN "\nSi7021\t");
		static struct si7021_data_t sidata;
		sidata = si7021_read();
		struct {
			float rh, temp;
		} sidata_f = {
			(float)sidata.rh * 125. / 65536. - 6.,
			(float)sidata.temp * 175.72 / 65536. - 46.85,
		};

		printf("H: %g\r\n", sidata_f.rh);
		printf("T: %g\r\n", sidata_f.temp);

		//CCS811
		while (!ccs811_poll());
		//printf(ESC_BLUE "\nCCS811\t");
		static struct ccs811_data_t ccsdata;

		ccs811_read_data(&ccsdata);
		ccs811_write_envdata(sidata_f.rh, sidata_f.temp);

		printf("C: %u\r\n", ccsdata.eco2);
		printf("V: %u\r\n", ccsdata.tvoc);
		//printf("status: 0x%02x\n", ccsdata.status);
#if 0				
		uint8_t *ptr = (uint8_t *)&data4;
		uint8_t i;

		for (i = sizeof(data4); i != 0; i--)
			printf("0x%02x, ", *ptr++);
		putchar('\n');
#endif
	}
	_delay_ms(8000);
}
