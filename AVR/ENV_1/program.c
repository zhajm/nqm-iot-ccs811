#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "ccs811.h"
#include "uart0.h"
#include "twi.h"
#include "escape.h"
#include "firmware.h"

void init()
{
	uart0_init();
	stdout = uart0_fd();

	twi_init();
}

int main()
{
	init();

	_delay_ms(2000);
	puts_P(PSTR("Hello, world!"));

	printf_P(PSTR(ESC_WHITE "Current firmware version: 0x%04x\n"), ccs811_read_fw_version());
	printf_P(PSTR(ESC_WHITE "New firmware: %u bytes\n"), sizeof(firmware));

	puts_P(PSTR(ESC_CYAN "Press enter to continue..."));
	char c;
	do
		c = getchar();
	while (c != '\n' && c != '\r');

	uint8_t res;
	// Reset CCS811
reset:
	do {
		puts_P(PSTR(ESC_YELLOW "Reseting..."));
		res = ccs811_reset();
		_delay_ms(100);
	} while (!res);

	// Check mode
	if (ccs811_read_status() & CCS811_FW_MODE) {
		puts_P(PSTR(ESC_RED "Error: sensor is still in application mode"));
		goto reset;
	} else
		puts_P(PSTR(ESC_GREEN "Entered bootloader mode"));

	// Erase application code
	do {
		printf_P(PSTR(ESC_YELLOW "Erasing application code."));
		res = ccs811_erase();
		_delay_ms(300);
	} while (!res);

	while (!(ccs811_read_status() & CCS811_ERSE_READY)) {
		putchar('.');
		_delay_ms(200);
	}
	putchar('\n');

	// Program application code
	puts_P(PSTR(ESC_YELLOW "Programing..."));
	PGM_VOID_P ptr = firmware;
	uint16_t size = sizeof(firmware), cnt = 0;
	uint8_t i;
program:
	i = size >= 8 ? 8 : size;
	if (i == 0) {
		putchar('\n');
		goto verify;
	}
	ccs811_program_P(ptr, i);
	ptr += i;
	size -= i;
	cnt += i;
	printf_P(PSTR("(%u/%u)...\r"), cnt, sizeof(firmware));
	goto program;

	// Verify application code
verify:
	do {
		printf_P(PSTR(ESC_YELLOW "Verifying application code."));
		res = ccs811_verify();
		_delay_ms(300);
	} while (!res);

	while (!(ccs811_read_status() & CCS811_VERIFY_READY)) {
		putchar('.');
		_delay_ms(200);
	}
	putchar('\n');

	if (ccs811_read_status() & CCS811_APP_VALID) {
		puts_P(PSTR(ESC_GREEN "Application code valid"));
		printf_P(PSTR(ESC_WHITE "Firmware version: 0x%04x\n"), ccs811_read_fw_version());
	} else
		puts_P(PSTR(ESC_RED "Application code invalid"));

	for (;;);
}
